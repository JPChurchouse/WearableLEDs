#ifndef MAIN_HPP
#define MAIN_HPP

#include <Arduino.h>
#include <ArduinoBLE.h>
#include "LEDHandler.hpp"

bool Running = false;

LEDHandler<12> LEDs_A(144);
LEDHandler<14> LEDs_B(144);

BLEService Service("19B10000-E8F2-537E-4F6C-D104768A1214"); // Bluetooth® Low Energy LED Service

// Bluetooth® Low Energy LED Switch Characteristic - custom 128-bit UUID, read and writable by central
BLEByteCharacteristic ToggleChara("19B10001-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite | BLENotify | BLEIndicate);
BLEByteCharacteristic ModeChara("d02a2828-2b77-4685-89f0-2b9f0ba38da5", BLERead | BLEWrite);



#endif