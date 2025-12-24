#pragma once

#include <FastLED.h>
#include "LEDStrip.hpp"

enum LEDPatternTypes : uint8_t
{
  Off,
  Alpha,
  Bravo,
  Charlie
};

class LEDPatternHandler
{
private:
  uint8_t _count = 10;
  uint8_t _selected = 0, _executing = 0;
  uint32_t _timestamp = 0;

public:
LEDPatternHandler(){}
  uint8_t count() { return _count; }
  uint8_t selection() { return _selected; }
  uint8_t executing() { return _executing; }

  void setSelection(uint8_t value) { _selected = value % _count; }
  void confirmSelection() { _executing = _selected; }

  void run()
  {
  }
};
