#pragma once

#include <Arduino.h>

class Encoder
{
public:
    Encoder(uint8_t pinA, uint8_t pinB);

    void begin();

    // Returns accumulated movement since last call, then clears it
    int32_t getDelta();

private:
    static void IRAM_ATTR isrHandler(void *arg);

    void IRAM_ATTR handleInterrupt();

    uint8_t _pinA;
    uint8_t _pinB;

    volatile int32_t _delta = 0;
    volatile uint8_t _lastState = 0;
    volatile uint32_t _lastInterruptTime = 0;

    uint32_t _debounceUs = 100;
};
