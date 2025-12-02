#pragma once
#include <Arduino.h>
#include <driver/i2s.h>
#include <arduinoFFT.h>

class Microphone
{
public:
    Microphone(
        i2s_port_t i2sPort,
        int pinWS,
        int pinSCK,
        int pinSD,
        uint16_t samples = 1024,
        double sampleRate = 16000.0);

    void begin();

    // Collects 'samples' frames from I2S mic
    void sample();

    // Run FFT on collected samples
    void computeFFT();

    // Return magnitude of a specific FFT bin
    double getBinMagnitude(uint16_t bin) const;

    // Estimate dominant frequency peak
    double getDominantFrequency() const;

    uint16_t getSampleCount() const { return _samples; }

private:
    i2s_port_t _port;
    int _pinWS, _pinSCK, _pinSD;
    uint16_t _samples;
    double _sampleRate;

    double *_vReal;
    double *_vImag;

    ArduinoFFT<double> *_fft;
};
