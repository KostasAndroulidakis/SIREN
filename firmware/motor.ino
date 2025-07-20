/**
 * @file motor.ino
 * @brief Servo motor control module for SIREN system
 * @author SIREN Project
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

// Hardware-compliant timing (SG90 + HC-SR04 specifications)
const int SERVO_SETTLE_TIME = 20;    ///< SG90 minimum settle time (15-20ms recommended)
const int SENSOR_TIME = 40;          ///< HC-SR04 measurement time including safety margin
const int DEGREE_STEP = 2;           ///< Conservative 2° steps for reliable operation

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
 * @brief Move servo to specified angle with proper settling time
 * @param angle Target angle in degrees (should be between MIN_ANGLE and MAX_ANGLE)
 * 
 * Commands the servo to move and waits for proper settling time.
 * SG90 requires 15-20ms minimum between commands for reliable operation.
 */
void moveServoToAngle(int angle) {
  radarServo.write(angle);
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
