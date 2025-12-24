#include "Microphone.hpp"


Microphone::Microphone(uint8_t, uint8_t, uint8_t) : enabled(false) {}


void Microphone::begin() {
// Minimal I2S config placeholder – stable, not fancy
}


void Microphone::enable(bool state) {
enabled = state;
}


bool Microphone::beatDetected() {
if (!enabled) return false;
// Placeholder energy thresholding
return random(0, 20) == 0;
}