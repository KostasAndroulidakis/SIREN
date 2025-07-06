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
  int settleTime = getSensorSettleTime();
  
  // Forward sweep: military-grade speed optimization
  for(int angle = getMinAngle(); angle <= getMaxAngle(); angle += stepSize) {
    moveServoToAngle(angle);
    delay(settleTime); // Minimal sensor settling for accuracy
    int distance = getDistance();
    sendRadarData(angle, distance);
  }
  
  // Backward sweep: maintain consistent military timing
  for(int angle = getMaxAngle(); angle >= getMinAngle(); angle -= stepSize) {
    moveServoToAngle(angle);
    delay(settleTime); // Minimal sensor settling for accuracy
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
  int settleTime = getSensorSettleTime();
  
  // Single direction sweep for maximum speed
  for(int angle = getMinAngle(); angle <= getMaxAngle(); angle += stepSize) {
    moveServoToAngle(angle);
    delay(settleTime);
    int distance = getDistance();
    sendRadarData(angle, distance);
  }
}
