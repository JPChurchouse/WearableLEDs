#pragma once
#include <Arduino.h>
#include <driver/i2s.h>

class INMP441 {
public:
    INMP441(i2s_port_t port = I2S_NUM_0);

    bool begin(int bclk, int ws, int data);
    bool readSamples(size_t sampleCount);
    float rms() const;
    int32_t peak() const;

private:
    i2s_port_t _port;
    float _rms;
    int32_t _peak;
};
