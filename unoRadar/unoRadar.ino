/**
 * @file unoRadar.ino
 * @brief Main Arduino sketch for unoRadar system
 * @author unoRadar Project
 * @date 2025
 * 
 * This is the main orchestration file for the unoRadar system that combines
 * ultrasonic sensor readings with servo motor control to create a 180-degree
 * radar sweep system with web visualization capabilities.
 */

// Military radar performance configuration (Hardware compliant)
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
void performRadarSweep();                   ///< Execute complete radar sweep (radar.ino)
void performSurveillanceSweep();            ///< Execute single-direction surveillance sweep (radar.ino)
void sendRadarData(int angle, int distance); ///< Send data via serial (communication.ino)
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
 * Continuously performs radar sweeps with configured delay between cycles.
 * This is the core operational loop of the radar system.
 */
void loop() {
  performRadarSweep();
  delay(SWEEP_DELAY_MS);
}
