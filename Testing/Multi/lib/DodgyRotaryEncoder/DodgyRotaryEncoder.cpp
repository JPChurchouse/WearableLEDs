#include "DodgyRotaryEncoder.hpp"

static bool inWindow(uint32_t now, uint32_t period, uint32_t test)
{
    return now - test < period;
}

DodgyRotaryEncoder::DodgyRotaryEncoder(uint8_t pinA, uint8_t pinB, uint8_t pinBtn, int32_t startValue)
    : _pinA(pinA), _pinB(pinB), _pinBtn(pinBtn),
      _counter(startValue),
      _lastA(0), _lastB(0),
      _lastButtonState(HIGH), _lastButtonMillis(0)
{
}

void DodgyRotaryEncoder::begin()
{
    pinMode(_pinA, INPUT_PULLUP);
    pinMode(_pinB, INPUT_PULLUP);
    pinMode(_pinBtn, INPUT_PULLUP);

    attachInterruptArg(digitalPinToInterrupt(_pinA), isrA, this, FALLING);
    attachInterruptArg(digitalPinToInterrupt(_pinB), isrB, this, FALLING);
}

int32_t DodgyRotaryEncoder::getValue()
{
    return _counter;
}

bool DodgyRotaryEncoder::isButtonPressed()
{
    bool raw = digitalRead(_pinBtn) == LOW;
    unsigned long now = millis();

    if (raw != _lastButtonState)
    {
        _lastButtonMillis = now;
        _lastButtonState = raw;
    }
    else if (now - _lastButtonMillis >= _buttonDebounceMs)
    {
        return raw;
    }
    return _lastButtonState;
}

// ISR trampolines
void IRAM_ATTR DodgyRotaryEncoder::isrA(void *arg)
{
    reinterpret_cast<DodgyRotaryEncoder *>(arg)->handleA();
}

void IRAM_ATTR DodgyRotaryEncoder::isrB(void *arg)
{
    reinterpret_cast<DodgyRotaryEncoder *>(arg)->handleB();
}

// ISR implementations
void IRAM_ATTR DodgyRotaryEncoder::handleA()
{
    uint32_t now = micros();

    // Debounce
    if (inWindow(now, _debounce_us, _lastA))
        return;

    // Update timestamp
    _lastA = now;

    if (inWindow(now, _window_us, _lastB))
    {
        _counter--;
    }
}

void IRAM_ATTR DodgyRotaryEncoder::handleB()
{
    uint32_t now = micros();

    // Debounce
    if (inWindow(now, _debounce_us, _lastB))
        return;

    // Update timestamp
    _lastB = now;

    if (inWindow(now, _window_us, _lastA))
    {
        _counter++;
    }
}