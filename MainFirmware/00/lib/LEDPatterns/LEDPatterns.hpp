#pragma once
#include <FastLED.h>


typedef void (*PatternFn)(CRGB*, uint16_t);


void solid(CRGB* leds, uint16_t count);
void rainbow(CRGB* leds, uint16_t count);
void beatPulse(CRGB* leds, uint16_t count);