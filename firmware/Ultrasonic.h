// Ultrasonic.h

#ifndef ULTRASONIC_H
#define ULTRASONIC_H

#include "config.h"

class Ultrasonic {
public:
    void init();
    float getDistance(float soundSpeed);
};

#endif