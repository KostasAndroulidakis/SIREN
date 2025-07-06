/**
 * @file RadarDisplay.tsx
 * @brief Army-grade radar visualization component
 * @author unoRadar Project
 * @date 2025
 * 
 * Military-style radar display with professional dark theme,
 * clean typography, and minimal UI design.
 */

import type { RadarDataPoint, SweepDirection } from '~/types';
import { RADAR_DIMENSIONS, RADAR_GRID, RADAR_COLORS, RADAR_DISPLAY } from '../constants/radar';
import { 
  polarToCartesian, 
  calculateDisplayRadius, 
  calculateDistanceIntensity 
} from '../utils/radar-math';

/**
 * Radar display configuration
 */
export interface RadarDisplayProps {
  readonly dataPoints: RadarDataPoint[];
  readonly sweepDirection: SweepDirection;
  readonly currentAngle: number;
  readonly width?: number;
  readonly height?: number;
  readonly maxDistance?: number;
  readonly showGrid?: boolean;
  readonly showAngleLines?: boolean;
  readonly showSweepLine?: boolean;
}

/**
 * Radar grid component - handles distance rings
 */
function RadarGrid({ 
  centerX, 
  centerY, 
  maxRadius, 
  maxDistance 
}: {
  centerX: number;
  centerY: number;
  maxRadius: number;
  maxDistance: number;
}) {
  return (
    <g className="radar-grid">
      {RADAR_GRID.DISTANCE_RINGS.map((distance) => {
        const radius = calculateDisplayRadius(distance, maxDistance, maxRadius);
        return (
          <g key={distance}>
            <path
              d={`M ${centerX - radius} ${centerY} A ${radius} ${radius} 0 0 1 ${centerX + radius} ${centerY}`}
              fill="none"
              stroke={RADAR_COLORS.GRID_SECONDARY}
              strokeWidth={RADAR_DISPLAY.GRID_LINE_WIDTH}
              className="opacity-60"
            />
            <text
              x={centerX + radius - 10}
              y={centerY - 5}
              fill={RADAR_COLORS.TEXT_SECONDARY}
              fontSize="10"
              fontFamily="ui-monospace, monospace"
              className="select-none"
            >
              {distance}
            </text>
          </g>
        );
      })}
    </g>
  );
}

/**
 * Radar angle lines component
 */
function RadarAngleLines({
  centerX,
  centerY,
  maxRadius,
}: {
  centerX: number;
  centerY: number;
  maxRadius: number;
}) {
  return (
    <g className="radar-angles">
      {RADAR_GRID.ANGLE_LINES.map((angle) => {
        const { x, y } = polarToCartesian(centerX, centerY, maxRadius, angle);
        return (
          <g key={angle}>
            <line
              x1={centerX}
              y1={centerY}
              x2={x}
              y2={y}
              stroke={RADAR_COLORS.GRID_SECONDARY}
              strokeWidth={RADAR_DISPLAY.GRID_LINE_WIDTH}
              className="opacity-40"
            />
            <text
              x={x + (angle < 90 ? 5 : angle > 90 ? -15 : -5)}
              y={y - 5}
              fill={RADAR_COLORS.TEXT_SECONDARY}
              fontSize="9"
              fontFamily="ui-monospace, monospace"
              className="select-none"
            >
              {angle}°
            </text>
          </g>
        );
      })}
    </g>
  );
}

/**
 * Radar data points component
 */
function RadarDataPoints({
  dataPoints,
  centerX,
  centerY,
  maxRadius,
  maxDistance,
}: {
  dataPoints: RadarDataPoint[];
  centerX: number;
  centerY: number;
  maxRadius: number;
  maxDistance: number;
}) {
  return (
    <g className="radar-data">
      {dataPoints.map((point, index) => {
        const radius = calculateDisplayRadius(point.distance, maxDistance, maxRadius);
        const { x, y } = polarToCartesian(centerX, centerY, radius, point.angle);
        const intensity = calculateDistanceIntensity(point.distance, maxDistance);
        
        return (
          <circle
            key={`${point.timestamp}-${index}`}
            cx={x}
            cy={y}
            r={RADAR_DISPLAY.POINT_RADIUS}
            fill={RADAR_COLORS.DATA_POINT}
            className="animate-pulse"
            opacity={intensity}
          />
        );
      })}
    </g>
  );
}

