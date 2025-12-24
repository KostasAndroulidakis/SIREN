// DHSTSensor.cpp

#include <Arduino.h>
#include "DHTSensor.h"

DHTSensor::DHTSensor() : dht(DHT_PIN, DHT_TYPE) {}

void DHTSensor::init() {
    dht.begin();
    Serial.println("DHT11 initialized");
}

void DHTSensor::read(THReading* result) {
    result->valid = false;
    
    delay(2000);
    
    float h = dht.readHumidity();
    float tC = dht.readTemperature();
    float tF = dht.readTemperature(true);
    
    if (isnan(h) || isnan(tC)) {
        Serial.println("DHT read failed");
        return;
    }
    
    result->humidity = h;
    result->temperatureC = tC;
    result->temperatureF = tF;
    result->valid = true;
}