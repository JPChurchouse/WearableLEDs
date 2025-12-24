#pragma once
#include <Arduino.h>


class Button {
public:
Button(uint8_t pin);
void begin();
bool wasPressed();


private:
static void IRAM_ATTR isr();
static volatile bool pressed;
uint8_t _pin;
};