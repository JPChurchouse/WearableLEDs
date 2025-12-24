#include "Button.hpp"


volatile bool Button::pressed = false;
static Button* btnInstance;


Button::Button(uint8_t pin) : _pin(pin) {
btnInstance = this;
}


void Button::begin() {
pinMode(_pin, INPUT_PULLUP);
attachInterrupt(digitalPinToInterrupt(_pin), isr, FALLING);
}


void IRAM_ATTR Button::isr() {
pressed = true;
}


bool Button::wasPressed() {
if (pressed) {
pressed = false;
return true;
}
return false;
}