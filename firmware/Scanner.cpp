// Scanner.cpp

#include <Arduino.h>
#include "Scanner.h"

Scanner::Scanner(Ultrasonic* ultra, ServoMotor* srv) {
    ultrasonic = ultra;
    servo = srv;
}

void Scanner::printData(int angle, float distance, THReading* envData) {
    Serial.print(angle);
    Serial.print(",");
    Serial.print(distance);
    Serial.print(",");
    Serial.print(envData->humidity);
    Serial.print(",");
    Serial.print(envData->temperatureC);
    Serial.print(",");
    Serial.println(envData->temperatureF);
}

void Scanner::scan(THReading* envData, float soundSpeed) {
    int angles[] = {0, 180};
    int steps[] = {1, -1};
    
    for (int i = 0; i < 2; i++) {
        int angle = angles[i];
        int step = steps[i];
        
        while ((step > 0 && angle <= 180) || (step < 0 && angle >= 0)) {
            servo->setAngle(angle);
            delay(60);
            
            float distance = ultrasonic->getDistance(soundSpeed);
            printData(angle, distance, envData);
            
            angle += step;
        }
        delay(60);
    }
}