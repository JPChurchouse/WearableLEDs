#pragma once

#include <Arduino.h>
#include <FastLED.h>

class LEDStrip
{
private:
    CRGB *_leds = nullptr;
    uint8_t _count = 0, _index = 0;

public:
    LEDStrip(uint8_t count, uint8_t index) : _count(count), _index(index)
    {
        _leds = new CRGB[count];
    }

    ~LEDStrip()
    {
        delete[] _leds;
    }

    // Direct access to FastLED buffer
    CRGB *leds() { return _leds; }

    uint8_t count() const { return _count; }

    uint8_t index() const { return _index; }

    void clear() { fill_solid(_leds, _count, CRGB::Black); }
};