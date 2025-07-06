/**
 * @file data-buffer.ts
 * @brief Radar data buffer for managing streaming data
 * @author unoRadar Project
 * @date 2025
 *
 * Manages a circular buffer of radar data points for efficient storage
 * and retrieval of streaming Arduino radar measurements.
 */

import type { RadarDataPoint, SweepDirection } from '~/types';

/**
 * Buffer statistics
 */
export interface BufferStats {
  totalPoints: number;
  bufferSize: number;
  lastUpdate: number;
  sweepDirection: SweepDirection;
  currentAngle: number;
}

/**
 * Circular buffer for radar data points
 */
export class RadarDataBuffer {
  private buffer: RadarDataPoint[] = [];
  private maxSize: number;
  private writeIndex: number = 0;
  private currentSweepDirection: SweepDirection = 'forward';

  constructor(maxSize: number = 1000) {
    this.maxSize = maxSize;
  }

  /**
   * Add new data point to buffer
   * @param dataPoint Radar measurement to add
   */
  addPoint(dataPoint: RadarDataPoint): void {
    // Update sweep direction tracking
    this.updateSweepDirection(dataPoint.angle);

    if (this.buffer.length < this.maxSize) {
      // Buffer not full, just push
      this.buffer.push(dataPoint);
    } else {
      // Circular buffer: overwrite oldest data
      this.buffer[this.writeIndex] = dataPoint;
      this.writeIndex = (this.writeIndex + 1) % this.maxSize;
    }
  }

  /**
   * Get latest N data points
   * @param count Number of recent points to retrieve
   * @returns Array of recent radar data points
   */
  getLatestPoints(count: number = 100): RadarDataPoint[] {
    const total = this.buffer.length;
    if (total === 0) return [];

    const startIndex = Math.max(0, total - count);
    return this.buffer.slice(startIndex).sort((a, b) => a.timestamp - b.timestamp);
  }

  /**
   * Get all data points for current sweep
   * @returns Array of data points from current sweep direction
   */
  getCurrentSweep(): RadarDataPoint[] {
    if (this.buffer.length === 0) return [];

    const sorted = [...this.buffer].sort((a, b) => a.timestamp - b.timestamp);
    const result: RadarDataPoint[] = [];

    // Find the most recent direction change or start of current sweep
    const currentDirection = this.currentSweepDirection;
    for (let i = sorted.length - 1; i >= 0; i--) {
      const point = sorted[i];
      const prevPoint = sorted[i - 1];

      if (prevPoint) {
        const direction: SweepDirection = point.angle > prevPoint.angle ? 'forward' : 'backward';
        if (direction !== currentDirection) {
          break; // Found direction change
        }
      }

      result.unshift(point);
    }

    return result;
  }

  /**
   * Get data points within angle range
   * @param minAngle Minimum angle (inclusive)
   * @param maxAngle Maximum angle (inclusive)
   * @returns Array of data points within angle range
   */
  getPointsInRange(minAngle: number, maxAngle: number): RadarDataPoint[] {
    return this.buffer.filter(point =>
      point.angle >= minAngle && point.angle <= maxAngle
    );
  }

  /**
   * Clear all data from buffer
   */
  clear(): void {
    this.buffer = [];
    this.writeIndex = 0;
    this.currentSweepDirection = 'forward';
  }

  /**
   * Get buffer statistics
   */
  getStats(): BufferStats {
    const latestPoint = this.buffer[this.buffer.length - 1];

    return {
      totalPoints: this.buffer.length,
      bufferSize: this.maxSize,
      lastUpdate: latestPoint?.timestamp || 0,
      sweepDirection: this.currentSweepDirection,
      currentAngle: latestPoint?.angle || 0,
    };
  }

  /**
   * Get buffer capacity utilization (0-1)
   */
  getUtilization(): number {
    return this.buffer.length / this.maxSize;
  }

  /**
   * Update sweep direction based on angle progression
   */
  private updateSweepDirection(currentAngle: number): void {
    if (this.buffer.length > 0) {
      const lastPoint = this.buffer[this.buffer.length - 1];
      if (currentAngle > lastPoint.angle) {
        this.currentSweepDirection = 'forward';
      } else if (currentAngle < lastPoint.angle) {
        this.currentSweepDirection = 'backward';
      }
    }
  }
}

/**
 * Global radar data buffer instance
 */
export const globalRadarBuffer = new RadarDataBuffer(2000); // Store last 2000 points