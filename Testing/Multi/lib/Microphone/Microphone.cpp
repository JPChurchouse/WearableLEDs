#include "Microphone.hpp"

Microphone::Microphone(
    i2s_port_t i2sPort,
    int pinWS,
    int pinSCK,
    int pinSD,
    uint16_t samples,
    double sampleRate)
    : _port(i2sPort),
      _pinWS(pinWS),
      _pinSCK(pinSCK),
      _pinSD(pinSD),
      _samples(samples),
      _sampleRate(sampleRate)
{
    _vReal = new double[_samples];
    _vImag = new double[_samples];
    _fft = new ArduinoFFT<double>(_vReal, _vImag, _samples, _sampleRate);
}

void Microphone::begin()
{
    i2s_config_t config = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
        .sample_rate = (uint32_t)_sampleRate,
        .bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT,
        .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
        .communication_format = I2S_COMM_FORMAT_STAND_I2S,
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
        .dma_buf_count = 4,
        .dma_buf_len = 256,
        .use_apll = false,
        .tx_desc_auto_clear = false,
        .fixed_mclk = 0};

    i2s_pin_config_t pins = {
        .bck_io_num = _pinSCK,
        .ws_io_num = _pinWS,
        .data_out_num = I2S_PIN_NO_CHANGE,
        .data_in_num = _pinSD};

    i2s_driver_install(_port, &config, 0, nullptr);
    i2s_set_pin(_port, &pins);
    i2s_zero_dma_buffer(_port);
}

void Microphone::sample()
{
    size_t bytesRead = 0;
    uint32_t raw = 0;

    for (uint16_t i = 0; i < _samples; i++)
    {
        i2s_read(_port, &raw, sizeof(uint32_t), &bytesRead, portMAX_DELAY);

        // Convert I2S 32-bit sample to signed 24-bit
        int32_t sample = (int32_t)(raw >> 8);

        // Convert to double and centre around 0
        _vReal[i] = (double)sample;
        _vImag[i] = 0.0;
    }
}

void Microphone::computeFFT()
{
    _fft->windowing(FFT_WIN_TYP_HAMMING, FFT_FORWARD);
    _fft->compute(FFT_FORWARD);
    _fft->complexToMagnitude();
}

double Microphone::getBinMagnitude(uint16_t bin) const
{
    if (bin >= _samples / 2)
        return 0.0;
    return _vReal[bin];
}

double Microphone::getDominantFrequency() const
{
    uint16_t half = _samples / 2;
    uint16_t maxIndex = 1;
    double maxMag = 0;

    for (uint16_t i = 1; i < half; i++)
    {
        if (_vReal[i] > maxMag)
        {
            maxMag = _vReal[i];
            maxIndex = i;
        }
    }

    double resolution = _sampleRate / _samples;
    return maxIndex * resolution;
}
