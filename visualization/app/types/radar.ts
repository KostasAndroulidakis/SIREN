/**
 * @file radar.ts
 * @brief Type definitions for unoRadar system
 * @author unoRadar Project
 * @date 2025
 */

/**
 * Raw radar data point from Arduino
 */
export interface RadarDataPoint {
  angle: number;    // Servo angle in degrees (15-165)
  distance: number; // Distance measurement in centimeters
  timestamp: number; // Unix timestamp when data was received
}

/**
 * Serial port connection status
 */
export type SerialStatus = 'disconnected' | 'connecting' | 'connected' | 'error';

/**
 * Serial port configuration
 */
export interface SerialConfig {
  path: string;      // Serial port path (e.g., '/dev/ttyUSB0')
  baudRate: number;  // Baud rate (should match Arduino: 9600)
}

/**
 * Radar sweep direction
 */
export type SweepDirection = 'forward' | 'backward';

/**
 * Radar system state
 */
export interface RadarState {
  isActive: boolean;
  currentAngle: number;
  sweepDirection: SweepDirection;
  lastUpdate: number;
}