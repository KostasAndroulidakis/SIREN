/**
 * @file radar.ts
 * @brief Radar display constants and configuration
 * @author unoRadar Project
 * @date 2025
 */

/**
 * Radar display dimensions
 */
export const RADAR_DIMENSIONS = {
  DEFAULT_WIDTH: 800,
  DEFAULT_HEIGHT: 450,
  MIN_PADDING: 40,
  HEADER_HEIGHT: 60,
} as const;

/**
 * Radar grid configuration
 */
export const RADAR_GRID = {
  DISTANCE_RINGS: [100, 200, 300, 400] as const, // HC-SR04 realistic ranges (2cm-400cm max)
  ANGLE_LINES: [0, 15, 30, 45, 60, 75, 90, 105, 120, 135, 150, 165, 180] as const,
  MAX_DISTANCE_DEFAULT: 400, // HC-SR04 maximum detection range
} as const;

/**
 * Radar colors (army-grade green theme)
 */
export const RADAR_COLORS = {
  BACKGROUND: '#000000',
  BACKGROUND_SECONDARY: '#0a0f0a',
  GRID_PRIMARY: '#1a3d1a',
  GRID_SECONDARY: '#0f2e0f',
  SWEEP_LINE: '#39ff14',
  DATA_POINT: '#39ff14',
  TEXT_PRIMARY: '#39ff14',
  TEXT_SECONDARY: '#2eb82e',
  BORDER: '#1a3d1a',
  STATUS_ACTIVE: '#39ff14',
} as const;

/**
 * Animation timings
 */
export const RADAR_ANIMATIONS = {
  SWEEP_PULSE_DURATION: '1s',
  POINT_FADE_DURATION: '5s',
  STATUS_BLINK_DURATION: '2s',
} as const;

/**
 * Data visualization settings
 */
export const RADAR_DISPLAY = {
  POINT_RADIUS: 2,
  SWEEP_LINE_WIDTH: 2,
  GRID_LINE_WIDTH: 1,
  CENTER_POINT_RADIUS: 3,
  BASE_LINE_WIDTH: 2,
  MIN_POINT_OPACITY: 0.3,
} as const;