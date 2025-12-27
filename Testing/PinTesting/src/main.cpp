#include <Arduino.h>

#define A 13

void setup() { 
    pinMode(A,OUTPUT);
}
 

void loop() {
    digitalWrite(A,1);
    delay(30);
    digitalWrite(A,0);
    delay(30);
    digitalWrite(A,1);
    delay(30);
    digitalWrite(A,0);
    delay(3910);
}
