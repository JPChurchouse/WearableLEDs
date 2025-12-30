#pragma once

#include <FastLED.h>
#include "LEDStrip.hpp"
#include "LEDPatterns.hpp"

class LEDPatternHandler
{
private:
  uint8_t _selected = 1, _executing = 1;
  uint32_t _timestamp = 0;
  uint8_t parseIndex(int i)
  {
    int result = i % PATTERN_COUNT;
    if (result < 0)
      result += PATTERN_COUNT;
    return result;
  }

public:
  LEDPatternHandler() {}
  const char *getSelectedName() { return patterns[_selected].name; }
  const char *getExecutingName() { return patterns[_executing].name; }
  uint8_t getSelectedIndex() { return _selected; }
  uint8_t getExecutingIndex() { return _executing; }

  void setSelection(uint8_t value) { _selected = parseIndex(value); }
  void nextPattern() { _selected = (_selected + 1) % PATTERN_COUNT; }
  void previousPattern() { _selected = (_selected - 1 + PATTERN_COUNT) % PATTERN_COUNT; }

  void confirmSelection() { _executing = _selected; }
  void run(LEDStrip *s)
  {
      patterns[_executing].run(s);return;
    const uint16_t del = 40; // 25 Hz
    if ((millis() - _timestamp) >= del)
    {
      _timestamp += del;
      patterns[_executing].run(s);
    }
  }
};
