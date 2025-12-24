// Ultrasonic.cpp

#include <Arduino.h>
#include "Ultrasonic.h"

void Ultrasonic::init() {
    pinMode(TRIG_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);
    digitalWrite(TRIG_PIN, LOW);
}

float Ultrasonic::getDistance(float soundSpeed) {
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);

    long duration = pulseIn(ECHO_PIN, HIGH);
    float distance = (duration * soundSpeed * 0.0001) / 2;

    if (distance < MIN_DISTANCE || distance > MAX_DISTANCE) {
        return -1;
    }
    return distance;
}