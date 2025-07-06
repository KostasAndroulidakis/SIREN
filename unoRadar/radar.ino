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
 * Performs optimized radar sweep cycle for defense-grade performance:
 * - Target speed: 50°/second (realistic military radar)
 * - Optimized step size for smooth tracking
 * - Minimal delays while maintaining sensor accuracy
 * 
 * At each angle position, the system:
 * - Moves servo to target angle with optimized timing
 * - Takes distance measurement with minimal settling
 * - Transmits data via serial communication
 */
void performRadarSweep() {
  int stepSize = getDegreeStep();
  int measurementTime = getSensorMeasurementTime();
  
  // Forward sweep: HC-SR04 compliant military-grade performance
  for(int angle = getMinAngle(); angle <= getMaxAngle(); angle += stepSize) {
    moveServoToAngle(angle);
    delay(measurementTime); // HC-SR04 compliant timing (60ms cycle requirement)
    int distance = getDistance();
    sendRadarData(angle, distance);
  }
  
  // Backward sweep: maintain HC-SR04 compliant timing
  for(int angle = getMaxAngle(); angle >= getMinAngle(); angle -= stepSize) {
    moveServoToAngle(angle);
    delay(measurementTime); // HC-SR04 compliant timing (60ms cycle requirement)
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
  int measurementTime = getSensorMeasurementTime();
  
  // Single direction sweep with HC-SR04 compliant timing
  for(int angle = getMinAngle(); angle <= getMaxAngle(); angle += stepSize) {
    moveServoToAngle(angle);
    delay(measurementTime); // HC-SR04 compliant 60ms cycle
    int distance = getDistance();
    sendRadarData(angle, distance);
  }
}
