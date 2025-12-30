#pragma once

#include <Arduino.h>
#include "LEDStrip.hpp"

template <uint8_t PIN>
class LEDStripManager
{
private:
    LEDStrip *_strip;

public:
    LEDStripManager(uint8_t count, uint8_t index)
    {
        _strip = new LEDStrip(count, index);
    }

    ~LEDStripManager()
    {
        delete _strip;
    }

    void begin()
    {
        FastLED.addLeds<WS2812B, PIN, GRB>(_strip->leds(), _strip->count());
        //FastLED.setCorrection(TypicalLEDStrip);
        FastLED.show();
    }

    LEDStrip *strip() { return _strip; }
};