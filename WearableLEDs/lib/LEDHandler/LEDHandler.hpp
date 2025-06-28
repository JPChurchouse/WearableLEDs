#ifndef LEDHandler_hpp
#define LEDHandler_hpp

#include <Arduino.h>
#include <FastLED.h>

template <int PIN>
class LEDHandler
{
private:
  int count;
  CRGB *LEDs;

public:
  LEDHandler(int ledCount);
  ~LEDHandler();

  void Update();

  CRGB &operator[](int index);
};

#include "LEDHandler.tpp"

#endif