#include <Arduino.h>
#include <LEDHandler.hpp>

template <int PIN>
LEDHandler<PIN>::LEDHandler(int ledCount)
    : count(ledCount)
{
  LEDs = new CRGB[count];
  FastLED.addLeds<WS2812B, PIN, GRB>(LEDs, count);
}

template <int PIN>
LEDHandler<PIN>::~LEDHandler()
{
  delete[] LEDs;
}

template <int PIN>
void LEDHandler<PIN>::update()
{
  fill_solid(LEDs, count, CRGB::Green);
  FastLED.show();
}
