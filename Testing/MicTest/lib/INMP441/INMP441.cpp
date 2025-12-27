#include "INMP441.hpp"

INMP441::INMP441(i2s_port_t port)
: _port(port), _rms(0), _peak(0) {}

bool INMP441::begin(int bclk, int ws, int data) {

    i2s_config_t config = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
        .sample_rate = 44100,
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
        .bck_io_num = bclk,
        .ws_io_num = ws,
        .data_out_num = I2S_PIN_NO_CHANGE,
        .data_in_num = data
    };

    if (i2s_driver_install(_port, &config, 0, nullptr) != ESP_OK)
        return false;

    if (i2s_set_pin(_port, &pins) != ESP_OK)
        return false;

    i2s_zero_dma_buffer(_port);
    return true;
}

bool INMP441::readSamples(size_t sampleCount) {

    int32_t samples[sampleCount];
    size_t bytesRead = 0;

    if (i2s_read(_port, samples, sizeof(samples), &bytesRead, portMAX_DELAY) != ESP_OK)
        return false;

    size_t count = bytesRead / sizeof(int32_t);
    if (count == 0) return false;

    int64_t sumSq = 0;
    _peak = 0;

    for (size_t i = 0; i < count; i++) {
        int32_t s = samples[i] >> 8;   // INMP441 left-justified
        sumSq += (int64_t)s * s;
        if (abs(s) > _peak) _peak = abs(s);
    }
int64_t sum = 0;

for (size_t i = 0; i < count; i++) {
    samples[i] >>= 14;   // convert to signed 24-bit
    sum += samples[i];
}

int32_t mean = sum / count;

  sumSq = 0;
_peak = 0;

for (size_t i = 0; i < count; i++) {
    int32_t s = samples[i] - mean;
    sumSq += (int64_t)s * s;
    if (abs(s) > _peak) _peak = abs(s);
}

_rms = sqrt((double)sumSq / count);

    return true;
}

float INMP441::rms() const {
    return _rms;
}

int32_t INMP441::peak() const {
    return _peak;
}
