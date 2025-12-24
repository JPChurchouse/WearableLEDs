#include <Arduino.h>
#include <U8g2lib.h>
#include <Wire.h>
#include "Encoder.hpp"
#include "Button.hpp"
#include "Microphone.hpp"
#include "LEDStrip.hpp"
#include "LEDPatterns.hpp"
#include "BLEManager.hpp"
#include "Config.hpp"

U8G2_SH1106_128X64_NONAME_F_HW_I2C display(U8G2_R0, U8X8_PIN_NONE);

Encoder encoder(ENC_A, ENC_B, ENC_SW);
Button bleButton(BTN_BLE);
Microphone mic(MIC_SCK, MIC_WS, MIC_SD);
BLEManager ble;

LEDStrip stripA((uint8_t)LED_A, NUM_LEDS);
LEDStrip stripB((uint8_t)LED_B, NUM_LEDS);
LEDStrip stripC((uint8_t)LED_C, NUM_LEDS);
LEDStrip stripD((uint8_t)LED_D, NUM_LEDS);

PatternFn patterns[] = {solid, rainbow, beatPulse};
int currentPattern = 0;

void setup()
{
    Wire.begin(DISP_SDA, DISP_SCL);
    display.begin();
    encoder.begin();
    bleButton.begin();
    mic.begin();
    ble.begin();

    stripA.begin();
    stripB.begin();
    stripC.begin();
    stripD.begin();
}

void loop()
{
    if (encoder.wasPressed())
    {
        currentPattern = (currentPattern + 1) % 3;
        stripA.setPattern(patterns[currentPattern]);
        stripB.setPattern(patterns[currentPattern]);
        stripC.setPattern(patterns[currentPattern]);
        stripD.setPattern(patterns[currentPattern]);
    }

    if (bleButton.wasPressed())
    {
        ble.enable(true);
    }

    stripA.update();
    stripB.update();
    stripC.update();
    stripD.update();

    delay(20);
}