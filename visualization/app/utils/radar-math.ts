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
 * @param angleInDegrees Angle in degrees (Arduino format: 0° = up, clockwise)
 * @returns Cartesian coordinates
 */
export function polarToCartesian(
  centerX: number,
  centerY: number,
  radius: number,
  angleInDegrees: number
): CartesianPoint {
  // Convert Arduino angle (0° = up, clockwise) to standard math coordinates
  const adjustedAngle = (90 - angleInDegrees) * (Math.PI / 180);
  
  return {
    x: centerX + radius * Math.cos(adjustedAngle),
    y: centerY - radius * Math.sin(adjustedAngle),
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