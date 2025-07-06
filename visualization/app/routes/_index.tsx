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

        {/* Radar Control Strip */}
        <div className="bg-black border border-green-500 rounded-sm p-3 mb-6">
          <div className="flex items-center justify-between font-mono text-sm">
            <span className="text-green-400 font-bold tracking-wider">RADAR CONTROL</span>
            
            <div className="flex items-center gap-6">
              {/* WebSocket Status */}
              <div className="flex items-center gap-2">
                <span className="text-green-300">WEBSOCKET</span>
                <div className={`w-2 h-2 rounded-full ${isConnected ? 'bg-green-400' : 'bg-red-500'}`}></div>
                <span className={`text-xs ${isConnected ? 'text-green-400' : 'text-red-400'}`}>
                  {isConnected ? 'CONNECTED' : 'DISCONNECTED'}
                </span>
              </div>

              {/* Arduino Status */}
              <div className="flex items-center gap-2">
                <span className="text-green-300">ARDUINO</span>
                <div className={`w-2 h-2 rounded-full ${
                  serialStatus === 'connected' ? 'bg-green-400' :
                  serialStatus === 'connecting' ? 'bg-yellow-400 animate-pulse' : 'bg-red-500'
                }`}></div>
                <span className={`text-xs ${
                  serialStatus === 'connected' ? 'text-green-400' :
                  serialStatus === 'connecting' ? 'text-yellow-400' : 'text-red-400'
                }`}>
                  {serialStatus === 'connected' ? 'CONNECTED' :
                   serialStatus === 'connecting' ? 'SYNC' : 'DISCONNECTED'}
                </span>
              </div>

              {/* Single Toggle Button */}
              <button
                onClick={serialStatus === 'connected' ? disconnectFromArduino : connectToArduino}
                disabled={!isConnected || serialStatus === 'connecting'}
                className={`px-4 py-1 border font-mono text-xs tracking-wider transition-all duration-200 ${
                  serialStatus === 'connected' 
                    ? 'bg-red-900 hover:bg-red-800 border-red-600 text-red-100' 
                    : 'bg-green-900 hover:bg-green-800 border-green-600 text-green-100'
                } disabled:bg-gray-700 disabled:border-gray-600 disabled:text-gray-400 disabled:cursor-not-allowed`}
              >
                {serialStatus === 'connected' ? 'DISCONNECT' :
                 serialStatus === 'connecting' ? 'CONNECTING...' : 'CONNECT'}
              </button>
            </div>
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