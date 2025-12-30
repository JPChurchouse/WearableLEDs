#include <Arduino.h>
#include <U8g2lib.h>
#include <Wire.h>
#include "Encoder.hpp"
#include "Button.hpp"
#include "LEDStrip.hpp"
#include "LEDStripManager.hpp"
#include "LEDPatternHandler.hpp"
#include "Config.hpp"

U8G2_SH1106_128X64_NONAME_F_HW_I2C display(U8G2_R0, U8X8_PIN_NONE);

Encoder encoder(PIN_ENC_A, PIN_ENC_B);
Button btnSelect = Button(PIN_ENC_SW);

LEDStripManager<PIN_LED_A> stripA(NUM_LEDS, 0);
LEDStripManager<PIN_LED_B> stripB(NUM_LEDS, 1);
LEDStripManager<PIN_LED_C> stripC(NUM_LEDS, 2);
LEDStripManager<PIN_LED_D> stripD(NUM_LEDS, 3);
LEDPatternHandler pattern;

void UpdateDisplay();

void setup()
{
    Wire.begin(PIN_DISP_SDA, PIN_DISP_SCL);
    display.begin();
    encoder.begin();
    btnSelect.begin();

    stripA.begin();
    stripB.begin();
    stripC.begin();
    stripD.begin();
    FastLED.setBrightness(100);
}

void loop()
{
    {
        auto delta = encoder.getDelta();
        if (delta > 0)
            pattern.nextPattern();
        else if (delta < 0)
            pattern.previousPattern();

        if (btnSelect.wasPressed())
            pattern.confirmSelection();
    }

    {
        pattern.run(stripA.strip());
        pattern.run(stripB.strip());
        pattern.run(stripC.strip());
        pattern.run(stripD.strip());
        FastLED.show();
    }

    UpdateDisplay();
}

uint64_t _disp_timestamp = 0;
void UpdateDisplay()
{
    auto now = millis();
    if (now - _disp_timestamp < 25)
        return;
    _disp_timestamp = now;

    uint8_t selectedIndex = pattern.getSelectedIndex();
    uint8_t activeIndex = pattern.getExecutingIndex();

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
        display.print(patterns[patternIndex].name);

        // Restore draw colour
        display.setDrawColor(1);
    }

    display.sendBuffer();
}