// DHTSensor.cpp
// Non-blocking DHT11 temperature/humidity reader with caching

#include <Arduino.h>
#include "DHTSensor.h"

// TIMING:
// DHT11 datasheet requires minimum 1 second between readings.
// We use 2 seconds for safety margin and because environmental
// conditions don't change rapidly anyway.
#define DHT_READ_INTERVAL 2000

DHTSensor::DHTSensor() : dht(DHT_PIN, DHT_TYPE), lastReadTime(0) {
    lastReading.valid = false;
}

void DHTSensor::init() {
    dht.begin();

    // DHT11 needs 1 second after power-on before first reading
    // (datasheet: "pass the unstable status")
    delay(1000);

    Serial.println(F("DHT11 initialized"));
}

bool DHTSensor::read(THReading* result) {
    unsigned long now = millis();
    
    // CACHING PATTERN:
    // If called too soon, return the cached reading without
    // blocking. This allows the main loop to continue smoothly.
    if (now - lastReadTime < DHT_READ_INTERVAL) {
        *result = lastReading;
        return false;  // not a new reading
    }
    
    lastReadTime = now;
    
    // Read sensor (this blocks for ~25ms)
    float h = dht.readHumidity();
    float tC = dht.readTemperature();
    float tF = dht.readTemperature(true);
    
    // Check for NaN (sensor communication error)
    if (isnan(h) || isnan(tC) || isnan(tF)) {
        Serial.println(F("DHT read failed"));
        result->valid = false;
        return false;
    }

    // Validate against sensor's reliable range
    // Values outside this range indicate sensor malfunction
    if (tC < DHT_MIN_TEMP || tC > DHT_MAX_TEMP ||
        h < DHT_MIN_HUMIDITY || h > DHT_MAX_HUMIDITY) {
        Serial.println(F("DHT reading out of range"));
        result->valid = false;
        return false;
    }
    
    // Cache the valid reading
    lastReading.humidity = h;
    lastReading.temperatureC = tC;
    lastReading.temperatureF = tF;
    lastReading.valid = true;
    
    *result = lastReading;
    return true;  // new reading taken
}