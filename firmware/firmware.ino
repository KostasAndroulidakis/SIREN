/**
 * @file SIREN.ino
 * @brief Main Arduino sketch for SIREN system
 * @author KostasAndroulidakis
 * @date 2025
 *
 * This is the main orchestration file for the SIREN system that combines
 * ultrasonic sensor readings with servo motor control to create a 180-degree
 * sonar sweep system with web visualization capabilities.
 */

// Military sonar performance configuration (Hardware compliant)
const int SWEEP_DELAY_MS = 50;      ///< Minimal delay between cycles for continuous military operation
const bool PERFORMANCE_MODE = true; ///< Enable military-grade performance optimizations
const bool HARDWARE_SAFE_MODE = true; ///< Ensure all operations respect SG90 and HC-SR04 limits

// Function declarations - implemented in separate module files
void initMotor();                           ///< Initialize servo motor (motor.ino)
void initSensor();                          ///< Initialize ultrasonic sensor (sensor.ino)
void initCommunication();                   ///< Initialize serial communication (communication.ino)
void moveServoToAngle(int angle);           ///< Move servo to specified angle (motor.ino)
int getDistance();                          ///< Get distance measurement (sensor.ino)
int getMinAngle();                          ///< Get minimum sweep angle (motor.ino)
int getMaxAngle();                          ///< Get maximum sweep angle (motor.ino)
void performSonarSweep();                   ///< Execute complete sonar sweep (sonar.ino)
void performSurveillanceSweep();            ///< Execute single-direction surveillance sweep (sonar.ino)
void sendSonarData(int angle, int distance); ///< Send data via serial (communication.ino)
int getDegreeStep();                        ///< Get optimized degree step size (motor.ino)
int getSensorTime();                        ///< Get HC-SR04 sensor measurement time (motor.ino)

/**
 * @brief Initialize all system components
 *
 * Sets up communication, motor control, and sensor components.
 * Called once at system startup.
 */
void setup() {
  initCommunication();
  initMotor();
  initSensor();
}

/**
 * @brief Main execution loop
 *
 * Continuously performs sonar sweeps with configured delay between cycles.
 * This is the core operational loop of the sonar system.
 */
void loop() {
  performSonarSweep();
  delay(SWEEP_DELAY_MS);
}
