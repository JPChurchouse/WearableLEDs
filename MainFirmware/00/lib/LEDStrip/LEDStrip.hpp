#pragma once

#include <FastLED.h>

using PatternFn = void (*)(CRGB* leds, uint16_t count);

class LEDStrip {
public:
    LEDStrip(uint8_t pin, uint16_t count);

    void begin();
    void setPattern(PatternFn fn);
    void update();

    CRGB* data() { return leds; }
    uint16_t size() const { return numLeds; }

private:
    uint8_t pin;
    uint16_t numLeds;
    CRGB* leds = nullptr;
    PatternFn pattern = nullptr;
};
