#include "Button.hpp"

Button::Button(uint8_t pin) : _pin(pin) {}

void Button::begin()
{
    pinMode(_pin, INPUT_PULLUP);

    attachInterruptArg(
        _pin,
        &Button::isrHandler,
        this,
        FALLING);
}

void IRAM_ATTR Button::isrHandler(void *arg)
{
    static_cast<Button *>(arg)->handleInterrupt();
}

void IRAM_ATTR Button::handleInterrupt()
{
    uint32_t now = millis();
    if (now - _lastInterruptTime >= _debounceMs)
    {
        _pressedFlag = true;
        _lastInterruptTime = now;
    }
}

bool Button::wasPressed()
{
    if (_pressedFlag)
    {
        _pressedFlag = false;
        return true;
    }
    return false;
}