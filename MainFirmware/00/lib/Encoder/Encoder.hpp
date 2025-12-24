#pragma once
#include <Arduino.h>


class Encoder {
public:
Encoder(uint8_t pinA, uint8_t pinB, uint8_t pinSW);
void begin();
int32_t getPosition();
bool wasPressed();


private:
static void IRAM_ATTR isrA();
static void IRAM_ATTR isrB();
static void IRAM_ATTR isrSW();


static volatile int32_t position;
static volatile bool pressed;


uint8_t _pinA, _pinB, _pinSW;
};