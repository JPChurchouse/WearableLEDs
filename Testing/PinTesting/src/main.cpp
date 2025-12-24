#include <Arduino.h>

#define A 32
#define B 33

void setup() {
    Serial.begin(115200);
    pinMode(A, INPUT_PULLUP);
pinMode(B, INPUT_PULLUP);

}

uint8_t last = 0;

void loop() {
    uint8_t val = (digitalRead(A) << 1) | digitalRead(B);
    if (val != last) {
        Serial.print("A=");
        Serial.print(val >> 1);
        Serial.print(" B=");
        Serial.println(val & 1);
        last = val;
    }
}
