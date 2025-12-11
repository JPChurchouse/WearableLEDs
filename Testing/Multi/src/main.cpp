#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "driver/i2s.h"
#include "DodgyRotaryEncoder.hpp"
#include "Microphone.hpp"

#include "Config.hpp"

Adafruit_SSD1306 display(128, 64, &Wire, -1);

DodgyRotaryEncoder knob(ENC_PIN_CLK, ENC_PIN_DT, ENC_PIN_SW);
/*
Microphone mic(
   I2S_NUM_0,
   MIC_PIN_WS,
   MIC_PIN_SCK,
   MIC_PIN_SD,
   1024,
   16000);
*/

void setup()
{
  knob.begin();

  Wire.begin(OLED_PIN_SDA, OLED_PIN_SCL); // SDA, SCL
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  {
    for (;;)
      delay(10);
  }
  display.clearDisplay();
  display.display();

  // mic.begin();

  pinMode(LED_PIN_A, OUTPUT);
  digitalWrite(LED_PIN_A, LOW);
  pinMode(LED_PIN_B, OUTPUT);
  digitalWrite(LED_PIN_B, LOW);
  pinMode(LED_PIN_C, OUTPUT);
  digitalWrite(LED_PIN_C, LOW);
  pinMode(LED_PIN_D, OUTPUT);
  digitalWrite(LED_PIN_D, LOW);
}

long display_time = 0;

void loop()
{
  uint32_t now = millis();

  if (now - display_time > 10)
  {
    display_time = now;

    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(SSD1306_WHITE);

    display.setCursor(0, 0);
    display.print(knob.getValue());

    display.setCursor(64, 0);
    display.print(knob.isButtonPressed() ? "DOWN" : "UP");

    display.setTextSize(1);
    display.setCursor(0, 16);
    display.print("A: ");
    display.print(knob._lastA);

    display.setCursor(0, 24);
    display.print("B: ");
    display.print(knob._lastB);

    display.setCursor(0, 32);
    display.print("D: ");
    int64_t diff = (int64_t)knob._lastA - (int64_t)knob._lastB;
    display.print(diff);

    display.display();
  }
}