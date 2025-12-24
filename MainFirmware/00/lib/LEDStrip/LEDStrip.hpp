#pragma once
#include <FastLED.h>
#include "../LEDPatterns/LEDPatterns.hpp"


class LEDStrip {
public:
LEDStrip(uint8_t pin);
void begin();
void setPattern(PatternFn fn);
void update();


private:
CRGB leds[NUM_LEDS];
uint8_t _pin;
PatternFn pattern;
};