// SpeedOfSound.cpp

#include <Arduino.h>
#include "SpeedOfSound.h"

float calculateSpeedOfSound(float tempC, float humidity) {
    return 331.3 + (0.606 * tempC) + (0.0124 * humidity);
}