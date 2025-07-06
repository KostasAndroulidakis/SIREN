/**
 * @file radar-math.ts
 * @brief Mathematical utilities for radar display calculations
 * @author unoRadar Project
 * @date 2025
 */

/**
 * Cartesian coordinates
 */
export interface CartesianPoint {
  x: number;
  y: number;
}

/**
 * Convert polar coordinates (angle, distance) to SVG cartesian coordinates
 * @param centerX Center X coordinate
 * @param centerY Center Y coordinate
 * @param radius Distance from center
 * @param angleInDegrees Angle in degrees (Arduino format: 15° = left, 165° = right)
 * @returns Cartesian coordinates
 */
export function polarToCartesian(
  centerX: number,
  centerY: number,
  radius: number,
  angleInDegrees: number
): CartesianPoint {
  // Arduino servo: 15° = left, 90° = center, 165° = right
  // Display should show: 0° = left, 90° = center, 180° = right
  // REVERSE the mapping: Arduino 15° = Display 180°, Arduino 165° = Display 0°
  const displayAngle = 180 - ((angleInDegrees - 15) / (165 - 15)) * 180;
  const radians = (displayAngle * Math.PI) / 180;

  return {
    x: centerX + radius * Math.cos(radians),
    y: centerY - radius * Math.sin(radians),
  };
}

/**
 * Calculate display radius based on distance and scale
 * @param distance Distance in centimeters
 * @param maxDistance Maximum distance for scaling
 * @param maxRadius Maximum display radius
 * @returns Display radius in pixels
 */
export function calculateDisplayRadius(
  distance: number,
  maxDistance: number,
  maxRadius: number
): number {
  return (Math.min(distance, maxDistance) / maxDistance) * maxRadius;
}

/**
 * Calculate point opacity based on age
 * @param timestamp Point timestamp
 * @param maxAge Maximum age in milliseconds
 * @param minOpacity Minimum opacity value
 * @returns Opacity value between minOpacity and 1
 */
export function calculatePointOpacity(
  timestamp: number,
  maxAge: number = 5000,
  minOpacity: number = 0.3
): number {
  const age = Date.now() - timestamp;
  return Math.max(minOpacity, 1 - (age / maxAge));
}

/**
 * Calculate intensity based on distance (closer objects are brighter)
 * @param distance Distance in centimeters
 * @param maxDistance Maximum distance
 * @param minIntensity Minimum intensity value
 * @returns Intensity value between minIntensity and 1
 */
export function calculateDistanceIntensity(
  distance: number,
  maxDistance: number,
  minIntensity: number = 0.3
): number {
  return Math.max(minIntensity, 1 - (distance / maxDistance));
}