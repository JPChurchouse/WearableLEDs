#pragma once

#include <Arduino.h>

class Button
{
public:
    Button(uint8_t pin);

    void begin();

    // Returns true once per press, then clears the flag
    bool wasPressed();

private:
    static void IRAM_ATTR isrHandler(void *arg);

    void IRAM_ATTR handleInterrupt();

    uint8_t _pin;
    volatile bool _pressedFlag = 0;
    volatile uint32_t _lastInterruptTime = 0;
    const uint32_t _debounceMs = 50;
};
