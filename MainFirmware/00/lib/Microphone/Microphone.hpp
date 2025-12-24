#pragma once
#include <Arduino.h>
#include <driver/i2s.h>


class Microphone {
public:
Microphone(uint8_t sck, uint8_t ws, uint8_t sd);
void begin();
void enable(bool state);
bool beatDetected();


private:
bool enabled;
};