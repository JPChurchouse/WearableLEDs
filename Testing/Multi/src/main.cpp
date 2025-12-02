#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "driver/i2s.h"
#include "Encoder.h"
#include "Microphone.hpp"

#define OLED_WIDTH 128
#define OLED_HEIGHT 64
#define OLED_PIN_SDA 21
#define OLED_PIN_SCL 22
Adafruit_SSD1306 display(OLED_WIDTH, OLED_HEIGHT, &Wire, -1);

// Encoder pins
#define ENC_PIN_CLK 32
#define ENC_PIN_DT 33
#define ENC_PIN_SW 25
Encoder knob(ENC_PIN_DT, ENC_PIN_CLK, ENC_PIN_SW);

// I2S pins (these matched the diagnostic that produced valid samples)
#define MIC_PIN_SCK 26
#define MIC_PIN_WS 27
#define MIC_PIN_SD 14
Microphone mic(
    I2S_NUM_0,
    MIC_PIN_WS,
    MIC_PIN_SCK,
    MIC_PIN_SD,
    1024,
    16000);

// LED pins (reserved, not driven)
#define LEDS_PIN_A 4
#define LEDS_PIN_B 16
#define LEDS_PIN_C 17
#define LEDS_PIN_D 5

void setup()
{
  // Encoder
  knob.begin();

  // OLED
  Wire.begin(OLED_PIN_SDA, OLED_PIN_SCL); // SDA, SCL
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  {
    for (;;)
      delay(10);
  }
  display.clearDisplay();
  display.display();

  // I2S mic
  mic.begin();

  // Reserve LED pins so nothing else grabs them (not used in test)
  pinMode(LEDS_PIN_A, OUTPUT);
  digitalWrite(LEDS_PIN_A, LOW);
  pinMode(LEDS_PIN_B, OUTPUT);
  digitalWrite(LEDS_PIN_B, LOW);
  pinMode(LEDS_PIN_C, OUTPUT);
  digitalWrite(LEDS_PIN_C, LOW);
  pinMode(LEDS_PIN_D, OUTPUT);
  digitalWrite(LEDS_PIN_D, LOW);
}

void loop()
{
  knob.update();
  int encoderValue = knob.getValue();
  bool buttonPressed = knob.isButtonPressed();

  mic.sample();
  mic.computeFFT();

  double freq = mic.getDominantFrequency();

  double bass = 0;
  for (int i = 2; i < 20; i++)
    bass += mic.getBinMagnitude(i);

  // -- OLED update
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  display.setCursor(0, 0);
  display.print("ENC: ");
  display.print(encoderValue);

  display.setCursor(0, 12);
  display.print("BTN: ");
  display.print(buttonPressed ? "DOWN" : "UP");

  display.setCursor(0, 24);
  display.print("Freq: ");
  display.println(freq);

  // Show peek sample in hex and decimal
  display.setCursor(0, 36);
  display.print("Bass: ");
  display.println(bass);

  // Draw bar graph scaled from 0..max (tweak maxVal if necessary)
  const int maxVal = 15000; // adjust if your readings are higher/lower
  int bar = map(constrain(bass, 0, maxVal), 0, maxVal, 0, 120);
  display.fillRect(0, 50, bar, 10, SSD1306_WHITE);

  display.display();

  delay(10);
}
