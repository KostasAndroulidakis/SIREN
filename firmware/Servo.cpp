// Servo.cpp
// SG90 servo wrapper with attach/detach for ultrasonic compatibility

#include <Arduino.h>
#include "Servo.h"

void ServoMotor::init() {
    servo.attach(SERVO_PIN);
    servo.write(90);    // Start at center position
    delay(60);          // Allow servo to reach position
    Serial.println(F("SG90 initialized"));
}

void ServoMotor::setAngle(int angle) {
    servo.write(angle);
}

// DETACH/ATTACH PATTERN:
// The Arduino Servo library uses Timer1 interrupts to generate
// PWM signals. Our ultrasonic measurement also uses Timer1 for
// Input Capture timing.
//
// When both run simultaneously, the servo's PWM configuration
// conflicts with our Input Capture setup, leading to distance
// errors of several centimeters.
//
// By detaching the servo during ultrasonic measurement, we
// free Timer1 for accurate pulse timing.

void ServoMotor::detach() {
    servo.detach();
}

void ServoMotor::attach() {
    servo.attach(SERVO_PIN);
}