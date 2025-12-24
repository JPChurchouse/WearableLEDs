#pragma once

// OLED
const uint8_t DISP_SDA = 21;
const uint8_t DISP_SCL = 22;

// Encoder
const uint8_t ENC_A = 39;
const uint8_t ENC_B = 36;
const uint8_t ENC_SW = 34;

// Buttons
const uint8_t BTN_BLE = 35;
// BTN_RESET is ESP32 EN pin (handled in hardware)

// Microphone (INMP441)
const uint8_t MIC_SCK = 32;
const uint8_t MIC_SD = 33;
const uint8_t MIC_WS = 25;

// LED Strips
const uint8_t LED_A = 16;
const uint8_t LED_B = 17;
const uint8_t LED_C = 18;
const uint8_t LED_D = 19;

const uint8_t NUM_LEDS = 144;