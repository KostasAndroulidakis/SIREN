// Servo.h
// SG90 Micro Servo Motor Driver
//
// PURPOSE:
// Rotates the ultrasonic sensor to scan 180 degrees,
// creating a radar-like sweep pattern.
//
// PWM INTERFERENCE ISSUE:
// Both the Servo library and pulseIn() use timer interrupts.
// When the servo is attached, its PWM signal can interfere
// with precise timing measurements from the ultrasonic sensor.
//
// SOLUTION:
// We detach() the servo before taking ultrasonic readings,
// then attach() it again afterward. This ensures clean
// pulse timing for distance measurement.
//
// SG90 SPECS:
//   - Operating voltage: 4.8-6V
//   - Rotation range: 0-180 degrees
//   - Speed: ~0.1s per 60 degrees (at 4.8V)
//   - PWM frequency: 50Hz (20ms period)
//   - Pulse width: 500μs (0°) to 2400μs (180°)

#ifndef SERVO_H
#define SERVO_H

#include <Servo.h>
#include "config.h"

// Servo movement limits
#define SERVO_MIN_ANGLE 10
#define SERVO_MAX_ANGLE 170

// Sweep configuration
// Note: HC-SR04 has 15° beam width, so 1° step creates overlap
// This is intentional for smooth radar display visualization
#define SERVO_STEP 1

// Delay between steps (ms)
// Must allow servo to reach position before measurement
// SG90 needs ~1.7ms per degree, but we use 60ms for stability
// and to meet HC-SR04's recommended 60ms measurement cycle
#define SERVO_DELAY 60

class ServoMotor {
public:
    void init();
    void setAngle(int angle);
    void detach();      // Detach before ultrasonic reading
    void attach();      // Re-attach after reading
private:
    Servo servo;
};

#endif