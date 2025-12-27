#include <Arduino.h>
#include <INMP441.hpp>

#define MIC_SCK 32
#define MIC_SD  33
#define MIC_WS  25

INMP441 mic;

void setup() {
    Serial.begin(115200);
    delay(1000);

    if (!mic.begin(MIC_SCK, MIC_WS, MIC_SD)) {
        Serial.println("INMP441 init failed");
        while (true);
    }

    Serial.println("INMP441 ready");
}

void loop() {
    if (mic.readSamples(512)) {
        Serial.print("RMS: ");
        Serial.print(mic.rms(), 1);
        Serial.print(" | Peak: ");
        Serial.println(mic.peak());
    }

    delay(200);
}
