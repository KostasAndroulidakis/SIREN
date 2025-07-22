/**
 * @file motor.ino
 * @brief Servo motor control module for SIREN system
 * @author KostasAndroulidakis
 * @date 2025
 *
 * Controls the SG90 micro servo motor that provides the rotating platform
 * for the ultrasonic sensor. Manages servo positioning, movement timing,
 * and operational angle limits for the sonar sweep functionality.
 *
 * POWER REQUIREMENTS (SG90 Servo Motor):
 * - Operating Voltage: 4.8V - 6.0V DC (5V nominal)
 * - Current Draw (Idle): ~10mA
 * - Current Draw (Moving): 100-250mA typical
 * - Current Draw (Stall): Up to 360mA maximum
 * - Torque: 2.5 kg·cm at 4.8V
 *
 * ARDUINO POWER COMPATIBILITY WARNING:
 * - Arduino I/O pins: 40mA maximum per pin (INSUFFICIENT for servo power)
 * - Servo MUST be powered from external 5V supply or Arduino 5V rail
 * - Signal wire connects to Arduino PWM pin (low current)
 * - Ground must be common between Arduino and servo power supply
 *
 * RECOMMENDED POWER SETUP:
 * - Servo VCC (Red): Arduino 5V pin or external 5V supply (>500mA capacity)
 * - Servo GND (Black/Brown): Arduino GND (common ground required)
 * - Servo Signal (Orange/Yellow): Arduino PWM Pin 9 (40mA signal, safe)
 */

#include <Servo.h>

// Motor hardware and configuration
Servo sonarServo;                    ///< Servo motor object for sonar platform
constexpr int SERVO_PIN = 9;             ///< PWM pin connected to servo control signal
constexpr int MIN_ANGLE = 5;             ///< Minimum sweep angle (5° safety margin from 0°)
constexpr int MAX_ANGLE = 175;           ///< Maximum sweep angle (5° safety margin from 180°)

// Current servo position tracking
static int currentServoPosition = 90;  ///< Track current servo angle for dynamic timing

// SG90 pulse width specifications (from datasheet)
constexpr int SG90_MIN_PULSE_WIDTH = 1000;  ///< SG90 minimum pulse width (1ms = 0°)
constexpr int SG90_MAX_PULSE_WIDTH = 2000;  ///< SG90 maximum pulse width (2ms = 180°)

// SG90 power specifications (from datasheet)
constexpr float SG90_OPERATING_VOLTAGE_MIN = 4.8f;  ///< Minimum operating voltage (V)
constexpr float SG90_OPERATING_VOLTAGE_MAX = 6.0f;  ///< Maximum operating voltage (V)
constexpr int SG90_CURRENT_IDLE_MA = 10;            ///< Idle current draw (mA)
constexpr int SG90_CURRENT_MOVING_MA = 200;         ///< Typical moving current (mA)
constexpr int SG90_CURRENT_STALL_MA = 360;          ///< Maximum stall current (mA)
constexpr int ARDUINO_PIN_MAX_CURRENT_MA = 40;      ///< Arduino I/O pin current limit (mA)

// SG90 timing specifications (from datasheet)
constexpr float SG90_SPEED_DEG_PER_MS = 0.6f;  ///< SG90 speed: 0.1s/60° = 0.6°/ms
constexpr int MIN_SETTLE_TIME = 20;            ///< Minimum settle time regardless of movement
constexpr int MAX_SETTLE_TIME = 200;           ///< Maximum settle time cap for safety

// Hardware-compliant timing (HC-SR04 specifications)
constexpr int SENSOR_TIME = 40;          ///< HC-SR04 measurement time including safety margin
constexpr int DEGREE_STEP = 2;           ///< Conservative 2° steps for reliable operation

/**
 * @brief Calculate dynamic settling time based on angle movement
 * @param targetAngle Target angle to move to
 * @return Settling time in milliseconds based on SG90 datasheet specs
 *
 * SG90 specification: 0.1 seconds per 60° (0.6°/ms)
 * Calculates time needed based on actual angle movement distance.
 */
