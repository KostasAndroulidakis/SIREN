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
const int SERVO_DELAY = 200;         ///< Delay in milliseconds after servo movement

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
 * @brief Move servo to specified angle
 * @param angle Target angle in degrees (should be between MIN_ANGLE and MAX_ANGLE)
 * 
 * Commands the servo to move to the specified angle and waits for the movement
 * to complete. The delay ensures mechanical stability before the next operation.
 */
void moveServoToAngle(int angle) {
  radarServo.write(angle);
  delay(SERVO_DELAY);
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
