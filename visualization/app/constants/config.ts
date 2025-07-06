/**
 * @file config.ts
 * @brief Centralized configuration constants for unoRadar system
 * @author unoRadar Project
 * @date 2025
 * 
 * Single source of truth for all configuration values.
 * No magic numbers or hardcoded values should exist outside this file.
 */

/**
 * Serial communication configuration
 */
export const SERIAL_CONFIG = {
  DEFAULT_PATH: '/dev/cu.usbmodem21201', // Mac Arduino path (update per machine)
  BAUD_RATE: 9600,                       // Must match Arduino SERIAL_BAUD_RATE
  CONNECTION_TIMEOUT_MS: 5000,           // Connection timeout in milliseconds
  RETRY_ATTEMPTS: 3,                     // Number of connection retry attempts
} as const;

/**
 * WebSocket server configuration
 */
export const WEBSOCKET_CONFIG = {
  DEFAULT_URL: 'ws://localhost:8080',    // Default WebSocket URL for frontend
  DEFAULT_PORT: 8080,                    // Default WebSocket server port
  RECONNECT_ATTEMPTS: 3,                 // Frontend reconnection attempts
  HEARTBEAT_INTERVAL_MS: 30000,          // WebSocket heartbeat interval
} as const;

/**
 * Application configuration
 */
export const APP_CONFIG = {
  UPDATE_INTERVAL_MS: 100,               // UI update interval
  DATA_RETENTION_MS: 300000,             // How long to keep data (5 minutes)
  MAX_BUFFER_SIZE: 2000,                 // Maximum radar data points in buffer
} as const;