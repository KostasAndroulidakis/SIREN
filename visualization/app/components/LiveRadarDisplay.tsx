/**
 * @file LiveRadarDisplay.tsx
 * @brief Live radar display with real-time data integration
 * @author unoRadar Project
 * @date 2025
 * 
 * Integrates the RadarDisplay component with real-time data parsing
 * and maintains radar data points buffer for visualization.
 */

import { useEffect, useState } from 'react';
import { parseRadarData } from '../lib/parser';
import RadarDisplay from './RadarDisplay';
import type { RadarDataPoint, SweepDirection } from '../types';

interface LiveRadarDisplayProps {
  /** Latest raw data from WebSocket */
  rawData: string | null;
  /** Maximum distance for radar display */
  maxDistance?: number;
  /** Maximum number of data points to keep */
  maxDataPoints?: number;
  /** Width of the radar display */
  width?: number;
  /** Height of the radar display */
  height?: number;
}

export function LiveRadarDisplay({ 
  rawData, 
  maxDistance = 200,
  maxDataPoints = 100,
  width = 600,
  height = 400 
}: LiveRadarDisplayProps) {
  const [dataPoints, setDataPoints] = useState<RadarDataPoint[]>([]);
  const [currentAngle, setCurrentAngle] = useState<number>(90);
  const [sweepDirection, setSweepDirection] = useState<SweepDirection>('forward');
  const [lastValidAngle, setLastValidAngle] = useState<number>(90);

  // Process new radar data
  useEffect(() => {
    if (rawData) {
      const parsed = parseRadarData(rawData);
      if (parsed) {
        setCurrentAngle(parsed.angle);
        
        // Determine sweep direction
        if (parsed.angle > lastValidAngle) {
          setSweepDirection('forward');
        } else if (parsed.angle < lastValidAngle) {
          setSweepDirection('backward');
        }
        setLastValidAngle(parsed.angle);

        // Add new data point and maintain buffer
        setDataPoints(prev => {
          const updated = [...prev, parsed];
          
          // Keep only the most recent points
          if (updated.length > maxDataPoints) {
            return updated.slice(-maxDataPoints);
          }
          
          return updated;
        });
      }
    }
  }, [rawData, maxDataPoints, lastValidAngle]);

  return (
    <RadarDisplay
      dataPoints={dataPoints}
      sweepDirection={sweepDirection}
      currentAngle={currentAngle}
      width={width}
      height={height}
      maxDistance={maxDistance}
      showGrid={true}
      showAngleLines={true}
      showSweepLine={true}
    />
  );
}