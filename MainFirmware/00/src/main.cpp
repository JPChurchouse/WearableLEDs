#include <Arduino.h>
#include <U8g2lib.h>
#include "Config.hpp"
#include "Encoder.hpp"
#include "Button.hpp"
#include "Microphone.hpp"
#include "LEDStrip.hpp"
#include "LEDPatterns.hpp"
#include "BLEManager.hpp"


U8G2_SSD1306_128X64_NONAME_F_HW_I2C display(U8G2_R0, U8X8_PIN_NONE);


Encoder encoder(ENC_A, ENC_B, ENC_SW);
Button bleButton(BTN_BLE);
Microphone mic(MIC_SCK, MIC_WS, MIC_SD);
BLEManager ble;


LEDStrip stripA(LED_A);
LEDStrip stripB(LED_B);
LEDStrip stripC(LED_C);
LEDStrip stripD(LED_D);


PatternFn patterns[] = { solid, rainbow, beatPulse };
int currentPattern = 0;


void setup() {
Serial.begin(115200);


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


void loop() {
if (encoder.wasPressed()) {
currentPattern = (currentPattern + 1) % 3;
stripA.setPattern(patterns[currentPattern]);
stripB.setPattern(patterns[currentPattern]);
stripC.setPattern(patterns[currentPattern]);
stripD.setPattern(patterns[currentPattern]);
}


if (bleButton.wasPressed()) {
ble.enable(true);
}


stripA.update();
stripB.update();
stripC.update();
stripD.update();


delay(20);
}