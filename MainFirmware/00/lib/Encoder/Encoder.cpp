#include "Encoder.hpp"


volatile int32_t Encoder::position = 0;
volatile bool Encoder::pressed = false;


static Encoder* instance;


Encoder::Encoder(uint8_t pinA, uint8_t pinB, uint8_t pinSW)
: _pinA(pinA), _pinB(pinB), _pinSW(pinSW) {
instance = this;
}


void Encoder::begin() {
pinMode(_pinA, INPUT);
pinMode(_pinB, INPUT);
pinMode(_pinSW, INPUT_PULLUP);


attachInterrupt(digitalPinToInterrupt(_pinA), isrA, CHANGE);
attachInterrupt(digitalPinToInterrupt(_pinB), isrB, CHANGE);
attachInterrupt(digitalPinToInterrupt(_pinSW), isrSW, FALLING);
}


void IRAM_ATTR Encoder::isrA() {
if (digitalRead(instance->_pinA) == digitalRead(instance->_pinB)) position++;
else position--;
}


void IRAM_ATTR Encoder::isrB() {
if (digitalRead(instance->_pinA) != digitalRead(instance->_pinB)) position++;
else position--;
}


void IRAM_ATTR Encoder::isrSW() {
pressed = true;
}


int32_t Encoder::getPosition() {
return position;
}


bool Encoder::wasPressed() {
if (pressed) {
pressed = false;
return true;
}
return false;
}