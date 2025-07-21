/**
 * @file sensor.ino
 * @brief Ultrasonic sensor module for SIREN system
 * @author SIREN Project
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

// Distance calculation constants
const float SOUND_SPEED_CONVERSION = 0.034;    ///< Sound speed conversion factor (cm/μs)
const int SOUND_TRAVEL_DIVISOR = 2;            ///< Divisor for round-trip time calculation

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
 * @brief Measure distance using ultrasonic sensor
 * @return Distance in centimeters (0 if measurement is invalid)
 *
 * Performs distance measurement using the HC-SR04 sensor:
 * 1. Sends 10μs trigger pulse
 * 2. Measures echo pulse duration
 * 3. Calculates distance using sound speed
 * 4. Validates measurement range
 *
 * Returns 0 for invalid measurements (out of range or sensor error).
 */
int getDistance() {
  // Send trigger pulse
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(TRIGGER_DELAY_LOW_US);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(TRIGGER_DELAY_HIGH_US);
  digitalWrite(TRIG_PIN, LOW);

  // Measure echo pulse duration
  long duration = pulseIn(ECHO_PIN, HIGH);

  // Calculate distance: (duration * sound_speed) / 2 (round trip)
  int distance = duration * SOUND_SPEED_CONVERSION / SOUND_TRAVEL_DIVISOR;

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
