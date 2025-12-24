// DHTSensor.h

#ifndef DHTSENSOR_H
#define DHTSENSOR_H

#include "config.h"
#include <DHT.h>

class DHTSensor {
public:
    DHTSensor();
    void init();
    void read(THReading* result);
private:
    DHT dht;
};

#endif