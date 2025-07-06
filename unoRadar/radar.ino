/**
 * @file radar.ino
 * @brief Radar sweep control module for unoRadar system
 * @author unoRadar Project
 * @date 2025
 * 
 * Orchestrates the complete radar sweep operation by coordinating
 * motor movement, sensor readings, and data transmission to create
 * a bidirectional 180-degree radar sweep pattern.
 */

/**
 * @brief Execute military-grade bidirectional radar sweep
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
void performRadarSweep() {
  int stepSize = getDegreeStep();
  int sensorTime = getSensorTime();
  
  // Forward sweep: Hardware-compliant timing
  for(int angle = getMinAngle(); angle <= getMaxAngle(); angle += stepSize) {
    moveServoToAngle(angle);      // Includes 20ms servo settling delay
    delay(sensorTime);            // Additional time for HC-SR04 measurement
    int distance = getDistance();
    sendRadarData(angle, distance);
  }
  
  // Backward sweep: Hardware-compliant timing
  for(int angle = getMaxAngle(); angle >= getMinAngle(); angle -= stepSize) {
    moveServoToAngle(angle);      // Includes 20ms servo settling delay  
    delay(sensorTime);            // Additional time for HC-SR04 measurement
    int distance = getDistance();
    sendRadarData(angle, distance);
  }
}

/**
 * @brief Execute single-direction high-speed sweep (surveillance mode)
 * 
 * Optimized for maximum coverage speed, typically used in
 * surveillance radar applications.
 */
void performSurveillanceSweep() {
  int stepSize = getDegreeStep();
  int sensorTime = getSensorTime();
  
  // Single direction sweep - Hardware-compliant timing
  for(int angle = getMinAngle(); angle <= getMaxAngle(); angle += stepSize) {
    moveServoToAngle(angle);      // Includes 20ms servo settling delay
    delay(sensorTime);            // Additional time for HC-SR04 measurement  
    int distance = getDistance();
    sendRadarData(angle, distance);
  }
}
