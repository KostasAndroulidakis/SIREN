// DHTSensor.h
// DHT11 Temperature & Humidity Sensor Driver
//
// PURPOSE:
// Provides environmental data to calculate accurate speed of sound.
// Uses caching to avoid blocking the main loop.
//
// DHT11 SPECS (from datasheet):
//   - Temperature range: 0-50°C (±2°C accuracy)
//   - Humidity range: 20-90% RH (±5% accuracy)
//   - Sampling period: minimum 1 second between readings
//   - Startup time: 1 second after power-on
//
// NON-BLOCKING PATTERN:
// DHT11 readings take ~25ms and block execution.
// Instead of reading every loop, we cache the last valid
// reading and only update every 2 seconds. This keeps
// the radar sweep smooth while still tracking environmental
// changes.

#ifndef DHTSENSOR_H
#define DHTSENSOR_H

#include "config.h"
#include <DHT.h>

// DHT11 sensor type for library
#define DHT_TYPE DHT11

// Valid ranges (from datasheet)
// Readings outside these are likely sensor errors
#define DHT_MIN_TEMP 0.0
#define DHT_MAX_TEMP 50.0
#define DHT_MIN_HUMIDITY 20.0
#define DHT_MAX_HUMIDITY 90.0

class DHTSensor {
public:
    DHTSensor();
    void init();
    bool read(THReading* result);   // Returns true if new reading taken
    
private:
    DHT dht;
    unsigned long lastReadTime;
    THReading lastReading;          // Cached reading for non-blocking access
};

#endif