
#pragma once

//
// PIN DEFINITIONS
//

// I2S Microphone
static const int MIC_PIN_SCK = 26;
static const int MIC_PIN_WS  = 27;
static const int MIC_PIN_SD  = 14;

// OLED Display (I2C)
static const int OLED_PIN_SDA = 21; // OLED Display SDA pin
static const int OLED_PIN_SCL = 22; // OLED Display SCL pin

// Rotary Encoder
static const int ENC_PIN_CLK = 35; // Encoder CLK pin (A)
static const int ENC_PIN_DT  = 33; // Encoder DT pin (B)
static const int ENC_PIN_SW  = 25; // Encoder SW pin

// LED Strips
static const int LED_PIN_A = 04; // LED strip ALPHA pin
static const int LED_PIN_B = 16; // LED strip BRAVO pin
static const int LED_PIN_C = 17; // LED strip CHARLIE pin
static const int LED_PIN_D = 05; // LED strip DELTA pin

//
// Other configuration values
//
static const int MIC_SAMPLE_RATE = 16000;
static const int MIC_SAMPLES = 1024;
