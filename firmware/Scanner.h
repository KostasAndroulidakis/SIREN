// Scanner.h
// Radar Sweep Orchestrator
//
// PURPOSE:
// Coordinates the servo motor and ultrasonic sensor to perform
// a complete radar sweep. Outputs measurements in real-time
// via serial connection.
//
// SWEEP PATTERN:
// Bidirectional sweep for smooth visualization:
//   1. Forward:  0° → 180° (left to right)
//   2. Backward: 180° → 0° (right to left)
//   3. Repeat...
//
// This creates a continuous scanning motion without the
// jarring snap-back that a unidirectional sweep would have.
//
// TIMING:
// Each step takes approximately:
//   60ms servo delay + 2ms trigger + up to 30ms echo
//   ≈ 90-100ms per degree
// Full sweep (362 steps): ~22 seconds
//
// OUTPUT FORMAT (CSV):
// angle,distance,humidity,temperatureC,temperatureF
// Each line is one measurement, sent as soon as taken.
// This allows real-time visualization by the receiving software.

#ifndef SCANNER_H
#define SCANNER_H

#include "config.h"
#include "Ultrasonic.h"
#include "Servo.h"
#include "Alert.h"
#include "Button.h"

class Scanner {
public:
    // Constructor takes pointers to all required components
    // This is dependency injection - makes testing easier
    Scanner(Ultrasonic* ultra, ServoMotor* srv, Alert* alrt, Button* btn);
    
    // Perform one complete bidirectional sweep
    // Returns false if interrupted by button press
    bool scan(THReading* envData, float soundSpeed);

private:
    Ultrasonic* ultrasonic;
    ServoMotor* servo;
    Alert* alert;
    Button* button;

    // Output one measurement line to serial
    void printData(int angle, float distance, THReading* envData);
};

#endif