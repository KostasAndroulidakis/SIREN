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
constexpr int SWEEP_DELAY_MS = 50;      ///< Minimal delay between cycles for continuous military operation

// Function declarations - implemented in separate module files
void initMotor();                           ///< Initialize servo motor (motor.ino)
void initSensor();                          ///< Initialize ultrasonic sensor (sensor.ino)
void initEnvironment();                     ///< Initialize environmental sensor (environment.ino)
void initCommunication();                   ///< Initialize serial communication (communication.ino)
void moveServoToAngle(int angle);           ///< Move servo to specified angle (motor.ino)
int getDistance();                          ///< Get distance measurement with default sound speed (sensor.ino)
int getCalibratedDistance(float soundSpeed); ///< Get distance with real-time sound speed (sensor.ino)
float getTemperature();                     ///< Get temperature reading (environment.ino)
float getHumidity();                        ///< Get humidity reading (environment.ino)
bool getEnvironmentalData(float &temp, float &hum); ///< Get both environmental readings (environment.ino)
float calculateAtmosphericSoundSpeed(float temp, float hum); ///< Calculate real-time sound speed (physics.ino)
float getDefaultAtmosphericSoundSpeed();    ///< Get default sound speed fallback (physics.ino)
float calculateSoundSpeedFromTemperature(float temp); ///< Calculate sound speed from temperature only (physics.ino)
bool isValidEnvironmentalData(float temp, float hum); ///< Validate environmental data (physics.ino)
int getMinAngle();                          ///< Get minimum sweep angle (motor.ino)
int getMaxAngle();                          ///< Get maximum sweep angle (motor.ino)
void performSonarSweep();                   ///< Execute complete sonar sweep (sonar.ino)
void performEnhancedSonarSweep();           ///< Execute sonar sweep with environmental data (sonar.ino)
void performSurveillanceSweep();            ///< Execute single-direction surveillance sweep (sonar.ino)
void sendSonarData(int angle, int distance); ///< Send data via serial (communication.ino)
void sendEnhancedSonarData(int angle, int distance, float temp, float hum); ///< Send enhanced data (communication.ino)
void sendCalibratedSonarData(int angle, int distance, float temp, float hum, float soundSpeed); ///< Send calibrated data (communication.ino)
void sendEnvironmentalData(float temp, float hum); ///< Send environmental data (communication.ino)
int getDegreeStep();                        ///< Get optimized degree step size (motor.ino)
int getSensorTime();                        ///< Get HC-SR04 sensor measurement time (motor.ino)

/**
 * @brief Initialize all system components
 *
 * Sets up communication, motor control, distance sensor, and environmental sensor.
 * Called once at system startup.
 */
void setup() {
  initCommunication();
  initMotor();
  initSensor();
  initEnvironment();
}

/**
 * @brief Main execution loop
 *
 * Continuously performs enhanced sonar sweeps with environmental monitoring.
 * Uses enhanced mode for comprehensive environmental data collection.
 */
void loop() {
  performEnhancedSonarSweep();
  delay(SWEEP_DELAY_MS);
}
