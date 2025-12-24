// firmware.ino
// SIREN - Sonic Imaging for Range Exploration and Navigation
//
// Main entry point for the ultrasonic radar system.
// Coordinates all components and implements the state machine.
//
// SYSTEM OVERVIEW:
// 1. User presses button to start scanning
// 2. DHT11 reads temperature/humidity (cached, non-blocking)
// 3. Speed of sound calculated from environmental data
// 4. Servo sweeps 0→180→0 while ultrasonic measures distance
// 5. Alert activates if objects detected within 100cm
// 6. Data transmitted via serial in CSV format
// 7. User presses button again to stop
//
// SERIAL OUTPUT FORMAT:
// 115200 baud, CSV: angle,distance,humidity,tempC,tempF
//
// HARDWARE CONNECTIONS: See config.h and docs/hardware.md

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

// Sensor instances
Ultrasonic ultrasonic;
DHTSensor dht;

// Actuator instances
ServoMotor servo;
Alert alert;

// Input instance
Button button;

// Scanner orchestrator - receives pointers to all components
// This is dependency injection pattern
Scanner scanner(&ultrasonic, &servo, &alert, &button);

// STATE MACHINE:
// Simple two-state machine: scanning or idle
// Controlled by button press (toggle)
bool scanning = false;

// ===========================================
// SETUP
// ===========================================

void setup() {
    // Initialize serial communication
    // 115200 baud for fast data transmission during sweep
    Serial.begin(SERIAL_BAUD);
    Serial.println(F("=== S I R E N ==="));
    Serial.println(F("Sonic Imaging for Range Exploration and Navigation"));
    
    // Initialize all components
    // Order matters: DHT needs 1 second startup delay
    dht.init();
    ultrasonic.init();
    servo.init();
    alert.init();
    button.init();
    
    Serial.println(F("Press button to start/stop"));
}

// ===========================================
// MAIN LOOP
// ===========================================

void loop() {
    // STATE TOGGLE:
    // Check for button press and toggle scanning state
    if (button.isPressed()) {
        scanning = !scanning;
        Serial.println(scanning ? F("SCAN STARTED") : F("SCAN STOPPED"));
        
        // Ensure alert is off when stopping
        if (!scanning) {
            // Print CSV header for data parsing
            Serial.println(F("angle,distance,humidity,temperatureC,temperatureF"));
        } else {
            alert.stop();
        }
    }
    
    // IDLE STATE:
    // If not scanning, exit loop early (saves power, reduces noise)
    if (!scanning) {
        return;
    }
    
    // ===========================================
    // ACTIVE SCANNING
    // ===========================================
    
    // READ ENVIRONMENT:
    // Get temperature and humidity for speed of sound calculation.
    // This is non-blocking - returns cached value if called too soon.
    THReading envData;
    dht.read(&envData);
    
    // CALCULATE SPEED OF SOUND:
    // Use environmental data if valid, otherwise fall back to
    // standard value (343 m/s at 20°C, 50% humidity).
    // See SpeedOfSound.h for the physics explanation.
    float soundSpeed;
    if (envData.valid) {
        soundSpeed = calculateSpeedOfSound(envData.temperatureC, envData.humidity);
    } else {
        soundSpeed = 343.0;     // Fallback: standard conditions
    }
    
    // PERFORM SCAN:
    // Execute one complete bidirectional sweep (0→180→0).
    // Returns false if interrupted by button press.
    if (!scanner.scan(&envData, soundSpeed)) {
        scanning = false;
        Serial.println(F("SCAN STOPPED"));
    }
}