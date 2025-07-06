/**
 * @file useWebSocket.ts
 * @brief WebSocket hook for real-time Arduino data
 * @author unoRadar Project
 * @date 2025
 */

import { useEffect, useState, useCallback } from 'react';
import type { SerialStatus } from '~/types';

interface WebSocketMessage {
  type: 'radar-data' | 'serial-status' | 'connection-response';
  payload?: {
    rawData?: string;
    status?: SerialStatus;
    timestamp?: number;
  };
}

interface UseWebSocketReturn {
  isConnected: boolean;
  serialStatus: SerialStatus;
  lastMessage: string | null;
  connectToArduino: () => void;
  disconnectFromArduino: () => void;
}

export function useWebSocket(url: string = 'ws://localhost:8080'): UseWebSocketReturn {
  const [ws, setWs] = useState<WebSocket | null>(null);
  const [isConnected, setIsConnected] = useState(false);
  const [serialStatus, setSerialStatus] = useState<SerialStatus>('disconnected');
  const [lastMessage, setLastMessage] = useState<string | null>(null);

  // Connect to WebSocket server
  useEffect(() => {
    const websocket = new WebSocket(url);

    websocket.onopen = () => {
      console.log('🔗 Connected to radar WebSocket server');
      setIsConnected(true);
      setWs(websocket);
    };

    websocket.onmessage = (event) => {
      try {
        const message: WebSocketMessage = JSON.parse(event.data);
        
        switch (message.type) {
          case 'serial-status':
            if (message.payload?.status) {
              setSerialStatus(message.payload.status);
              console.log('📡 Serial status:', message.payload.status);
            }
            break;
            
          case 'radar-data':
            if (message.payload?.rawData) {
              setLastMessage(message.payload.rawData);
              console.log('📊 Radar data:', message.payload.rawData);
            }
            break;
        }
      } catch (error) {
        console.error('❌ Error parsing WebSocket message:', error);
      }
    };

    websocket.onclose = () => {
      console.log('🔌 Disconnected from radar WebSocket server');
      setIsConnected(false);
      setWs(null);
    };

    websocket.onerror = (error) => {
      console.error('❌ WebSocket error:', error);
    };

    return () => {
      websocket.close();
    };
  }, [url]);

  // Connect to Arduino
  const connectToArduino = useCallback(() => {
    if (ws && ws.readyState === WebSocket.OPEN) {
      const message = {
        type: 'connection-request',
        payload: {
          config: {
            path: '/dev/ttyUSB0', // Default path
            baudRate: 9600
          }
        }
      };
      ws.send(JSON.stringify(message));
      console.log('📞 Requesting Arduino connection...');
    }
  }, [ws]);

  // Disconnect from Arduino
  const disconnectFromArduino = useCallback(() => {
    if (ws && ws.readyState === WebSocket.OPEN) {
      const message = {
        type: 'disconnect-request'
      };
      ws.send(JSON.stringify(message));
      console.log('📴 Requesting Arduino disconnection...');
    }
  }, [ws]);

  return {
    isConnected,
    serialStatus,
    lastMessage,
    connectToArduino,
    disconnectFromArduino,
  };
}