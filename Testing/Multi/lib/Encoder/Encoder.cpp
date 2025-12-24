#include "Encoder.h"

Encoder::Encoder(uint8_t a, uint8_t b, uint8_t button, unsigned long buttonDebounceMs)
{
    pinA = a;
    pinB = b;
    pinButton = button;
    encoderValue = 0;
    valueChanged = false;
    lastInterruptTimeA = 0;
    debounceDelayButton = buttonDebounceMs;
    lastButtonState = HIGH;
    lastButtonTime = 0;
    buttonPressed = false;
}

void IRAM_ATTR Encoder::handleInterruptA(Encoder *instance)
{
    unsigned long now = micros();
    if (now - instance->lastInterruptTimeA < 1000)
        return; // debounce ~1ms
    instance->lastInterruptTimeA = now;

    bool A = digitalRead(instance->pinA);
    bool B = digitalRead(instance->pinB);
    if (A == B)
        instance->encoderValue++;
    else
        instance->encoderValue--;
    instance->valueChanged = true;
}

void Encoder::begin()
{
    pinMode(pinA, INPUT_PULLUP);
    pinMode(pinB, INPUT_PULLUP);
    pinMode(pinButton, INPUT_PULLUP);
    attachInterruptArg(digitalPinToInterrupt(pinA), (void (*)(void *))handleInterruptA, this, CHANGE);
}

void Encoder::update()
{
    bool currentState = digitalRead(pinButton);
    unsigned long now = millis();
    if (currentState != lastButtonState)
    {
        lastButtonTime = now;
    }
    if ((now - lastButtonTime) > debounceDelayButton)
    {
        buttonPressed = (currentState == LOW);
    }
    lastButtonState = currentState;
}

bool Encoder::hasChanged()
{
    if (valueChanged)
    {
        valueChanged = false;
        return true;
    }
    return false;
}

int32_t Encoder::getValue()
{
    return encoderValue;
}

bool Encoder::isButtonPressed()
{
    return buttonPressed;
}

void Encoder::reset()
{
    encoderValue = 0;
    valueChanged = false;
}
