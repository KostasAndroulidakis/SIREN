#include "config.h"
#include "Ultrasonic.h"
#include "Servo.h"
#include "Scanner.h"
#include "DHTSensor.h"
#include "SpeedOfSound.h"

Ultrasonic ultrasonic;
ServoMotor servo;
DHTSensor dht;
Scanner scanner(&ultrasonic, &servo);

void setup() {
    Serial.begin(SERIAL_BAUD);
    Serial.println("=== SONAR SYSTEM ===");
    
    dht.init();
    ultrasonic.init();
    servo.init();
}

void loop() {
    THReading envData;
    dht.read(&envData);
    
    float soundSpeed = envData.valid ? 
        calculateSpeedOfSound(envData.temperatureC, envData.humidity) : 343.0;
    
    if (!envData.valid) {
        Serial.println("Using default sound speed");
    }
    
    scanner.scan(&envData, soundSpeed);
}