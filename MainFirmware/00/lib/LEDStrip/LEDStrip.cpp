#include "LEDStrip.hpp"


LEDStrip::LEDStrip(uint8_t pin) : _pin(pin), pattern(nullptr) {}


void LEDStrip::begin() {
FastLED.addLeds<WS2812B, _pin, GRB>(leds, NUM_LEDS);
}


void LEDStrip::setPattern(PatternFn fn) {
pattern = fn;
}


void LEDStrip::update() {
if (pattern) pattern(leds, NUM_LEDS);
FastLED.show();
}