int calculateSettlingTime(int targetAngle) {
  int angleDifference = abs(targetAngle - currentServoPosition);
  int settlingTime = static_cast<int>(angleDifference / SG90_SPEED_DEG_PER_MS);

  // Ensure settling time is within safe bounds
  if (settlingTime < MIN_SETTLE_TIME) {
    settlingTime = MIN_SETTLE_TIME;
  } else if (settlingTime > MAX_SETTLE_TIME) {
    settlingTime = MAX_SETTLE_TIME;
  }

  return settlingTime;
}

/**
 * @brief Display power supply warnings and requirements
 *
 * CRITICAL: Outputs power supply warnings to serial console.
 * SG90 servo requires 100-360mA which exceeds Arduino I/O pin limits (40mA).
 * Must be powered from Arduino 5V rail or external supply.
 */
void displayPowerRequirements() {
  Serial.println("SG90 SERVO POWER REQUIREMENTS:");
  Serial.print("  Operating Voltage: ");
  Serial.print(SG90_OPERATING_VOLTAGE_MIN, 1);
  Serial.print("V - ");
  Serial.print(SG90_OPERATING_VOLTAGE_MAX, 1);
  Serial.println("V");
  Serial.print("  Current Draw (Moving): ");
  Serial.print(SG90_CURRENT_MOVING_MA);
  Serial.println("mA typical");
  Serial.print("  Current Draw (Stall): ");
  Serial.print(SG90_CURRENT_STALL_MA);
  Serial.println("mA maximum");
  Serial.println();
  Serial.println("CRITICAL POWER WARNING:");
  Serial.print("  Arduino I/O Pin Limit: ");
  Serial.print(ARDUINO_PIN_MAX_CURRENT_MA);
  Serial.println("mA (INSUFFICIENT for servo power)");
  Serial.println("  SERVO MUST BE POWERED FROM ARDUINO 5V RAIL OR EXTERNAL SUPPLY");
  Serial.println("  Signal wire only connects to PWM pin");
  Serial.println();
}

/**
 * @brief Initialize the servo motor
 *
 * Attaches the servo motor to its control pin with SG90-specific pulse width calibration.
 * Uses datasheet-compliant 1000-2000µs range instead of Arduino library defaults.
 * Displays power supply requirements and warnings.
 * Must be called before any servo movement commands.
 */
void initMotor() {
  displayPowerRequirements();
  sonarServo.attach(SERVO_PIN, SG90_MIN_PULSE_WIDTH, SG90_MAX_PULSE_WIDTH);
  currentServoPosition = 90;  // Initialize to center position
}

/**
 * @brief Move servo to specified angle with dynamic settling time
 * @param angle Target angle in degrees (must be between MIN_ANGLE and MAX_ANGLE)
 *
 * Commands the servo to move and waits for appropriate settling time based on movement distance.
 * Uses SG90 datasheet specification: 0.1 seconds per 60° for accurate timing.
 * MISRA C++ compliant: Input validation prevents hardware damage.
 */
void moveServoToAngle(int angle) {
  // MISRA C++ Rule 18.1.1: Validate input parameters to prevent hardware damage
  if (angle < MIN_ANGLE || angle > MAX_ANGLE) {
    return;  // Reject invalid angles to protect servo hardware
  }

  // Calculate dynamic settling time based on movement distance
  int settlingTime = calculateSettlingTime(angle);

  sonarServo.write(angle);
  currentServoPosition = angle;  // Update position tracker
  delay(settlingTime);  // SG90 datasheet-compliant dynamic settling time
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
 * Returns the lower limit of the servo's operational range (5°).
 * Provides 5° safety margin from SG90's absolute minimum (0°)
 * for reliable operation while maximizing sonar coverage.
 */
int getMinAngle() {
  return MIN_ANGLE;
}

/**
 * @brief Get maximum operational angle
 * @return Maximum angle in degrees for sonar sweep
 *
 * Returns the upper limit of the servo's operational range (175°).
 * Provides 5° safety margin from SG90's absolute maximum (180°)
 * for reliable operation while maximizing sonar coverage.
 */
int getMaxAngle() {
  return MAX_ANGLE;
}
