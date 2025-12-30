// Scanner.cpp
// Orchestrates servo sweep with ultrasonic measurements

#include <Arduino.h>
#include "Servo.h"
#include "Scanner.h"
#include "config.h"

// DEPENDENCY INJECTION:
// Scanner doesn't create its own components - they're passed in.
// This allows the main program to control initialization order
// and makes unit testing possible (you could pass mock objects).
Scanner::Scanner(Ultrasonic* ultra, ServoMotor* srv, Alert* alrt, Button* btn) {
    ultrasonic = ultra;
    servo = srv;
    alert = alrt;
    button = btn;
}

// OUTPUT FORMAT:
// CSV with 5 fields per line, no header row.
// This format is easy to parse in Processing, Python, or any
// serial monitor that supports CSV logging.
void Scanner::printData(int angle, float distance, THReading* envData) {
    Serial.print(angle);
    Serial.print(",");
    Serial.print(distance);
    Serial.print(",");
    if (envData->valid) {
        Serial.print(envData->humidity);
        Serial.print(",");
        Serial.print(envData->temperatureC);
        Serial.print(",");
        Serial.println(envData->temperatureF);
    } else {
        Serial.println(",,");
    }
}

bool Scanner::scan(THReading* envData, float soundSpeed) {
    // BIDIRECTIONAL SWEEP SETUP:
    // Instead of always starting at 0, we alternate directions.
    // angles[0]=0, steps[0]=+1   → sweep 0 to 180
    // angles[1]=180, steps[1]=-1 → sweep 180 to 0
    int angles[] = {SERVO_MIN_ANGLE, SERVO_MAX_ANGLE};
    int steps[] = {1, -1};
    
    // Perform both sweeps (forward and backward)
    for (int i = 0; i < 2; i++) {
        int angle = angles[i];
        int step = steps[i];
        
        // SWEEP LOOP:
        // Continue while angle is within valid range
        // Forward:  while angle <= SERVO_MAX_ANGLE
        // Backward: while angle >= SERVO_MIN_ANGLE
        while ((step > 0 && angle <= SERVO_MAX_ANGLE) || (step < 0 && angle >= SERVO_MIN_ANGLE)) {
            
            // CHECK FOR INTERRUPT:
            // Allow user to stop mid-sweep by pressing button.
            // Without this, user would have to wait up to 22 seconds
            // for the sweep to complete.
            if (button->isPressed()) {
                alert->stop();
                return false;   // Signal that scan was interrupted
            }

            // MOVE SERVO:
            // Position the sensor at current angle
            servo->setAngle(angle);
            delay(SERVO_DELAY);     // Wait for servo to reach position
            
            // TAKE MEASUREMENT:
            // Detach servo to free Timer1 for ultrasonic Input Capture
            // See Servo.h for detailed explanation
            servo->detach();             // cut PWM after move
            delayMicroseconds(50);       // stabilization

            float distance = ultrasonic->getDistance(soundSpeed);

            servo->attach();
            delay(5);
            
            // UPDATE ALERT:
            // Check if object is within alert threshold and
            // update LED/buzzer accordingly
            alert->update(distance);

            // SEND DATA:
            // Transmit measurement immediately for real-time display
            printData(angle, distance, envData);
            
            // NEXT STEP:
            // Move to next angle (increment or decrement based on direction)
            angle += step;
        }
    }
    return true;    // Scan completed without interruption
}