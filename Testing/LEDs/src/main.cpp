#include <FastLED.h>

// ===== CONFIG =====
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB
#define NUM_LEDS    10          // change to match your strip length
#define BRIGHTNESS  64

// Pins
#define PIN_1 19
#define PIN_2 18
#define PIN_3 17
#define PIN_4 16

// LED arrays
CRGB leds1[NUM_LEDS];
CRGB leds2[NUM_LEDS];
CRGB leds3[NUM_LEDS];
CRGB leds4[NUM_LEDS];

void setup() {
  delay(1000); // power-up safety delay

  FastLED.addLeds<LED_TYPE, PIN_1, COLOR_ORDER>(leds1, NUM_LEDS);
  FastLED.addLeds<LED_TYPE, PIN_2, COLOR_ORDER>(leds2, NUM_LEDS);
  FastLED.addLeds<LED_TYPE, PIN_3, COLOR_ORDER>(leds3, NUM_LEDS);
  FastLED.addLeds<LED_TYPE, PIN_4, COLOR_ORDER>(leds4, NUM_LEDS);

  FastLED.setBrightness(BRIGHTNESS);
}

void loop() {
  fill_solid(leds1, NUM_LEDS, CRGB::Red);
  fill_solid(leds2, NUM_LEDS, CRGB::Green);
  fill_solid(leds3, NUM_LEDS, CRGB::Blue);
  fill_solid(leds4, NUM_LEDS, CRGB::White);
  FastLED.show();
  delay(1000);

  fill_solid(leds1, NUM_LEDS, CRGB::Green);
  fill_solid(leds2, NUM_LEDS, CRGB::Blue);
  fill_solid(leds3, NUM_LEDS, CRGB::Red);
  fill_solid(leds4, NUM_LEDS, CRGB::Yellow);
  FastLED.show();
  delay(1000);

  fill_solid(leds1, NUM_LEDS, CRGB::Black);
  fill_solid(leds2, NUM_LEDS, CRGB::Black);
  fill_solid(leds3, NUM_LEDS, CRGB::Black);
  fill_solid(leds4, NUM_LEDS, CRGB::Black);
  FastLED.show();
  delay(500);
}