/**
 * Radar sweep line component
 */
function RadarSweepLine({
  centerX,
  centerY,
  maxRadius,
  currentAngle,
}: {
  centerX: number;
  centerY: number;
  maxRadius: number;
  currentAngle: number;
}) {
  const { x, y } = polarToCartesian(centerX, centerY, maxRadius, currentAngle);
  
  return (
    <line
      x1={centerX}
      y1={centerY}
      x2={x}
      y2={y}
      stroke={RADAR_COLORS.SWEEP_LINE}
      strokeWidth={RADAR_DISPLAY.SWEEP_LINE_WIDTH}
      className="opacity-80 animate-pulse"
    />
  );
}

/**
 * Main radar display component
 */
export default function RadarDisplay({
  dataPoints = [],
  sweepDirection = 'forward',
  currentAngle = 0,
  width = RADAR_DIMENSIONS.DEFAULT_WIDTH,
  height = RADAR_DIMENSIONS.DEFAULT_HEIGHT,
  maxDistance = RADAR_GRID.MAX_DISTANCE_DEFAULT,
  showGrid = true,
  showAngleLines = true,
  showSweepLine = true,
}: RadarDisplayProps) {
  const centerX = width / 2;
  const centerY = height - RADAR_DIMENSIONS.HEADER_HEIGHT;
  const maxRadius = Math.min(width / 2, height - RADAR_DIMENSIONS.HEADER_HEIGHT) - RADAR_DIMENSIONS.MIN_PADDING;

  return (
    <div className="bg-black rounded-lg border border-green-900 p-4 font-mono">
      {/* Header */}
      <div className="text-green-400 mb-4 text-sm">
        <div className="flex justify-between items-center">
          <h3 className="text-lg font-bold tracking-wider">RADAR SCOPE</h3>
          <div className="flex gap-6 text-xs">
            <span>ANG: {currentAngle.toString().padStart(3, '0')}°</span>
            <span>DIR: {sweepDirection.toUpperCase()}</span>
            <span>TGT: {dataPoints.length.toString().padStart(3, '0')}</span>
          </div>
        </div>
      </div>

      {/* SVG Display */}
      <svg
        width={width}
        height={height}
        className="border border-green-900 rounded bg-black"
      >
        {/* Grid */}
        {showGrid && (
          <RadarGrid
            centerX={centerX}
            centerY={centerY}
            maxRadius={maxRadius}
            maxDistance={maxDistance}
          />
        )}

        {/* Angle Lines */}
        {showAngleLines && (
          <RadarAngleLines
            centerX={centerX}
            centerY={centerY}
            maxRadius={maxRadius}
          />
        )}

        {/* Data Points */}
        <RadarDataPoints
          dataPoints={dataPoints}
          centerX={centerX}
          centerY={centerY}
          maxRadius={maxRadius}
          maxDistance={maxDistance}
        />

        {/* Sweep Line */}
        {showSweepLine && (
          <RadarSweepLine
            centerX={centerX}
            centerY={centerY}
            maxRadius={maxRadius}
            currentAngle={currentAngle}
          />
        )}

        {/* Center Point */}
        <circle
          cx={centerX}
          cy={centerY}
          r={RADAR_DISPLAY.CENTER_POINT_RADIUS}
          fill={RADAR_COLORS.DATA_POINT}
          stroke={RADAR_COLORS.TEXT_PRIMARY}
          strokeWidth="1"
        />

        {/* Base Line */}
        <line
          x1={centerX - maxRadius}
          y1={centerY}
          x2={centerX + maxRadius}
          y2={centerY}
          stroke={RADAR_COLORS.GRID_PRIMARY}
          strokeWidth={RADAR_DISPLAY.BASE_LINE_WIDTH}
        />
      </svg>

      {/* Status Footer */}
      <div className="mt-4 text-xs text-green-400 flex justify-between">
        <div className="flex gap-4">
          <span>RNG: {maxDistance}CM</span>
          <span>SCL: {(maxRadius / maxDistance).toFixed(1)}PX/CM</span>
        </div>
        <div className="flex items-center gap-2">
          <div className="w-2 h-2 bg-green-400 rounded-full animate-pulse"></div>
          <span>ACTIVE</span>
        </div>
      </div>
    </div>
  );
}