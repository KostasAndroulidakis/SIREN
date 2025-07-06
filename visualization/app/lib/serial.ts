/**
 * @file serial.ts
 * @brief Serial communication interface for Arduino radar data
 * @author unoRadar Project
 * @date 2025
 * 
 * Handles serial port connection, data reception, and connection management
 * for receiving radar data from Arduino unoRadar system.
 */

import { SerialPort } from 'serialport';
import { ReadlineParser } from '@serialport/parser-readline';
import type { SerialConfig, SerialStatus } from '~/types';

/**
 * Serial communication manager for Arduino radar
 */
export class ArduinoSerial {
  private port: SerialPort | null = null;
  private parser: ReadlineParser | null = null;
  private status: SerialStatus = 'disconnected';
  private onDataCallback: ((data: string) => void) | null = null;
  private onStatusCallback: ((status: SerialStatus) => void) | null = null;

  /**
   * Connect to Arduino serial port
   * @param config Serial port configuration
   */
  async connect(config: SerialConfig): Promise<void> {
    try {
      this.updateStatus('connecting');

      this.port = new SerialPort({
        path: config.path,
        baudRate: config.baudRate,
        autoOpen: false,
      });

      // Set up line parser for Arduino text output
      this.parser = this.port.pipe(new ReadlineParser({ delimiter: '\n' }));

      // Set up event handlers
      this.setupEventHandlers();

      // Open the connection
      await new Promise<void>((resolve, reject) => {
        this.port!.open((err) => {
          if (err) {
            reject(new Error(`Failed to open serial port: ${err.message}`));
          } else {
            resolve();
          }
        });
      });

      this.updateStatus('connected');
    } catch (error) {
      this.updateStatus('error');
      throw error;
    }
  }

  /**
   * Disconnect from Arduino
   */
  async disconnect(): Promise<void> {
    if (this.port && this.port.isOpen) {
      await new Promise<void>((resolve) => {
        this.port!.close(() => {
          resolve();
        });
      });
    }
    
    this.port = null;
    this.parser = null;
    this.updateStatus('disconnected');
  }

  /**
   * Get current connection status
   */
  getStatus(): SerialStatus {
    return this.status;
  }

  /**
   * Set data receive callback
   */
  onData(callback: (data: string) => void): void {
    this.onDataCallback = callback;
  }

  /**
   * Set status change callback
   */
  onStatusChange(callback: (status: SerialStatus) => void): void {
    this.onStatusCallback = callback;
  }

  /**
   * List available serial ports
   */
  static async listPorts(): Promise<Array<{ path: string; manufacturer?: string }>> {
    const ports = await SerialPort.list();
    return ports.map(port => ({
      path: port.path,
      manufacturer: port.manufacturer,
    }));
  }

  /**
   * Set up serial port event handlers
   */
  private setupEventHandlers(): void {
    if (!this.port || !this.parser) return;

    // Handle incoming data
    this.parser.on('data', (data: string) => {
      const cleanData = data.trim();
      if (cleanData && this.onDataCallback) {
        this.onDataCallback(cleanData);
      }
    });

    // Handle connection errors
    this.port.on('error', (err) => {
      console.error('Serial port error:', err);
      this.updateStatus('error');
    });

    // Handle disconnection
    this.port.on('close', () => {
      this.updateStatus('disconnected');
    });
  }

  /**
   * Update connection status and notify listeners
   */
  private updateStatus(newStatus: SerialStatus): void {
    this.status = newStatus;
    if (this.onStatusCallback) {
      this.onStatusCallback(newStatus);
    }
  }
}

/**
 * Default serial configuration for Arduino
 */
export const DEFAULT_SERIAL_CONFIG: SerialConfig = {
  path: '/dev/cu.usbmodem21201', // Mac Arduino path
  baudRate: 9600,                // Must match Arduino SERIAL_BAUD_RATE
};