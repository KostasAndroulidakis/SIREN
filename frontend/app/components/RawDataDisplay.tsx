/**
 * @file RawDataDisplay.tsx
 * @brief Raw radar data display component
 * @author unoRadar Project
 * @date 2025
 *
 * Displays real-time raw radar data from Arduino in a terminal-like interface
 * with parsing validation and army-grade styling.
 */

import { useEffect, useState } from 'react';
import { RadarDataParser, type ParseResult } from '../lib/parser';

interface RawDataDisplayProps {
  /** Latest raw data from WebSocket */
  rawData: string | null;
  /** Maximum number of data lines to display */
  maxLines?: number;
}

interface DataLine {
  id: string;
  timestamp: number;
  rawData: string;
  parseResult: ParseResult;
}

export function RawDataDisplay({ rawData, maxLines = 20 }: RawDataDisplayProps) {
  const [dataLines, setDataLines] = useState<DataLine[]>([]);
  const [parser] = useState(() => new RadarDataParser());

  // Process new raw data
  useEffect(() => {
    if (rawData) {
      const parseResult = parser.parseDataLine(rawData);

      const newLine: DataLine = {
        id: crypto.randomUUID(),
        timestamp: Date.now(),
        rawData,
        parseResult,
      };

      setDataLines(prev => {
        const updated = [newLine, ...prev];
        return updated.slice(0, maxLines);
      });
    }
  }, [rawData, parser, maxLines]);

  const formatTimestamp = (timestamp: number) => {
    return new Date(timestamp).toLocaleTimeString('en-US', {
      hour12: false,
      hour: '2-digit',
      minute: '2-digit',
      second: '2-digit',
      fractionalSecondDigits: 3,
    });
  };

  const getStatusColor = (success: boolean) => {
    return success ? 'text-green-400' : 'text-red-400';
  };

  const getStatusIcon = (success: boolean) => {
    return success ? '✓' : '✗';
  };

  return (
    <div className="bg-gray-900 border border-green-900 rounded-lg p-6">
      <div className="flex items-center justify-between mb-4">
        <h2 className="text-green-400 font-mono font-bold">RAW DATA STREAM</h2>
        <div className="text-green-600 font-mono text-xs">
          {dataLines.length} / {maxLines} lines
        </div>
      </div>

      {/* Data Statistics */}
      <div className="grid grid-cols-3 gap-4 mb-4 text-green-300 font-mono text-xs">
        <div className="bg-black border border-green-800 rounded p-2">
          <div className="text-green-500">TOTAL RECEIVED</div>
          <div className="text-lg font-bold">{dataLines.length}</div>
        </div>
        <div className="bg-black border border-green-800 rounded p-2">
          <div className="text-green-500">VALID PARSED</div>
          <div className="text-lg font-bold text-green-400">
            {dataLines.filter(line => line.parseResult.success).length}
          </div>
        </div>
        <div className="bg-black border border-green-800 rounded p-2">
          <div className="text-red-500">PARSE ERRORS</div>
          <div className="text-lg font-bold text-red-400">
            {dataLines.filter(line => !line.parseResult.success).length}
          </div>
        </div>
      </div>

      {/* Data Log */}
      <div className="bg-black border border-green-800 rounded-lg p-4 h-80 overflow-y-auto">
        <div className="font-mono text-xs space-y-1">
          {dataLines.length === 0 ? (
            <div className="text-gray-500 text-center py-8">
              Waiting for radar data...
            </div>
          ) : (
            dataLines.map((line) => (
              <div key={line.id} className="flex items-start gap-2 py-1 border-b border-gray-800">
                {/* Timestamp */}
                <div className="text-green-600 w-20 shrink-0">
                  {formatTimestamp(line.timestamp)}
                </div>

                {/* Status */}
                <div className={`w-4 shrink-0 ${getStatusColor(line.parseResult.success)}`}>
                  {getStatusIcon(line.parseResult.success)}
                </div>

                {/* Raw Data */}
                <div className="text-green-300 flex-1 min-w-0">
                  <div className="truncate">{line.rawData}</div>

                  {/* Parsed Data or Error */}
                  {line.parseResult.success && line.parseResult.data ? (
                    <div className="text-green-400 text-xs mt-1">
                      → Angle: {line.parseResult.data.angle}° | Distance: {line.parseResult.data.distance}cm
                    </div>
                  ) : (
                    <div className="text-red-400 text-xs mt-1">
                      → Error: {line.parseResult.error}
                    </div>
                  )}
                </div>
              </div>
            ))
          )}
        </div>
      </div>

      {/* Current Sweep Info */}
      {dataLines.length > 0 && dataLines[0].parseResult.success && (
        <div className="mt-4 text-green-600 font-mono text-xs text-center">
          Sweep Direction: {parser.getSweepDirection().toUpperCase()} |
          Last Valid: {dataLines.find(line => line.parseResult.success)?.parseResult.data &&
            `${dataLines.find(line => line.parseResult.success)?.parseResult.data?.angle}° @ ${dataLines.find(line => line.parseResult.success)?.parseResult.data?.distance}cm`}
        </div>
      )}
    </div>
  );
}