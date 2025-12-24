// Servo.h

#ifndef SERVO_H
#define SERVO_H

#include <Servo.h>
#include "config.h"

class ServoMotor {
public:
    void init();
    void setAngle(int angle);
private:
    Servo servo;
};

#endif