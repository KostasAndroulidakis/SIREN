#include "Alert.h"
#include "Button.h"
#include "config.h"
#include "DHTSensor.h"
#include "Scanner.h"
#include "Servo.h"
#include "SpeedOfSound.h"
#include "Ultrasonic.h"

// ===========================================
// GLOBAL OBJECTS
// ===========================================

Ultrasonic ultrasonic;
ServoMotor servo;
DHTSensor dht;
Alert alert;
Button button;
Scanner scanner(&ultrasonic, &servo, &alert, &button);

bool scanning = false;

// ===========================================
// SETUP
// ===========================================

void setup() {
    Serial.begin(SERIAL_BAUD);
    Serial.println("=== SONAR SYSTEM ===");
    
    dht.init();
    ultrasonic.init();
    servo.init();
    alert.init();
    button.init();
    
    Serial.println("Press button to start/stop");
}

// ===========================================
// MAIN LOOP
// ===========================================

void loop() {
    // Toggle scanning on button press
    if (button.isPressed()) {
        scanning = !scanning;
        Serial.println(scanning ? "SCAN STARTED" : "SCAN STOPPED");
        
        if (!scanning) {
            alert.stop();
        }
    }
    
    if (!scanning) {
        return;
    }
    
    // Read environment (non-blocking)
    THReading envData;
    dht.read(&envData);
    
    // Speed of sound (fallback 343 m/s)
    float soundSpeed = envData.valid ? 
        calculateSpeedOfSound(envData.temperatureC, envData.humidity) : 343.0;
    
    // Run scan, check if interrupted
    if (!scanner.scan(&envData, soundSpeed)) {
        scanning = false;
        Serial.println("SCAN STOPPED");
    }
}