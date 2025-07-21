# Software

## Data Flow

```text
Firmware (Arduino) <--> Backend (Websocket) <--> Frontend (GUI)
```

## Architecture

```text
main.cpp → launches → backend (WebSocket server) ← connects ← frontend (Qt6 GUI)
                          ↕
                    firmware (Arduino)
```

## Tech Stack

### System Launcher

- Language: C++
- File: `main.cpp`
- Purpose: MISRA-compliant process orchestrator for backend and frontend

### Firmware

- Language: C++ (Arduino)
- Location: `firmware/`
- Purpose: Sensor data collection and servo control

### Backend

- Language: C++
- Framework: Boost.Asio + Boost.Beast
- Location: `backend/`
- Purpose: WebSocket server, serial communication, real-time data processing

### Frontend

- Language: C++
- Framework: Qt6 LTS
- Location: `frontend/`
- Purpose: Military-grade GUI with WebSocket client and sonar visualization.
