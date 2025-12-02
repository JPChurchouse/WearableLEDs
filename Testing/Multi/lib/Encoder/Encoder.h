// Encoder.h
#pragma once
#include <Arduino.h>

class Encoder
{
private:
    uint8_t pinA, pinB, pinButton;
    volatile int32_t encoderValue;
    volatile bool valueChanged;
    volatile unsigned long lastInterruptTimeA;
    unsigned long debounceDelayButton;

    // Button handling
    bool lastButtonState;
    unsigned long lastButtonTime;
    bool buttonPressed;

    // Interrupt handler helper
    static void IRAM_ATTR handleInterruptA(Encoder *instance);

public:
    Encoder(uint8_t a, uint8_t b, uint8_t button, unsigned long buttonDebounceMs = 50);

    void begin();

    // Call in loop() to update button state
    void update();

    bool hasChanged();
    int32_t getValue();
    bool isButtonPressed();
    void reset();
};