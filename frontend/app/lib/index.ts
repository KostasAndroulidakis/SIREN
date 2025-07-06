/**
 * @file index.ts
 * @brief Library exports barrel
 * @author unoRadar Project
 * @date 2025
 */

export { ArduinoSerial, DEFAULT_SERIAL_CONFIG } from './serial';
export { RadarWebSocketServer } from './websocket-server';
export type { WSMessage } from './websocket-server';
export { RadarDataParser, parseRadarData, isValidRadarFormat } from './parser';
export type { ParseResult } from './parser';
export { RadarDataBuffer, globalRadarBuffer } from './data-buffer';
export type { BufferStats } from './data-buffer';