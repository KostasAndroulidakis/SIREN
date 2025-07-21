/**
 * @file environment.ino
 * @brief Environmental sensor module for SIREN system
 * @author KostasAndroulidakis
 * @date 2025
 *
 * Manages the DHT11 temperature and humidity sensor for environmental
 * monitoring. Provides temperature and humidity readings that can be
 * used for atmospheric compensation of sonar measurements and system
 * environmental monitoring.
 */

#include <DHT.h>
#include <Adafruit_Sensor.h>

// Environmental sensor hardware configuration
const int DHT_PIN = 4;                     ///< Digital pin connected to DHT11 data line
const int DHT_TYPE = DHT11;                ///< DHT sensor type (DHT11)

// DHT11 timing constraints (from datasheet)
const int DHT_READ_INTERVAL_MS = 2000;     ///< Minimum interval between readings (2 seconds)
const int DHT_STARTUP_DELAY_MS = 1000;     ///< Startup stabilization time (1 second)

// DHT11 actual measurement limits (from datasheet)
const float MIN_TEMPERATURE = 0.0;        ///< DHT11 minimum temperature (°C)
const float MAX_TEMPERATURE = 50.0;       ///< DHT11 maximum temperature (°C)  
const float MIN_HUMIDITY = 20.0;          ///< DHT11 minimum humidity (%)
const float MAX_HUMIDITY = 90.0;          ///< DHT11 maximum humidity (%)

// DHT sensor object
DHT environmentSensor(DHT_PIN, DHT_TYPE);

// Timing control for sensor readings
unsigned long lastReadingTime = 0;

// Function declarations
bool isValidTemperature(float temperature);
bool isValidHumidity(float humidity);
bool canReadSensor();

/**
 * @brief Initialize the environmental sensor
 *
 * Configures the DHT11 sensor and allows startup stabilization time.
 * Must be called before any environmental measurement attempts.
 */
void initEnvironment() {
  environmentSensor.begin();
  delay(DHT_STARTUP_DELAY_MS);  // DHT11 requires 1-2 second startup time
  lastReadingTime = millis() - DHT_READ_INTERVAL_MS;  // Allow immediate first reading
}

/**
 * @brief Read temperature from DHT11 sensor
 * @return Temperature in Celsius (NAN if reading failed or too soon)
 *
 * Reads temperature with proper timing constraints. DHT11 requires
 * minimum 2-second interval between readings for reliable operation.
 */
float getTemperature() {
  if (!canReadSensor()) {
    return NAN;  // Too soon for another reading
  }

  float temperature = environmentSensor.readTemperature();
  
  // Update reading timestamp on successful read attempt
  lastReadingTime = millis();
  
  return isValidTemperature(temperature) ? temperature : NAN;
}

/**
 * @brief Read humidity from DHT11 sensor
 * @return Humidity percentage (NAN if reading failed or too soon)
 *
 * Reads humidity with proper timing constraints. DHT11 requires
 * minimum 2-second interval between readings for reliable operation.
 */
float getHumidity() {
  if (!canReadSensor()) {
    return NAN;  // Too soon for another reading
  }

  float humidity = environmentSensor.readHumidity();
  
  // Update reading timestamp on successful read attempt  
  lastReadingTime = millis();
  
  return isValidHumidity(humidity) ? humidity : NAN;
}

/**
 * @brief Read both temperature and humidity in single operation
 * @param temperature Reference to store temperature value
 * @param humidity Reference to store humidity value
 * @return true if both readings are valid, false otherwise
 *
 * Optimized function to read both environmental parameters in one
 * sensor access cycle, respecting DHT11 timing constraints.
 */
bool getEnvironmentalData(float &temperature, float &humidity) {
  if (!canReadSensor()) {
    return false;  // Too soon for another reading
  }

  // Try reading DHT11 up to 3 times for reliability
  for (int attempt = 1; attempt <= 3; attempt++) {
    temperature = environmentSensor.readTemperature();
    humidity = environmentSensor.readHumidity();
    
    bool tempValid = isValidTemperature(temperature);
    bool humValid = isValidHumidity(humidity);
    
    if (tempValid && humValid) {
      lastReadingTime = millis();
      return true;  // Success!
    }
    
    // Wait 100ms before retry (DHT11 can be finicky)
    if (attempt < 3) {
      delay(100);
    }
  }
  
  lastReadingTime = millis();
  return false;
}

// Sound speed calculations moved to physics.ino for better SRP compliance

/**
 * @brief Check if sensor can be read (timing constraint)
 * @return true if minimum interval has passed, false otherwise
 *
 * DHT11 requires minimum 2-second interval between readings.
 * Prevents sensor communication errors due to timing violations.
 */
bool canReadSensor() {
  return (millis() - lastReadingTime) >= DHT_READ_INTERVAL_MS;
}

/**
 * @brief Validate temperature reading
 * @param temperature Temperature value to validate
 * @return true if temperature is within DHT11 operating range
 */
bool isValidTemperature(float temperature) {
  return !isnan(temperature) && 
         temperature >= MIN_TEMPERATURE && 
         temperature <= MAX_TEMPERATURE;
}

/**
 * @brief Validate humidity reading  
 * @param humidity Humidity value to validate
 * @return true if humidity is within DHT11 operating range
 */
bool isValidHumidity(float humidity) {
  return !isnan(humidity) && 
         humidity >= MIN_HUMIDITY && 
         humidity <= MAX_HUMIDITY;
}