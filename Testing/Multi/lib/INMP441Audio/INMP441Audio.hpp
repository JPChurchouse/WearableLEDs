#pragma once
#include <Arduino.h>
#include <driver/i2s.h>
#include <arduinoFFT.h>

class INMP441Audio {
public:
    INMP441Audio(int bclkPin, int lrclkPin, int dataPin,
                 uint16_t sampleCount = 1024,
                 uint32_t sampleRate = 16000);

    void begin();
    void update();              // Capture + FFT

    double getVolume() const;   // RMS amplitude
    double getBass() const;     // Energy < 150Hz
    double getDominantFreq() const;

private:
    int _bclkPin, _lrclkPin, _dataPin;
    uint16_t _samples;
    uint32_t _sampleRate;

    i2s_port_t _port = I2S_NUM_0;

    double *vReal;
    double *vImag;

    ArduinoFFT<double> *_fft;

    double volume = 0;
    double bass = 0;
    double dominantFreq = 0;
};
