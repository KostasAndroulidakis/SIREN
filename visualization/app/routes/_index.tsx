import type { MetaFunction } from "@remix-run/node";
import { useWebSocket } from "../hooks/useWebSocket";
import { RawDataDisplay } from "../components/RawDataDisplay";
import { LiveRadarDisplay } from "../components/LiveRadarDisplay";

export const meta: MetaFunction = () => {
  return [
    { title: "unoRadar" },
    { name: "description", content: "Arduino Radar Visualization" },
  ];
};

export default function Index() {
  const {
    isConnected,
    serialStatus,
    lastMessage,
    connectToArduino,
    disconnectFromArduino
  } = useWebSocket();

  return (
    <div className="min-h-screen bg-black p-8">
      <div className="max-w-6xl mx-auto">
        {/* Header */}
        <h1 className="text-green-400 font-mono text-2xl font-bold mb-8 text-center">
          UNO RADAR - LIVE VISUALIZATION
        </h1>

        {/* Connection Status */}
        <div className="bg-gray-900 border border-green-900 rounded-lg p-6 mb-6">
          <h2 className="text-green-400 font-mono font-bold mb-4">CONNECTION STATUS</h2>

          <div className="grid grid-cols-2 gap-4 text-green-300 font-mono text-sm">
            <div className="flex justify-between">
              <span>WebSocket:</span>
              <span className={isConnected ? 'text-green-400' : 'text-red-400'}>
                {isConnected ? '🟢 CONNECTED' : '🔴 DISCONNECTED'}
              </span>
            </div>

            <div className="flex justify-between">
              <span>Arduino Serial:</span>
              <span className={
                serialStatus === 'connected' ? 'text-green-400' :
                serialStatus === 'connecting' ? 'text-yellow-400' : 'text-red-400'
              }>
                {serialStatus === 'connected' ? '🟢 CONNECTED' :
                 serialStatus === 'connecting' ? '🟡 CONNECTING' : '🔴 DISCONNECTED'}
              </span>
            </div>
          </div>
        </div>

        {/* Controls */}
        <div className="bg-gray-900 border border-green-900 rounded-lg p-6 mb-6">
          <h2 className="text-green-400 font-mono font-bold mb-4">CONTROLS</h2>

          <div className="flex gap-4">
            <button
              onClick={connectToArduino}
              disabled={!isConnected || serialStatus === 'connected'}
              className="px-6 py-2 bg-green-800 hover:bg-green-700 disabled:bg-gray-600 disabled:cursor-not-allowed rounded border border-green-600 font-mono text-sm text-green-100 transition-colors"
            >
              {serialStatus === 'connected' ? 'ARDUINO CONNECTED' : 
               serialStatus === 'connecting' ? 'CONNECTING...' : 'CONNECT ARDUINO'}
            </button>

            <button
              onClick={disconnectFromArduino}
              disabled={!isConnected || serialStatus === 'disconnected'}
              className="px-6 py-2 bg-red-800 hover:bg-red-700 disabled:bg-gray-600 disabled:cursor-not-allowed rounded border border-red-600 font-mono text-sm text-red-100 transition-colors"
            >
              DISCONNECT ARDUINO
            </button>
          </div>
        </div>

        {/* Live Radar Display */}
        <div className="mb-6">
          <LiveRadarDisplay 
            rawData={lastMessage} 
            maxDistance={400}
            maxDataPoints={50}
            width={800}
            height={450}
          />
        </div>

        {/* Raw Data Display */}
        <RawDataDisplay rawData={lastMessage} maxLines={10} />

        {/* Instructions */}
        <div className="mt-8 text-green-600 font-mono text-xs text-center">
          <p>1. Start radar server: npm run radar-server</p>
          <p>2. Connect Arduino to USB port</p>
          <p>3. Click &quot;CONNECT ARDUINO&quot; button</p>
        </div>
      </div>
    </div>
  );
}