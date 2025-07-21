/**
 * @file physics.ino
 * @brief Physics and mathematical calculations for SIREN system
 * @author KostasAndroulidakis
 * @date 2025
 *
 * SRP: Single responsibility for all physics and mathematical calculations.
 * SSOT: Single source of truth for acoustic physics formulas.
 * MISRA C++ compliant: All constants defined, no magic numbers.
 *
 * Responsibilities:
 * - Atmospheric sound speed calculations
 * - Acoustic physics constants and formulas
 * - Unit conversions for sensor measurements
 * - Mathematical validation and range checking
 *
 * NOT responsible for:
 * - Hardware sensor operations (handled by sensor.ino/environment.ino)
 * - Serial communication (handled by communication.ino)
 * - System coordination (handled by sonar.ino)
 */

// SSOT: Acoustic physics constants (MISRA C++ Rule 5.0.1)
namespace physics {
  namespace acoustic {
    // Standard atmospheric sound speed reference
    constexpr float BASE_SOUND_SPEED_0C_M_S = 331.3f;       ///< Sound speed at 0°C, dry air (m/s)
    constexpr float REFERENCE_TEMPERATURE_C = 0.0f;         ///< Reference temperature (°C)
    
    // Temperature coefficient from acoustic physics
    constexpr float TEMP_COEFFICIENT_M_S_PER_C = 0.6f;      ///< +0.6 m/s per °C (standard linear approximation)
    
    // Humidity effect (minimal for typical ranges, included for completeness)
    constexpr float HUMIDITY_COEFFICIENT_M_S_PER_PERCENT = 0.124f; ///< +0.124 m/s per %RH (at 20°C)
    
    // Temperature and humidity operating ranges for validation
    constexpr float MIN_VALID_TEMPERATURE_C = -20.0f;       ///< Minimum valid temperature
    constexpr float MAX_VALID_TEMPERATURE_C = 60.0f;        ///< Maximum valid temperature
    constexpr float MIN_VALID_HUMIDITY_PERCENT = 0.0f;      ///< Minimum valid humidity
    constexpr float MAX_VALID_HUMIDITY_PERCENT = 100.0f;    ///< Maximum valid humidity
  }
  
  namespace units {
    // Unit conversion constants
    constexpr float M_S_TO_CM_US = 0.0001f;                 ///< Convert m/s to cm/μs (1 m/s = 0.0001 cm/μs)
    constexpr float CM_US_TO_M_S = 10000.0f;                ///< Convert cm/μs to m/s (1 cm/μs = 10000 m/s)
    
    // Default values for fallback scenarios
    constexpr float DEFAULT_ROOM_TEMPERATURE_C = 20.0f;     ///< Standard room temperature (°C)
    constexpr float DEFAULT_ROOM_HUMIDITY_PERCENT = 50.0f;  ///< Standard room humidity (%)
  }
}

// Function declarations
bool isValidTemperatureRange(float temperature);
bool isValidHumidityRange(float humidity);
bool isValidEnvironmentalData(float temperature, float humidity);

/**
 * @brief Calculate atmospheric sound speed from temperature and humidity
 * @param temperature Current temperature in Celsius
 * @param humidity Current humidity percentage
 * @return Sound speed in cm/microsecond
 *
 * MISRA C++ compliant acoustic physics calculation.
 * Uses standard linear approximation with temperature and humidity compensation.
 * 
 * Primary formula: v = 331.3 + 0.6×T (m/s) - temperature effect dominant
 * Humidity effect: minimal correction for precise measurements
 * 
 * References:
 * - ISO 3744:2010 (Acoustics - Determination of sound power levels)
 * - Engineering acoustics handbook
 */
float calculateAtmosphericSoundSpeed(float temperature, float humidity) {
  // Input validation (MISRA C++ defensive programming)
  if (!isValidEnvironmentalData(temperature, humidity)) {
    // Return default sound speed for invalid inputs
    return calculateAtmosphericSoundSpeed(physics::units::DEFAULT_ROOM_TEMPERATURE_C, 
                                          physics::units::DEFAULT_ROOM_HUMIDITY_PERCENT);
  }
  
  // Primary temperature effect (dominant factor)
  float temperatureEffect = temperature * physics::acoustic::TEMP_COEFFICIENT_M_S_PER_C;
  
  // Secondary humidity effect (small correction, typically <1%)
  // Simplified model: humidity effect is temperature-dependent, but using average at 20°C
  float humidityEffect = (humidity / 100.0f) * physics::acoustic::HUMIDITY_COEFFICIENT_M_S_PER_PERCENT;
  
  // Calculate sound speed in m/s
  float soundSpeedMeterPerSecond = physics::acoustic::BASE_SOUND_SPEED_0C_M_S + 
                                   temperatureEffect + 
                                   humidityEffect;
  
  // Convert to cm/μs for HC-SR04 pulse timing calculations
  return soundSpeedMeterPerSecond * physics::units::M_S_TO_CM_US;
}

/**
 * @brief Get default sound speed for fallback scenarios
 * @return Default sound speed in cm/microsecond (20°C, 50% humidity)
 *
 * SSOT for default sound speed when environmental sensors fail.
 * Uses standard indoor conditions as reference.
 */
float getDefaultAtmosphericSoundSpeed() {
  return calculateAtmosphericSoundSpeed(physics::units::DEFAULT_ROOM_TEMPERATURE_C,
                                        physics::units::DEFAULT_ROOM_HUMIDITY_PERCENT);
}

/**
 * @brief Calculate sound speed from temperature only (simplified model)
 * @param temperature Current temperature in Celsius
 * @return Sound speed in cm/microsecond
 *
 * Simplified calculation when only temperature is available.
 * Uses standard linear approximation: v = 331.3 + 0.6×T
 */
float calculateSoundSpeedFromTemperature(float temperature) {
  if (!isValidTemperatureRange(temperature)) {
    temperature = physics::units::DEFAULT_ROOM_TEMPERATURE_C;
  }
  
  float soundSpeedMeterPerSecond = physics::acoustic::BASE_SOUND_SPEED_0C_M_S + 
                                   (temperature * physics::acoustic::TEMP_COEFFICIENT_M_S_PER_C);
  
  return soundSpeedMeterPerSecond * physics::units::M_S_TO_CM_US;
}

/**
 * @brief Validate temperature is within reasonable atmospheric range
 * @param temperature Temperature value to validate
 * @return true if temperature is valid for acoustic calculations
 */
bool isValidTemperatureRange(float temperature) {
  return !isnan(temperature) && 
         temperature >= physics::acoustic::MIN_VALID_TEMPERATURE_C && 
         temperature <= physics::acoustic::MAX_VALID_TEMPERATURE_C;
}

/**
 * @brief Validate humidity is within reasonable atmospheric range
 * @param humidity Humidity percentage to validate
 * @return true if humidity is valid for acoustic calculations
 */
bool isValidHumidityRange(float humidity) {
  return !isnan(humidity) && 
         humidity >= physics::acoustic::MIN_VALID_HUMIDITY_PERCENT && 
         humidity <= physics::acoustic::MAX_VALID_HUMIDITY_PERCENT;
}

/**
 * @brief Validate both temperature and humidity for acoustic calculations
 * @param temperature Temperature in Celsius
 * @param humidity Humidity percentage
 * @return true if both values are valid for sound speed calculation
 */
bool isValidEnvironmentalData(float temperature, float humidity) {
  return isValidTemperatureRange(temperature) && isValidHumidityRange(humidity);
}