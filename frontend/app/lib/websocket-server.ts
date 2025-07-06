/**
 * @file websocket-server.ts
 * @brief WebSocket server for real-time radar data streaming
 * @author unoRadar Project
 * @date 2025
 *
 * Creates WebSocket server to stream Arduino radar data to frontend clients
 * in real-time. Handles multiple client connections and data broadcasting.
 */

import { WebSocketServer, WebSocket } from 'ws';
import { ArduinoSerial, DEFAULT_SERIAL_CONFIG } from './serial';
import { WEBSOCKET_CONFIG } from '../constants';
import type { SerialConfig, SerialStatus } from '~/types';

/**
 * WebSocket message types
 */
export interface WSMessage {
  type: 'radar-data' | 'serial-status' | 'connection-request' | 'disconnect-request';
  payload?: {
    rawData?: string;
    timestamp?: number;
    status?: SerialStatus;
    config?: SerialConfig;
  };
}

/**
 * WebSocket server for radar data streaming
 */
export class RadarWebSocketServer {
  private wss: WebSocketServer;
  private arduino: ArduinoSerial;
  private clients: Set<WebSocket> = new Set();

  constructor(port: number = WEBSOCKET_CONFIG.DEFAULT_PORT) {
    this.wss = new WebSocketServer({ port });
    this.arduino = new ArduinoSerial();
    this.setupWebSocketServer();
    this.setupArduinoHandlers();

    console.log(`🚀 Radar WebSocket server running on ws://localhost:${port}`);
  }

  /**
   * Connect to Arduino with specified configuration
   */
  async connectArduino(config: SerialConfig = DEFAULT_SERIAL_CONFIG): Promise<void> {
    try {
      await this.arduino.connect(config);
      console.log(`✅ Connected to Arduino on ${config.path}`);
    } catch (error) {
      console.error('❌ Failed to connect to Arduino:', error);
      throw error;
    }
  }

  /**
   * Disconnect from Arduino
   */
  async disconnectArduino(): Promise<void> {
    await this.arduino.disconnect();
    console.log('🔌 Disconnected from Arduino');
  }

  /**
   * Broadcast message to all connected clients
   */
  private broadcast(message: WSMessage): void {
    const data = JSON.stringify(message);
    this.clients.forEach(client => {
      if (client.readyState === WebSocket.OPEN) {
        client.send(data);
      }
    });
  }

  /**
   * Set up WebSocket server event handlers
   */
  private setupWebSocketServer(): void {
    this.wss.on('connection', (ws) => {
      console.log('👤 Client connected');
      this.clients.add(ws);

      // Send current Arduino status to new client
      ws.send(JSON.stringify({
        type: 'serial-status',
        payload: { status: this.arduino.getStatus() }
      }));

      // Handle client messages
      ws.on('message', async (data) => {
        try {
          const message: WSMessage = JSON.parse(data.toString());
          await this.handleClientMessage(message);
        } catch (error) {
          console.error('Error handling client message:', error);
        }
      });

      // Handle client disconnection
      ws.on('close', () => {
        console.log('👤 Client disconnected');
        this.clients.delete(ws);
      });
    });
  }

  /**
   * Set up Arduino event handlers
   */
  private setupArduinoHandlers(): void {
    // Handle incoming radar data
    this.arduino.onData((data: string) => {
      this.broadcast({
        type: 'radar-data',
        payload: { rawData: data, timestamp: Date.now() }
      });
    });

    // Handle Arduino status changes
    this.arduino.onStatusChange((status: SerialStatus) => {
      console.log(`📡 Arduino status: ${status}`);
      this.broadcast({
        type: 'serial-status',
        payload: { status }
      });
    });
  }

  /**
   * Handle messages from WebSocket clients
   */
  private async handleClientMessage(message: WSMessage): Promise<void> {
    switch (message.type) {
      case 'connection-request': {
        const config = message.payload?.config || DEFAULT_SERIAL_CONFIG;
        await this.connectArduino(config);
        break;
      }

      case 'disconnect-request': {
        await this.disconnectArduino();
        break;
      }

      default: {
        console.warn('Unknown message type:', message.type);
      }
    }
  }

  /**
   * Gracefully shutdown the server
   */
  async shutdown(): Promise<void> {
    console.log('🛑 Shutting down radar server...');
    await this.disconnectArduino();
    this.wss.close();
  }
}