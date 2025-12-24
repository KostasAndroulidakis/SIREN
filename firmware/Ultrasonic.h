// Ultrasonic.h
// HC-SR04 Ultrasonic Distance Sensor Driver
//
// PRINCIPLE OF OPERATION:
// The sensor emits an ultrasonic pulse (40kHz) and measures
// the time it takes for the echo to return after bouncing
// off an object. Distance is calculated from this time
// using the speed of sound.
//
// PHYSICS:
//   distance = (time × speed_of_sound) / 2
//   We divide by 2 because the sound travels TO the object
//   and BACK, so the measured time is for twice the distance.
//
// SENSOR SPECS (from datasheet):
//   - Range: 2cm to 400cm
//   - Accuracy: 3mm
//   - Beam angle: 15 degrees
//   - Measurement cycle: minimum 60ms between readings

#ifndef ULTRASONIC_H
#define ULTRASONIC_H

#include "config.h"

// HC-SR04 sensor limits (from datasheet)
#define MIN_DISTANCE 2          // Minimum distance in cm
#define MAX_DISTANCE 400        // Maximum distance in cm

class Ultrasonic {
public:
    void init();
    float getDistance(float soundSpeed);    // Returns distance in cm, or -1 if invalid
};

#endif