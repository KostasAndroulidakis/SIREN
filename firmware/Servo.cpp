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
// PWM signals. The pulseIn() function used by HC-SR04 also
// relies on precise timing.
//
// When both run simultaneously, the servo's PWM interrupts can
// cause jitter in pulseIn() measurements, leading to distance
// errors of several centimeters.
//
// By detaching the servo during ultrasonic measurement, we
// disable its PWM interrupts and get clean timing.

void ServoMotor::detach() {
    servo.detach();
}

void ServoMotor::attach() {
    servo.attach(SERVO_PIN);
}