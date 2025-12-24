#pragma once
#include <Arduino.h>

class DodgyRotaryEncoder
{
public:
    // Constructor: pins for A, B, and push button
    DodgyRotaryEncoder(uint8_t pinA, uint8_t pinB, uint8_t pinBtn, int32_t startValue = 0);

    // Call in setup()
    void begin();

    // Get current counter value
    int32_t getValue();

    // Get button state (true = pressed)
    bool isButtonPressed();

private:
    // ISR trampolines
    static void IRAM_ATTR isrA(void *arg);
    static void IRAM_ATTR isrB(void *arg);

    void IRAM_ATTR handleA();
    void IRAM_ATTR handleB();

    // Pins
    uint8_t _pinA, _pinB, _pinBtn;

    // Counter
    volatile int32_t _counter;

    // Timestamp of last edge for each pin
    volatile uint32_t _lastA;
    volatile uint32_t _lastB;

    // Debounce constants
    const uint32_t _debounce_us = 10000; // pin debounce
    const uint32_t _window_us = 100000; // max interval between pin edges for a valid step

    // Button debounce
    bool _lastButtonState;
    unsigned long _lastButtonMillis;
    const unsigned long _buttonDebounceMs = 30;
};
