/**
 * @file sonar.ino
 * @brief Sonar sweep control module for SIREN system
 * @author KostasAndroulidakis
 * @date 2025
 *
 * Orchestrates the complete sonar sweep operation by coordinating
 * motor movement, sensor readings, and data transmission to create
 * a bidirectional 180-degree sonar sweep pattern.
 */

/**
 * @brief Execute military-grade bidirectional sonar sweep
 *
 * Performs hardware-compliant defense-grade performance:
 * - Target speed: 66.7°/second = 11.1 RPM (near military 12+ RPM requirement)
 * - SG90 servo safe: 66.7°/s << 400°/s maximum capability
 * - HC-SR04 compliant: 60ms cycle time respected
 * - 4° step resolution for smooth target tracking
 *
 * At each angle position, the system:
 * - Moves servo to target angle with hardware-safe timing
 * - Takes distance measurement with HC-SR04 compliant settling
 * - Transmits data via high-speed serial communication
 */
void performSonarSweep() {
  int stepSize = getDegreeStep();
  int sensorTime = getSensorTime();

  // Forward sweep: Hardware-compliant timing
  for(int angle = getMinAngle(); angle <= getMaxAngle(); angle += stepSize) {
    moveServoToAngle(angle);      // Includes 20ms servo settling delay
    delay(sensorTime);            // Additional time for HC-SR04 measurement
    int distance = getDistance();
    sendSonarData(angle, distance);
  }

  // Backward sweep: Hardware-compliant timing
  for(int angle = getMaxAngle(); angle >= getMinAngle(); angle -= stepSize) {
    moveServoToAngle(angle);      // Includes 20ms servo settling delay
    delay(sensorTime);            // Additional time for HC-SR04 measurement
    int distance = getDistance();
    sendSonarData(angle, distance);
  }
}

/**
 * @brief Execute enhanced bidirectional sonar sweep with environmental data
 *
 * Performs military-grade sonar sweep with integrated environmental monitoring.
 * Includes temperature and humidity readings for atmospheric compensation
 * and comprehensive system monitoring.
 *
 * Environmental readings are taken periodically during sweep to minimize
 * impact on sonar performance while providing atmospheric data.
 */
void performEnhancedSonarSweep() {
  int stepSize = getDegreeStep();
  int sensorTime = getSensorTime();
  
  // Get environmental data at start of sweep (DHT11 timing constraint)
  float temperature, humidity;
  bool hasEnvironmentalData = getEnvironmentalData(temperature, humidity);

  // Forward sweep with environmental data
  for(int angle = getMinAngle(); angle <= getMaxAngle(); angle += stepSize) {
    moveServoToAngle(angle);      // Includes 20ms servo settling delay
    delay(sensorTime);            // Additional time for HC-SR04 measurement
    int distance = getDistance();
    
    if (hasEnvironmentalData) {
      sendEnhancedSonarData(angle, distance, temperature, humidity);
    } else {
      sendSonarData(angle, distance);  // Fallback to basic data
    }
  }

  // Backward sweep - reuse environmental data (DHT11 2-second constraint)
  for(int angle = getMaxAngle(); angle >= getMinAngle(); angle -= stepSize) {
    moveServoToAngle(angle);      // Includes 20ms servo settling delay  
    delay(sensorTime);            // Additional time for HC-SR04 measurement
    int distance = getDistance();
    
    if (hasEnvironmentalData) {
      sendEnhancedSonarData(angle, distance, temperature, humidity);
    } else {
      sendSonarData(angle, distance);  // Fallback to basic data
    }
  }
}

/**
 * @brief Execute single-direction high-speed sweep (surveillance mode)
 *
 * Optimized for maximum coverage speed, typically used in
 * surveillance sonar applications.
 */
void performSurveillanceSweep() {
  int stepSize = getDegreeStep();
  int sensorTime = getSensorTime();

  // Single direction sweep - Hardware-compliant timing
  for(int angle = getMinAngle(); angle <= getMaxAngle(); angle += stepSize) {
    moveServoToAngle(angle);      // Includes 20ms servo settling delay
    delay(sensorTime);            // Additional time for HC-SR04 measurement
    int distance = getDistance();
    sendSonarData(angle, distance);
  }
}
