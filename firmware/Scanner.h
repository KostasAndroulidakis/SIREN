// Scanner.h

#ifndef SCANNER_H
#define SCANNER_H

#include "config.h"
#include "Ultrasonic.h"
#include "Servo.h"

class Scanner {
public:
    Scanner(Ultrasonic* ultra, ServoMotor* srv);
    void scan(THReading* envData, float soundSpeed);
    
private:
    Ultrasonic* ultrasonic;
    ServoMotor* servo;
    void printData(int angle, float distance, THReading* envData);
};

#endif