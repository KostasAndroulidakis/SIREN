/**
 * @file motor.ino
 * @brief Servo motor control module for SIREN system
 * @author KostasAndroulidakis
 * @date 2025
 *
 * Controls the SG90 micro servo motor that provides the rotating platform
 * for the ultrasonic sensor. Manages servo positioning, movement timing,
 * and operational angle limits for the sonar sweep functionality.
 */

#include <Servo.h>

// Motor hardware and configuration
Servo sonarServo;                    ///< Servo motor object for sonar platform
constexpr int SERVO_PIN = 9;             ///< PWM pin connected to servo control signal
constexpr int MIN_ANGLE = 15;            ///< Minimum sweep angle in degrees
constexpr int MAX_ANGLE = 165;           ///< Maximum sweep angle in degrees

// SG90 pulse width specifications (from datasheet)
constexpr int SG90_MIN_PULSE_WIDTH = 1000;  ///< SG90 minimum pulse width (1ms = 0°)
constexpr int SG90_MAX_PULSE_WIDTH = 2000;  ///< SG90 maximum pulse width (2ms = 180°)

// Hardware-compliant timing (SG90 + HC-SR04 specifications)
constexpr int SERVO_SETTLE_TIME = 20;    ///< SG90 minimum settle time (15-20ms recommended)
constexpr int SENSOR_TIME = 40;          ///< HC-SR04 measurement time including safety margin
constexpr int DEGREE_STEP = 2;           ///< Conservative 2° steps for reliable operation

/**
 * @brief Initialize the servo motor
 *
 * Attaches the servo motor to its control pin with SG90-specific pulse width calibration.
 * Uses datasheet-compliant 1000-2000µs range instead of Arduino library defaults.
 * Must be called before any servo movement commands.
 */
void initMotor() {
  sonarServo.attach(SERVO_PIN, SG90_MIN_PULSE_WIDTH, SG90_MAX_PULSE_WIDTH);
}

/**
 * @brief Move servo to specified angle with proper settling time
 * @param angle Target angle in degrees (must be between MIN_ANGLE and MAX_ANGLE)
 *
 * Commands the servo to move and waits for proper settling time.
 * SG90 requires 15-20ms minimum between commands for reliable operation.
 * MISRA C++ compliant: Input validation prevents hardware damage.
 */
void moveServoToAngle(int angle) {
  // MISRA C++ Rule 18.1.1: Validate input parameters to prevent hardware damage
  if (angle < MIN_ANGLE || angle > MAX_ANGLE) {
    return;  // Reject invalid angles to protect servo hardware
  }
  
  sonarServo.write(angle);
  delay(SERVO_SETTLE_TIME);  // SG90 hardware requirement: 15-20ms settling
}

/**
 * @brief Get optimized degree step size for fast sweep
 * @return Degree step size for military-grade performance
 */
int getDegreeStep() {
  return DEGREE_STEP;
}

/**
 * @brief Get sensor measurement time for HC-SR04
 * @return Sensor measurement time in milliseconds
 */
int getSensorTime() {
  return SENSOR_TIME;
}

/**
 * @brief Get minimum operational angle
 * @return Minimum angle in degrees for sonar sweep
 *
 * Returns the lower limit of the servo's operational range.
 * This prevents mechanical damage and ensures reliable operation.
 */
int getMinAngle() {
  return MIN_ANGLE;
}

/**
 * @brief Get maximum operational angle
 * @return Maximum angle in degrees for sonar sweep
 *
 * Returns the upper limit of the servo's operational range.
 * This prevents mechanical damage and ensures reliable operation.
 */
int getMaxAngle() {
  return MAX_ANGLE;
}
