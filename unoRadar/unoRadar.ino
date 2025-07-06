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

// Military radar performance configuration
const int SWEEP_DELAY_MS = 100;     ///< Optimized delay between radar sweep cycles for military speed
const bool PERFORMANCE_MODE = true; ///< Enable military-grade performance optimizations

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
int getSensorMeasurementTime();             ///< Get HC-SR04 measurement time (motor.ino)

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
