// SpeedOfSound.h
// Environmental Speed of Sound Calculator
//
// WHY THIS MATTERS:
// The speed of sound is NOT constant - it varies with air conditions.
// Most ultrasonic examples assume 343 m/s (at 20°C, 50% humidity),
// but this can cause errors up to 5% in different conditions.
//
// PHYSICS:
// Sound travels through air by compressing and expanding gas molecules.
// 
// Temperature effect (dominant):
//   - Warmer air → molecules move faster → sound travels faster
//   - Each 1°C increase → ~0.6 m/s faster
//   - At 0°C: 331 m/s, at 30°C: 349 m/s
//
// Humidity effect (secondary):
//   - Water vapor is lighter than N2/O2 molecules
//   - More humidity → less dense air → slightly faster sound
//   - Effect is small: ~0.01 m/s per 1% RH

#ifndef SPEEDOFSOUND_H
#define SPEEDOFSOUND_H

// Calculate speed of sound based on environmental conditions
// Parameters:
//   tempC    - Temperature in Celsius (valid: 0-50°C for DHT11)
//   humidity - Relative humidity in % (valid: 20-90% for DHT11)
// Returns:
//   Speed of sound in m/s
float calculateSpeedOfSound(float tempC, float humidity);

#endif