// Scanner.h

#ifndef SCANNER_H
#define SCANNER_H

#include "config.h"
#include "Ultrasonic.h"
#include "Servo.h"
#include "Alert.h"
#include "Button.h"

class Scanner {
public:
    Scanner(Ultrasonic* ultra, ServoMotor* srv, Alert* alrt, Button* btn);
    bool scan(THReading* envData, float soundSpeed);  // returns false if interrupted
    
private:
    Ultrasonic* ultrasonic;
    ServoMotor* servo;
    Alert* alert;
    Button* button;
    void printData(int angle, float distance, THReading* envData);
};

#endif