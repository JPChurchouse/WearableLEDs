#include <Arduino.h>
#include "LEDHandler.hpp"

template <int PIN>
LEDHandler<PIN>::LEDHandler(int ledCount) : count(ledCount)
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
void LEDHandler<PIN>::Update()
{
  fill_solid(LEDs, count, CRGB::Green);
  FastLED.show();
  ulong mod = 765;
  ulong time = millis();
  for (int i = 0; i < count; i++)
  {
    ulong phase = time + i;
    uint8_t R = 0, G = 0, B = 0, Max = 255;
    //...generate a number between 0 and 767 based on time and LED index...
    unsigned p = ((20 * i) + phase) % (Max * 3); // (255 * 3) // This line covers all the settings
    if (p <= (Max)) // Red max
    {
      R = p;
      B = Max - R;
    }
    // > 255
    else if (p <= (Max * 2)) // Green max (255 * 2)
    {
      G = p - Max;
      R = Max - G;
    }
    // > 510 (s55 * 2)
    else if(p <= (Max * 3)) // Blue max (255 * 3)
    {
      B = p - (Max * 2);
      G = Max - B;
    }
    LEDs[i] = CRGB(R, G, B);
  }
}

template <int PIN>
CRGB &LEDHandler<PIN>::operator[](int index)
{
  return LEDs[index];
}