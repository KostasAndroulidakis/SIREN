// SpeedOfSound.cpp
// Environmental Speed of Sound Calculator

#include <Arduino.h>
#include "SpeedOfSound.h"

// FORMULA DERIVATION:
// The simplified formula for speed of sound in air:
//
//   c = 331.3 + 0.606×T + 0.0124×RH
//
// Where:
//   c  = speed of sound (m/s)
//   T  = temperature (°C)
//   RH = relative humidity (%)
//
// BREAKDOWN:
//   331.3       Base speed at 0°C, dry air
//   0.606×T     Temperature contribution (~0.6 m/s per °C)
//   0.0124×RH   Humidity contribution (~0.01 m/s per % RH)
//
// EXAMPLES:
//   At 20°C, 50% RH: c = 331.3 + 12.12 + 0.62 = 344.0 m/s
//   At 30°C, 80% RH: c = 331.3 + 18.18 + 0.99 = 350.5 m/s
//   At  0°C,  0% RH: c = 331.3 +  0    + 0    = 331.3 m/s
//
// NOTE: This is a simplified approximation. The exact formula
// involves the adiabatic index and molar mass of humid air.
// For our distance range (2-400cm), this approximation gives
// accuracy within 0.5%.

float calculateSpeedOfSound(float tempC, float humidity) {
    return 331.3 + (0.606 * tempC) + (0.0124 * humidity);
}