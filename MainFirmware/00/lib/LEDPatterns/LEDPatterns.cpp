#include "LEDPatterns.hpp"


void solid(CRGB* leds, uint16_t count) {
fill_solid(leds, count, CRGB::Blue);
}


void rainbow(CRGB* leds, uint16_t count) {
static uint8_t hue;
fill_rainbow(leds, count, hue++);
}


void beatPulse(CRGB* leds, uint16_t count) {
fadeToBlackBy(leds, count, 20);
leds[random(count)] = CRGB::White;
}