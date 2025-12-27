#include <Arduino.h>
#include <Wire.h>
#include <U8g2lib.h>
#include "driver/i2s.h"
#include "INMP441Audio.hpp" 

INMP441Audio mic(    32,   25,   33,   1024,   16000); 

// -------- OLED --------
U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

// -------- Encoder --------
#define ENC_A 39
#define ENC_B 36
#define ENC_SW 34
#define BTN_FUNC 35

volatile int encoderCount = 0;
volatile bool buttonPressed = false;
volatile bool func_pressed = false;
int lastEncoded = 0;

// -------- Encoder ISR --------
void IRAM_ATTR encoderISR()
{
  int MSB = digitalRead(ENC_A);
  int LSB = digitalRead(ENC_B);

  int encoded = (MSB << 1) | LSB;
  int sum = (lastEncoded << 2) | encoded;

  if (sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011)
    encoderCount++;
  if (sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000)
    encoderCount--;

  lastEncoded = encoded;
}

void IRAM_ATTR buttonISR()
{
  buttonPressed = true;
}

void IRAM_ATTR funcISR()
{
  func_pressed = !func_pressed;
}

void setup()
{
  Serial.begin(115200);
  Wire.begin(21, 22);

  u8g2.begin();

  pinMode(ENC_A, INPUT_PULLUP);
  pinMode(ENC_B, INPUT_PULLUP);
  pinMode(ENC_SW, INPUT_PULLUP);
  pinMode(BTN_FUNC, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(ENC_A), encoderISR, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ENC_B), encoderISR, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ENC_SW), buttonISR, FALLING);
  attachInterrupt(digitalPinToInterrupt(BTN_FUNC), funcISR, FALLING);
  
  mic.begin();
}

static const u8g2_cb_t *rotations[] = {
    U8G2_R0,
    U8G2_R1,
    U8G2_R2,
    U8G2_R3};

void loop()
{
    mic.update();

  static uint8_t rotation = 0;

  if (buttonPressed)
  {
    rotation = (rotation + 1) % 4;
    u8g2.setDisplayRotation(rotations[rotation]);
    buttonPressed = false;
  }

  int xPos = constrain(encoderCount, 0, 127);

  u8g2.clearBuffer();

  // ---------- Text + Fonts ----------
  u8g2.setFont(u8g2_font_5x8_tf);
  //u8g2.drawStr(0, 8, "U8g2 feature demo");

  //u8g2.setFont(u8g2_font_6x12_tf);
  //u8g2.drawStr(0, 22, "Medium text");

  //u8g2.setFont(u8g2_font_ncenB14_tr);
  //u8g2.drawStr(0, 40, "BIG");

    u8g2.setCursor(0, 16);
    u8g2.print("Bass: ");
    u8g2.print(mic.getBass());

    u8g2.setCursor(0, 24);
    u8g2.print("Vol:  ");
    u8g2.println(mic.getVolume());

    u8g2.setCursor(0, 32);
    u8g2.print("Freq: ");
    u8g2.println(mic.getDominantFreq());  

  // ---------- Inverted text ----------
  u8g2.setDrawColor(1);
  u8g2.drawBox(80, 26, 44, 16);
  u8g2.setDrawColor(0);
  u8g2.setFont(u8g2_font_6x12_tf);
  u8g2.drawStr(84, 38, "INV");
  u8g2.setDrawColor(1);

  // ---------- Graphics ----------
  u8g2.drawLine(0, 44, 127, 44);
  if(func_pressed)
  {
    u8g2.drawFilledEllipse(110, 10, 6, U8G2_DRAW_ALL);
  }
  else
  {
  u8g2.drawCircle(110, 10, 6, U8G2_DRAW_ALL);
  }

  // ---------- Animated element ----------
  u8g2.drawFrame(xPos, 50, 6, 10);

  u8g2.sendBuffer();

  delay(30);
}
