/**
 * @file server.ts
 * @brief Radar WebSocket server startup script
 * @author unoRadar Project
 * @date 2025
 * 
 * Standalone server for radar data streaming. Run this alongside the Remix app
 * to enable real-time Arduino communication.
 */

import { RadarWebSocketServer } from './app/lib/websocket-server';

const WS_PORT = 8080;

async function startRadarServer() {
  console.log('🎯 Starting unoRadar WebSocket server...');
  
  const server = new RadarWebSocketServer(WS_PORT);
  
  // Graceful shutdown handling
  process.on('SIGINT', async () => {
    console.log('\n🛑 Received SIGINT, shutting down gracefully...');
    await server.shutdown();
    process.exit(0);
  });

  process.on('SIGTERM', async () => {
    console.log('\n🛑 Received SIGTERM, shutting down gracefully...');
    await server.shutdown();
    process.exit(0);
  });

  console.log(`✅ Radar server ready! Connect your Arduino and use the web interface.`);
  console.log(`📡 WebSocket: ws://localhost:${WS_PORT}`);
}

// Start the server
startRadarServer().catch((error) => {
  console.error('❌ Failed to start radar server:', error);
  process.exit(1);
});