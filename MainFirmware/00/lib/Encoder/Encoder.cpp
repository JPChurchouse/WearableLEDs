#include "Encoder.hpp"

// State transition table
// Index: (oldState << 2) | newState
// Values: -1 = CCW, +1 = CW, 0 = invalid/no movement
static const int8_t transitionTable[16] = {
    0, -1, 1, 0,
    1, 0, 0, -1,
    -1, 0, 0, 1,
    0, 1, -1, 0};

Encoder::Encoder(uint8_t pinA, uint8_t pinB)
    : _pinA(pinA),
      _pinB(pinB)
{
}

void Encoder::begin()
{
    pinMode(_pinA, INPUT_PULLUP);
    pinMode(_pinB, INPUT_PULLUP);

    uint8_t a = digitalRead(_pinA);
    uint8_t b = digitalRead(_pinB);
    _lastState = (a << 1) | b;

    attachInterruptArg(_pinA, &Encoder::isrHandler, this, CHANGE);
    attachInterruptArg(_pinB, &Encoder::isrHandler, this, CHANGE);
}

void IRAM_ATTR Encoder::isrHandler(void *arg)
{
    static_cast<Encoder *>(arg)->handleInterrupt();
}

void IRAM_ATTR Encoder::handleInterrupt()
{
    uint32_t now = micros();
    if (now - _lastInterruptTime < _debounceUs)
    {
        return;
    }

    uint8_t a = digitalRead(_pinA);
    uint8_t b = digitalRead(_pinB);
    uint8_t newState = (a << 1) | b;

    uint8_t index = (_lastState << 2) | newState;
    int8_t movement = transitionTable[index];

    if (movement != 0)
    {
        _delta += movement;
    }

    _lastState = newState;
    _lastInterruptTime = now;
}

int32_t Encoder::getDelta()
{
    int32_t value = _delta;
    _delta = 0;
    return value;
}
