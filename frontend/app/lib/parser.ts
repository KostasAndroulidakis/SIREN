/**
 * @file parser.ts
 * @brief Arduino radar data parsing module
 * @author unoRadar Project
 * @date 2025
 * 
 * Parses Arduino serial data in the format "Angle: X - Distance: Y"
 * and converts it to structured RadarDataPoint objects with validation.
 */

import type { RadarDataPoint, SweepDirection } from '~/types';

/**
 * Parsing result with success/error status
 */
export interface ParseResult {
  success: boolean;
  data?: RadarDataPoint;
  error?: string;
}

/**
 * Arduino radar data parser
 */
export class RadarDataParser {
  private lastAngle: number = 0;
  private sweepDirection: SweepDirection = 'forward';

  /**
   * Parse Arduino serial line into RadarDataPoint
   * @param rawData Raw serial line from Arduino (e.g., "Angle: 45 - Distance: 120")
   * @returns Parsing result with data or error
   */
  parseDataLine(rawData: string): ParseResult {
    try {
      // Validate input
      if (!rawData || typeof rawData !== 'string') {
        return { success: false, error: 'Invalid input data' };
      }

      const trimmedData = rawData.trim();
      if (trimmedData.length === 0) {
        return { success: false, error: 'Empty data line' };
      }

      // Parse using regex: "Angle: X - Distance: Y"
      const angleDistanceRegex = /^Angle:\s*(\d+)\s*-\s*Distance:\s*(\d+)$/i;
      const match = trimmedData.match(angleDistanceRegex);

      if (!match) {
        return { success: false, error: `Invalid format: ${trimmedData}` };
      }

      const angle = parseInt(match[1], 10);
      const distance = parseInt(match[2], 10);

      // Validate parsed values
      const validation = this.validateMeasurement(angle, distance);
      if (!validation.valid) {
        return { success: false, error: validation.error };
      }

      // Update sweep direction tracking
      this.updateSweepDirection(angle);

      const dataPoint: RadarDataPoint = {
        angle,
        distance,
        timestamp: Date.now(),
      };

      return { success: true, data: dataPoint };

    } catch (error) {
      return { 
        success: false, 
        error: `Parse error: ${error instanceof Error ? error.message : 'Unknown error'}` 
      };
    }
  }

  /**
   * Get current sweep direction
   */
  getSweepDirection(): SweepDirection {
    return this.sweepDirection;
  }

  /**
   * Reset parser state
   */
  reset(): void {
    this.lastAngle = 0;
    this.sweepDirection = 'forward';
  }

  /**
   * Validate measurement values
   */
  private validateMeasurement(angle: number, distance: number): { valid: boolean; error?: string } {
    // Validate angle range (Arduino servo limits: 15-165 degrees)
    if (isNaN(angle) || angle < 0 || angle > 180) {
      return { valid: false, error: `Invalid angle: ${angle} (must be 0-180)` };
    }

    // Validate distance range (HC-SR04 sensor limits: 2-400 cm)
    if (isNaN(distance) || distance < 0 || distance > 500) {
      return { valid: false, error: `Invalid distance: ${distance} (must be 0-500)` };
    }

    return { valid: true };
  }

  /**
   * Update sweep direction based on angle progression
   */
  private updateSweepDirection(currentAngle: number): void {
    if (this.lastAngle !== 0) {
      if (currentAngle > this.lastAngle) {
        this.sweepDirection = 'forward';
      } else if (currentAngle < this.lastAngle) {
        this.sweepDirection = 'backward';
      }
      // If angles are equal, keep current direction
    }
    this.lastAngle = currentAngle;
  }
}

/**
 * Utility function to parse single data line
 * @param rawData Raw Arduino serial line
 * @returns RadarDataPoint or null if parsing fails
 */
export function parseRadarData(rawData: string): RadarDataPoint | null {
  const parser = new RadarDataParser();
  const result = parser.parseDataLine(rawData);
  return result.success ? result.data! : null;
}

/**
 * Utility function to validate Arduino data format
 * @param rawData Raw Arduino serial line
 * @returns true if format is valid
 */
export function isValidRadarFormat(rawData: string): boolean {
  const angleDistanceRegex = /^Angle:\s*\d+\s*-\s*Distance:\s*\d+$/i;
  return angleDistanceRegex.test(rawData.trim());
}