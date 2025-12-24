#pragma once
#include <Arduino.h>


class BLEManager {
public:
void begin();
void enable(bool state);
int getRequestedPattern();
};