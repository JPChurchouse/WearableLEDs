#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "driver/i2s.h"
#include "DodgyRotaryEncoder.hpp"
#include "INMP441Audio.hpp" 
#include "Config.hpp"

Adafruit_SSD1306 display(128, 64, &Wire, -1);

DodgyRotaryEncoder knob(ENC_PIN_CLK, ENC_PIN_DT, ENC_PIN_SW);

INMP441Audio mic(    32,   25,   33,   1024,   16000); 

void setup()
{
  knob.begin();

  Wire.begin(OLED_PIN_SDA, OLED_PIN_SCL); // SDA, SCL
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  {
    while ( 1); 
  }
  display.clearDisplay();
  display.display();

  mic.begin();

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

    mic.update();

    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);

    display.setTextSize(2);
    display.setCursor(0, 0);
    display.print(knob.getValue());
    display.setCursor(64, 0);
    display.print(knob.isButtonPressed() ? "DOWN" : "UP");

    display.setTextSize(1);

    display.setCursor(0, 16);
    display.print("Bass: ");
    display.print(mic.getBass());

    display.setCursor(0, 24);
    display.print("Vol:  ");
    display.println(mic.getVolume());

    display.setCursor(0, 32);
    display.print("Freq: ");
    display.println(mic.getDominantFreq());  

    display.display();
  }
}