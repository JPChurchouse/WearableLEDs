#include "RotaryEncoder.hpp"

RotaryEncoder::RotaryEncoder(uint8_t pinA, uint8_t pinB, uint8_t pinBtn, int32_t startIndex)
    : _pinA(pinA), _pinB(pinB), _pinBtn(pinBtn),
      _movementFlag(false),
      _lrmem(3), _lrsum(0), _counter(startIndex),
      _lastButtonState(HIGH), _lastButtonMillis(0)
{
}

void RotaryEncoder::begin() {
    pinMode(_pinA, INPUT);
    pinMode(_pinB, INPUT);
    pinMode(_pinBtn, INPUT_PULLUP);

    // Attach ISRs: pass 'this' as arg so trampoline can call instance method
    attachInterruptArg(digitalPinToInterrupt(_pinA), isrHandlerA, this, CHANGE);
    attachInterruptArg(digitalPinToInterrupt(_pinB), isrHandlerB, this, CHANGE);
}

void IRAM_ATTR RotaryEncoder::isrHandlerA(void* arg) {
    reinterpret_cast<RotaryEncoder*>(arg)->handleInterrupt();
}

void IRAM_ATTR RotaryEncoder::isrHandlerB(void* arg) {
    reinterpret_cast<RotaryEncoder*>(arg)->handleInterrupt();
}

void IRAM_ATTR RotaryEncoder::handleInterrupt() {
    // Keep ISR tiny — just set the flag
    _movementFlag = true;
}

RotaryEncoder::State RotaryEncoder::read() {
    State s;
    s.changed = false;
    s.direction = 0;
    s.index = _counter;
    s.button = false;

    // --- Rotation handling ---
    if (_movementFlag) {
        // Clear the flag first (avoid missing a subsequent edge)
        _movementFlag = false;

        // Read encoder pins (not in ISR)
        int8_t l = digitalRead(_pinA) ? 1 : 0;
        int8_t r = digitalRead(_pinB) ? 1 : 0;

        // Shift previous two bits and add new A/B
        _lrmem = ((_lrmem & 0x03) << 2) + 2 * l + r;
        _lrsum += TRANS[_lrmem];

        // If not in detent (neutral) state, wait until it is
        if (_lrsum % 4 != 0) {
            // still moving; no final step yet
        } else {
            // in neutral state; check for full step counts
            if (_lrsum == 4) {
                _counter++;
                s.direction = 1;
                s.changed = true;
            } else if (_lrsum == -4) {
                _counter--;
                s.direction = -1;
                s.changed = true;
            }
            // reset accumulator
            _lrsum = 0;
        }
    }

    // --- Button handling with simple debounce ---
    bool rawBtn = digitalRead(_pinBtn); // HIGH when released (pullup)
    unsigned long now = millis();

    if (rawBtn != _lastButtonState) {
        // state changed — reset timer
        _lastButtonMillis = now;
        _lastButtonState = rawBtn;
    } else {
        // stable for debounce period?
        if ((now - _lastButtonMillis) >= _buttonDebounceMs) {
            bool pressed = (rawBtn == LOW); // active-low
            if (pressed != s.button) {
                // We report button state in s.button; set changed true if different
                s.button = pressed;
                s.changed = true;
            } else {
                s.button = pressed;
            }
        } else {
            // not settled yet — keep previous reported state (false)
            s.button = (rawBtn == LOW) && ((now - _lastButtonMillis) >= _buttonDebounceMs);
        }
    }

    // update index
    s.index = _counter;

    return s;
}
