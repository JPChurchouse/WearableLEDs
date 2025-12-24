#include "INMP441Audio.hpp"

// ------------------------
// Constructor
// ------------------------
INMP441Audio::INMP441Audio(int bclkPin, int lrclkPin, int dataPin,
                           uint16_t sampleCount, uint32_t sampleRate)
: _bclkPin(bclkPin),
  _lrclkPin(lrclkPin),
  _dataPin(dataPin),
  _samples(sampleCount),
  _sampleRate(sampleRate)
{
    vReal = new double[_samples];
    vImag = new double[_samples];
    _fft = new ArduinoFFT<double>(vReal, vImag, _samples, _sampleRate);
}

// ------------------------
// Begin (I²S initialisation)
// ------------------------
void INMP441Audio::begin() {
    i2s_config_t config = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
        .sample_rate = _sampleRate,
        .bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT,
        .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
        .communication_format = I2S_COMM_FORMAT_I2S,
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
        .dma_buf_count = 4,
        .dma_buf_len = 256,
        .use_apll = false,
        .tx_desc_auto_clear = false,
        .fixed_mclk = 0
    };

    i2s_pin_config_t pins = {
        .bck_io_num = _bclkPin,
        .ws_io_num  = _lrclkPin,
        .data_out_num = -1,
        .data_in_num  = _dataPin
    };

    i2s_driver_install(_port, &config, 0, NULL);
    i2s_set_pin(_port, &pins);
    i2s_zero_dma_buffer(_port);
}

// ------------------------
// Update (Capture + FFT)
// ------------------------
void INMP441Audio::update() {
    size_t bytes_read = 0;

    for (uint16_t i = 0; i < _samples; i++) {
        int32_t raw = 0;
        i2s_read(_port, &raw, sizeof(int32_t), &bytes_read, portMAX_DELAY);

        // INMP441 outputs 24-bit samples left-justified in 32 bits
        vReal[i] = (double)(raw >> 8);
        vImag[i] = 0;
    }

    // FFT
    _fft->windowing(FFT_WIN_TYP_HAMMING, FFT_FORWARD);
    _fft->compute(FFT_FORWARD);
    _fft->complexToMagnitude();

    // ----------------------
    // Volume (RMS)
    // ----------------------
    double acc = 0;
    for (uint16_t i = 0; i < _samples; i++) {
        acc += vReal[i] * vReal[i];
    }
    volume = sqrt(acc / _samples);

    // ----------------------
    // Dominant Frequency
    // ----------------------
    uint16_t peakIndex = 1;
    double peakVal = 0;

    for (uint16_t i = 1; i < _samples / 2; i++) {
        if (vReal[i] > peakVal) {
            peakVal = vReal[i];
            peakIndex = i;
        }
    }

    dominantFreq = (peakIndex * _sampleRate) / _samples;

    // ----------------------
    // Bass (< 150 Hz)
    // ----------------------
    double bassSum = 0;

    for (uint16_t i = 2; i < _samples / 2; i++) {
        double freq = (i * _sampleRate) / _samples;
        if (freq < 150) bassSum += vReal[i];
    }

    bass = bassSum / 20.0;   // tuneable
}

// ------------------------
// Getters
// ------------------------
double INMP441Audio::getVolume() const { return volume; }
double INMP441Audio::getBass() const { return bass; }
double INMP441Audio::getDominantFreq() const { return dominantFreq; }
