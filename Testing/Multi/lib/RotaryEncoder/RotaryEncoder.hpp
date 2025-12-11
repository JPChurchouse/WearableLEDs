#pragma once
#include <Arduino.h>

class RotaryEncoder {
public:
    struct State {
        bool changed;     // true if rotation or button changed
        int8_t direction; // -1, 0, +1 (one step)
        int32_t index;    // running counter
        bool button;      // true = pressed
    };

    RotaryEncoder(uint8_t pinA, uint8_t pinB, uint8_t pinBtn, int32_t startIndex = 0);

    // Call in setup()
    void begin();

    // Polling method — call in your loop(). Returns a snapshot State struct.
    State read();

private:
    // ISR trampoline (uses attachInterruptArg)
    static void IRAM_ATTR isrHandlerA(void* arg);
    static void IRAM_ATTR isrHandlerB(void* arg);

    // Called by ISR trampoline; runs in IRAM context
    void IRAM_ATTR handleInterrupt();

    uint8_t _pinA, _pinB, _pinBtn;

    // Flag set by ISR when movement happened
    volatile bool _movementFlag;

    // Decoder state
    uint8_t _lrmem;
    int _lrsum;
    int32_t _counter;

    // Button state for simple debounce
    bool _lastButtonState;
    unsigned long _lastButtonMillis;
    const unsigned long _buttonDebounceMs = 30;

    // TRANS lookup table as inline static constexpr to avoid linker issues
    inline static constexpr int8_t TRANS[16] = {
        0, -1, 1, 14,
        1, 0, 14, -1,
        -1, 14, 0, 1,
        14, 1, -1, 0
    };
};
