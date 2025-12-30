#pragma once

#include "LEDStrip.hpp"

void runOff(LEDStrip *s)
{
    s->clear();
}

void runRainbowCycle(LEDStrip *s)
{
    static uint8_t hue = 0;
    for (int i = 0; i < s->count(); i++)
    {
        s->leds()[i] = CHSV(hue + (i * 255 / s->count()), 255, 255);
    }
    hue++;
}

void runColorWipe(LEDStrip *s)
{
    static int pos = 0;
    static CRGB colour = CRGB::Red;

    s->leds()[pos] = colour;
    pos++;

    if (pos >= s->count())
    {
        pos = 0;
        colour = CHSV(random8(), 255, 255);
        fill_solid(s->leds(), s->count(), CRGB::Black);
    }
}

void runLarsonScanner(LEDStrip *s)
{
    static int pos = 0;
    static int dir = 1;

    fadeToBlackBy(s->leds(), s->count(), 40);
    s->leds()[pos] = CRGB::Red;

    pos += dir;
    if (pos <= 0 || pos >= s->count() - 1)
        dir = -dir;
}

void runConfetti(LEDStrip *s)
{
    fadeToBlackBy(s->leds(), s->count(), 20);
    int pos = random16(s->count());
    s->leds()[pos] += CHSV(random8(), 200, 255);
}

void runBpmPulse(LEDStrip *s)
{
    uint8_t bpm = 62;
    uint8_t beat = beatsin8(bpm, 64, 255);

    for (int i = 0; i < s->count(); i++)
    {
        s->leds()[i] = CHSV(0, 255, beat);
    }
}

void runFireEffect(LEDStrip *s)
{
    static uint8_t heat[300]; // adjust if strip >300 LEDs

    for (int i = 0; i < s->count(); i++)
    {
        heat[i] = qsub8(heat[i], random8(0, 50));
    }

    for (int k = s->count() - 1; k >= 2; k--)
    {
        heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2]) / 3;
    }

    if (random8() < 120)
    {
        heat[random8(3)] = qadd8(heat[random8(3)], random8(160, 255));
    }

    for (int j = 0; j < s->count(); j++)
    {
        s->leds()[j] = HeatColor(heat[j]);
    }
}

void runTheatreChase(LEDStrip *s)
{
    static int offset = 0;
    fill_solid(s->leds(), s->count(), CRGB::Black);

    for (int i = offset; i < s->count(); i += 3)
    {
        s->leds()[i] = CRGB::White;
    }

    offset = (offset + 1) % 3;
}

void runTwinkleFade(LEDStrip *s)
{
    fadeToBlackBy(s->leds(), s->count(), 30);
    if (random8() < 80)
    {
        s->leds()[random16(s->count())] = CHSV(random8(), 180, 255);
    }
}

void runCometTrail(LEDStrip *s)
{
    static int pos = 0;
    static int dir = 1;

    fadeToBlackBy(s->leds(), s->count(), 20);
    s->leds()[pos] = CRGB::Blue;

    pos += dir;
    if (pos == 0 || pos == s->count() - 1)
        dir = -dir;
}

void runMoodGlow(LEDStrip *s)
{
    static uint8_t hue = 0;
    static uint8_t brightness = 0;
    static int dir = 1;

    brightness += dir * 5;
    if (brightness <= 20 || brightness >= 250)
        dir = -dir;

    fill_solid(s->leds(), s->count(), CHSV(hue, 180, brightness));
    hue++;
}

void runAmbo(LEDStrip *s)
{
    const uint16_t phase_time = 4000;
const uint8_t  phase_count = 3;
  uint8_t phase=0 ;

auto now = millis();

phase = (now% (phase_count*phase_time))/4000;


}

using PatternFunction = void (*)(LEDStrip *s);

struct PatternDescriptor
{
    const char *name;    // Display name
    PatternFunction run; // Function pointer
};

const PatternDescriptor patterns[] = {
    {"Off", runOff},
    {"Rainbow Cycle", runRainbowCycle},
    {"Colour Wipe", runColorWipe},
    {"Larson Scanner", runLarsonScanner},
    {"Confetti", runConfetti},
    {"BPM Pulse", runBpmPulse},
    {"Fire", runFireEffect},
    {"Theatre Chase", runTheatreChase},
    {"Twinkle Fade", runTwinkleFade},
    {"Comet Trail", runCometTrail},
    {"Mood Glow", runMoodGlow},
    {"Ambulance", runAmbo}};

constexpr uint8_t PATTERN_COUNT = sizeof(patterns) / sizeof(patterns[0]);