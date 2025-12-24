// Scanner.cpp
// Orchestrates servo sweep with ultrasonic measurements

#include <Arduino.h>
#include "Scanner.h"

Scanner::Scanner(Ultrasonic* ultra, ServoMotor* srv, Alert* alrt, Button* btn) {
    ultrasonic = ultra;
    servo = srv;
    alert = alrt;
    button = btn;
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

bool Scanner::scan(THReading* envData, float soundSpeed) {
    // Bidirectional sweep: 0->180, then 180->0
    int angles[] = {0, 180};
    int steps[] = {1, -1};
    
    for (int i = 0; i < 2; i++) {
        int angle = angles[i];
        int step = steps[i];
        
        while ((step > 0 && angle <= 180) || (step < 0 && angle >= 0)) {
            if (button->isPressed()) {
                alert->stop();
                return false;
            }
            
            servo->setAngle(angle);
            delay(SERVO_DELAY);
            
            servo->detach();
            float distance = ultrasonic->getDistance(soundSpeed);
            servo->attach();
            
            alert->update(distance);
            printData(angle, distance, envData);
            
            angle += step;
        }
    }
    return true;
}