/**
 * @file motor.ino
 * @brief Servo motor control module for unoRadar system
 * @author unoRadar Project
 * @date 2025
 * 
 * Controls the SG90 micro servo motor that provides the rotating platform
 * for the ultrasonic sensor. Manages servo positioning, movement timing,
 * and operational angle limits for the radar sweep functionality.
 */

#include <Servo.h>

// Motor hardware and configuration
Servo radarServo;                    ///< Servo motor object for radar platform
const int SERVO_PIN = 9;             ///< PWM pin connected to servo control signal
const int MIN_ANGLE = 15;            ///< Minimum sweep angle in degrees
const int MAX_ANGLE = 165;           ///< Maximum sweep angle in degrees

// Military radar performance optimization (HC-SR04 compliant)
const int SERVO_STEP_DELAY = 25;     ///< Servo movement time for smooth operation
const int SENSOR_MEASUREMENT_TIME = 35; ///< HC-SR04 measurement + safety margin (60ms cycle requirement)
const int DEGREE_STEP = 3;           ///< Optimized step size for 50°/second with HC-SR04 constraints

/**
 * @brief Initialize the servo motor
 * 
 * Attaches the servo motor to its control pin and prepares it for operation.
 * Must be called before any servo movement commands.
 */
void initMotor() {
  radarServo.attach(SERVO_PIN);
}

/**
 * @brief Move servo to specified angle with military-grade timing
 * @param angle Target angle in degrees (should be between MIN_ANGLE and MAX_ANGLE)
 * 
 * Commands the servo to move to the specified angle with optimized timing
 * for realistic military radar sweep speeds (30-60°/second).
 */
void moveServoToAngle(int angle) {
  radarServo.write(angle);
  delay(SERVO_STEP_DELAY);  // Optimized for 50°/second military speed
}

/**
 * @brief Get optimized degree step size for fast sweep
 * @return Degree step size for military-grade performance
 */
int getDegreeStep() {
  return DEGREE_STEP;
}

/**
 * @brief Get sensor measurement time (HC-SR04 compliant)
 * @return HC-SR04 measurement time including safety margin
 */
int getSensorMeasurementTime() {
  return SENSOR_MEASUREMENT_TIME;
}

/**
 * @brief Get minimum operational angle
 * @return Minimum angle in degrees for radar sweep
 * 
 * Returns the lower limit of the servo's operational range.
 * This prevents mechanical damage and ensures reliable operation.
 */
int getMinAngle() {
  return MIN_ANGLE;
}

/**
 * @brief Get maximum operational angle
 * @return Maximum angle in degrees for radar sweep
 * 
 * Returns the upper limit of the servo's operational range.
 * This prevents mechanical damage and ensures reliable operation.
 */
int getMaxAngle() {
  return MAX_ANGLE;
}
