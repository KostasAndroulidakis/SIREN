// Ultrasonic.cpp
// HC-SR04 driver with configurable speed of sound

#include <Arduino.h>
#include "Ultrasonic.h"

// TIMEOUT CALCULATION:
// Maximum distance is 400cm. At slowest reasonable speed of sound
// (~331 m/s at 0°C), round-trip time would be:
//   t = 2 × distance / speed = 2 × 4m / 331m/s = 0.024s = 24ms
// We use 30ms (30000μs) to add safety margin.
#define ECHO_TIMEOUT_US 30000

// UNIT CONVERSION:
// Speed of sound is in m/s, duration is in μs, we want cm.
//   distance(cm) = duration(μs) × speed(m/s) × (100cm/1m) × (1s/1000000μs)
//   distance(cm) = duration × speed × 0.0001
#define MICROSEC_TO_CM 0.0001

void Ultrasonic::init() {
    pinMode(TRIG_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);
    digitalWrite(TRIG_PIN, LOW);
    Serial.println("HC-SR04 initialized");
}

float Ultrasonic::getDistance(float soundSpeed) {
    // TRIGGER SEQUENCE (from datasheet):
    // 1. Ensure trigger is LOW
    // 2. Send HIGH pulse for at least 10μs
    // 3. Sensor will emit 8 pulses at 40kHz

    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);           // Ensure clean LOW state
    
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(12);          // >10μs required by datasheet
    digitalWrite(TRIG_PIN, LOW);

    // ECHO MEASUREMENT:
    // pulseIn() returns the duration (in μs) that ECHO pin stays HIGH.
    // This duration represents the round-trip time of the sound wave.
    // Returns 0 if no echo received within timeout.
    long duration = pulseIn(ECHO_PIN, HIGH, ECHO_TIMEOUT_US);
        
    if (duration == 0) {
        return -1;  // No echo - object out of range or no object
    }
    
    // DISTANCE FORMULA:
    //   distance = (duration × speed × unit_conversion) / 2
    //
    // Division by 2 because sound travels:
    //   sensor → object → sensor (round trip = 2× actual distance)
    float distance = (duration * soundSpeed * MICROSEC_TO_CM) / 2;

    // Validate against sensor's reliable range
    if (distance < MIN_DISTANCE || distance > MAX_DISTANCE) {
        return -1;
    }
    return distance;
}