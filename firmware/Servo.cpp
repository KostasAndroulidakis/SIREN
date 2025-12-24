// Servo.cpp

#include <Arduino.h>
#include "Servo.h"

void ServoMotor::init() {
    servo.attach(SERVO_PIN);
    servo.write(90);
    delay(60);
}

void ServoMotor::setAngle(int angle) {
    servo.write(angle);
}