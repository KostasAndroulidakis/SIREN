/**
 * @file sensor.ino
 * @brief Ultrasonic sensor module for SIREN system
 * @author KostasAndroulidakis
 * @date 2025
 *
 * Manages the HC-SR04 ultrasonic sensor for distance measurements.
 * Handles sensor initialization, distance calculation using sound wave
 * travel time, and data validation for reliable sonar measurements.
 */

// Sensor hardware pin assignments
const int TRIG_PIN = 2;                        ///< Digital pin for trigger signal
const int ECHO_PIN = 3;                        ///< Digital pin for echo signal

// Measurement range limits
const int MIN_DISTANCE = 2;                    ///< Minimum reliable distance in cm
const int MAX_DISTANCE = 400;                  ///< Maximum reliable distance in cm

// Timing constants for sensor operation
const int TRIGGER_DELAY_LOW_US = 2;            ///< Low trigger pulse duration in microseconds
const int TRIGGER_DELAY_HIGH_US = 10;          ///< High trigger pulse duration in microseconds

// SSOT: Distance calculation constants (MISRA C++ Rule 5.0.1)
namespace {
  constexpr float DEFAULT_SOUND_SPEED_CM_US = 0.034f; ///< Default sound speed (cm/μs) fallback
  constexpr int SOUND_TRAVEL_DIVISOR = 2;             ///< Round-trip divisor (sound travels to target and back)
}

// Function declarations
bool isValidDistance(int distance);

/**
 * @brief Initialize the ultrasonic sensor
 *
 * Configures the trigger pin as output and echo pin as input.
 * Must be called before any distance measurement attempts.
 */
void initSensor() {
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
}

/**
 * @brief Measure distance using default sound speed (legacy compatibility)
 * @return Distance in centimeters (0 if measurement is invalid)
 *
 * MISRA C++ compliant distance measurement with default sound speed.
 * For backward compatibility when environmental data unavailable.
 */
int getDistance() {
  return getCalibratedDistance(DEFAULT_SOUND_SPEED_CM_US);
}

/**
 * @brief Measure distance using real-time calibrated sound speed
 * @param soundSpeedCmPerUs Real-time sound speed in cm/microsecond
 * @return Distance in centimeters (0 if measurement is invalid)
 *
 * SRP: Single responsibility for calibrated distance measurement.
 * SSOT: Single source of truth for HC-SR04 sensor operation.
 * MISRA C++ Rule 5.0.1: All constants defined, no magic numbers.
 *
 * Process:
 * 1. Sends 10μs trigger pulse (HC-SR04 specification)
 * 2. Measures echo pulse duration
 * 3. Calculates distance using real-time sound speed
 * 4. Validates measurement against sensor range
 */
int getCalibratedDistance(float soundSpeedCmPerUs) {
  // Send trigger pulse (HC-SR04 protocol)
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(TRIGGER_DELAY_LOW_US);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(TRIGGER_DELAY_HIGH_US);
  digitalWrite(TRIG_PIN, LOW);

  // Measure echo pulse duration
  long duration = pulseIn(ECHO_PIN, HIGH);

  // Calculate calibrated distance: (duration * real_time_sound_speed) / 2
  // Division by 2 because sound travels to target and back (round trip)
  int distance = static_cast<int>((duration * soundSpeedCmPerUs) / SOUND_TRAVEL_DIVISOR);

  return isValidDistance(distance) ? distance : 0;
}

/**
 * @brief Validate distance measurement
 * @param distance Measured distance in centimeters
 * @return true if distance is within valid range, false otherwise
 *
 * Checks if the measured distance falls within the sensor's reliable
 * operating range to filter out erroneous readings.
 */
bool isValidDistance(int distance) {
  return (distance >= MIN_DISTANCE && distance <= MAX_DISTANCE);
}
