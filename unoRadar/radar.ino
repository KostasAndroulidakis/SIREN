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
 * @brief Execute complete bidirectional radar sweep
 * 
 * Performs a full radar sweep cycle consisting of:
 * 1. Forward sweep: minimum angle to maximum angle
 * 2. Backward sweep: maximum angle to minimum angle
 * 
 * At each angle position, the system:
 * - Moves servo to target angle
 * - Takes distance measurement
 * - Transmits data via serial communication
 */
void performRadarSweep() {
  // Forward sweep: scan from minimum to maximum angle
  for(int angle = getMinAngle(); angle <= getMaxAngle(); angle++) {
    moveServoToAngle(angle);
    int distance = getDistance();
    sendRadarData(angle, distance);
  }
  
  // Backward sweep: scan from maximum to minimum angle
  for(int angle = getMaxAngle(); angle >= getMinAngle(); angle--) {
    moveServoToAngle(angle);
    int distance = getDistance();
    sendRadarData(angle, distance);
  }
}
