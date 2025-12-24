#include "LEDStrip.hpp"

LEDStrip::LEDStrip(uint8_t pin_, uint16_t count)
    : pin(pin_), numLeds(count) {}

void LEDStrip::begin() {
    leds = new CRGB[numLeds];

    // FastLED requires compile-time pins → use switch
    switch (pin) {
        case 4:
            FastLED.addLeds<WS2812B, 4, GRB>(leds, numLeds);
            break;
        case 5:
            FastLED.addLeds<WS2812B, 5, GRB>(leds, numLeds);
            break;
        case 16:
            FastLED.addLeds<WS2812B, 16, GRB>(leds, numLeds);
            break;
        default:
            // Unsupported pin → silent failure is bad, but crashing is worse
            break;
    }
}

void LEDStrip::setPattern(PatternFn fn) {
    pattern = fn;
}

void LEDStrip::update() {
    if (pattern) {
        pattern(leds, numLeds);
    }
}
