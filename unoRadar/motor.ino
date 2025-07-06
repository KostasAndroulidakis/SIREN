#include <Servo.h>

Servo radarServo;
const int SERVO_PIN = 9;
const int MIN_ANGLE = 15;
const int MAX_ANGLE = 165;
const int SERVO_DELAY = 30;

void initMotor() {
  radarServo.attach(SERVO_PIN);
}

void moveServoToAngle(int angle) {
  radarServo.write(angle);
  delay(SERVO_DELAY);
}

int getMinAngle() {
  return MIN_ANGLE;
}

int getMaxAngle() {
  return MAX_ANGLE;
}