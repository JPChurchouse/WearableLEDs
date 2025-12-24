#include <Arduino.h>
#include <U8g2lib.h>
#include <Wire.h>
#include "Encoder.hpp"
#include "Button.hpp"
#include "Microphone.hpp"
#include "LEDStrip.hpp"
#include "LEDStripManager.hpp"
#include "LEDPatternHandler.hpp"
#include "BLEManager.hpp"
#include "Config.hpp"

U8G2_SH1106_128X64_NONAME_F_HW_I2C display(U8G2_R0, U8X8_PIN_NONE);

Encoder encoder(ENC_A, ENC_B, ENC_SW);
Button bleButton(BTN_BLE);
Microphone mic(MIC_SCK, MIC_WS, MIC_SD);
BLEManager ble;

LEDStripManager<LED_A> stripA(NUM_LEDS, 0);
LEDStripManager<LED_B> stripB(NUM_LEDS, 1);
LEDStripManager<LED_C> stripC(NUM_LEDS, 2);
LEDStripManager<LED_D> stripD(NUM_LEDS, 3);
LEDPatternHandler pattern;

void UpdateDisplay();

void setup()
{
    Wire.begin(DISP_SDA, DISP_SCL);
    display.begin();
    encoder.begin();
    bleButton.begin();
    mic.begin();
    // ble.begin();

    stripA.begin();
    stripB.begin();
    stripC.begin();
    stripD.begin();
}

void loop()
{
    if (encoder.wasPressed())
    {
    }

    if (bleButton.wasPressed())
    {
        // ble.enable(true);
    }

    pattern.run();

    UpdateDisplay();

    delay(5);
}

const char *PATTERN_NAMES[] = {
    "Off",
    "Odd / Even Swap",
    "Beat Pulse",
    "Rainbow March",
    "Strobe",
    "VU Meter",
    "Scanner",
    "Chaos"};

void UpdateDisplay()
{
    uint8_t selectedIndex = pattern.selection();
    uint8_t activeIndex = pattern.executing();
    uint8_t PATTERN_COUNT = pattern.count();

    display.clearBuffer();

    // ---------- Layout ----------
    display.setFont(u8g2_font_6x10_tr);
    const uint8_t lineHeight = display.getMaxCharHeight();
    const uint8_t titleHeight = lineHeight + 2;

    const uint8_t screenHeight = 64;
    const uint8_t screenWidth = 128;

    const uint8_t visibleLines =
        (screenHeight - titleHeight) / lineHeight;

    // ---------- Title ----------
    display.setCursor(0, lineHeight);
    display.print("LED Patterns");

    // ---------- Scrolling logic ----------
    int scrollOffset = 0;

    if (selectedIndex >= visibleLines)
    {
        scrollOffset = selectedIndex - visibleLines + 1;
    }

    if (scrollOffset < 0)
        scrollOffset = 0;
    if (scrollOffset > PATTERN_COUNT - visibleLines)
        scrollOffset = PATTERN_COUNT - visibleLines;

    // ---------- Pattern list ----------
    for (int i = 0; i < visibleLines; i++)
    {
        int patternIndex = i + scrollOffset;
        if (patternIndex >= PATTERN_COUNT)
            break;

        int y = titleHeight + (i + 1) * lineHeight;

        bool isSelected = (patternIndex == selectedIndex);
        bool isActive = (patternIndex == activeIndex);

        // Active pattern: inverted box
        if (isActive)
        {
            display.setDrawColor(1);
            display.drawBox(0, y - lineHeight + 1, screenWidth, lineHeight);
            display.setDrawColor(0);
        }

        // Selection box (outline)
        if (isSelected)
        {
            display.setDrawColor(1);
            display.drawFrame(0, y - lineHeight + 1, screenWidth, lineHeight);
        }

        // Text
        display.setCursor(2, y);
        display.print(PATTERN_NAMES[patternIndex]);

        // Restore draw colour
        display.setDrawColor(1);
    }

    display.sendBuffer();
}