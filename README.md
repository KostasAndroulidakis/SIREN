# SIREN

![SIREN Logo](siren-logo.png)

A minimal sonar system built from first principles.

An ultrasonic sensor sweeps 180 degrees on a servo motor, measuring distances.

The Arduino transmits data through a C++ backend to a C++ frontend.

Military-grade aspirations within consumer hardware constraints.

Simple components, precise measurements, real-time visualization.

## Data Flow

```text
Firmware (Arduino) <--> Backend (Websocket) <--> Frontend (GUI)
```

## Hardware

### Development board

- Elegoo UNO R3 (Arduino UNO R3 Clone)

### Micro Servo Motor

- SG90

### Ultrasonic Sensor

- HC-SR04s

### Temperature & Humidity Sensor

- DHT11

## Software

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
- Purpose: Military-grade GUI with WebSocket client and sonar visualization

### Architecture

```text
main.cpp → launches → backend (WebSocket server) ← connects ← frontend (Qt6 GUI)
                          ↕
                    firmware (Arduino)
```

### Build & Run

```bash
# Build components
cd backend/build && make
cd frontend/build && make

# Compile launcher
g++ -std=c++17 main.cpp -o SIREN

# Run system
./SIREN
```

## PIN Connections

### Elegoo UNO R3 (Arduino UNO clone)

```text
🔴 5V (Power)   ──→  Breadboard (+)
⚫ GND (Power)  ──→  Breadboard (-)
```

### Ultrasonic sensor HC-SR04

```text
🔴 VCC          ──→  Breadboard (+)
🟡 Trig         ──→  UNO D2
🟢 Echo         ──→  UNO D3 PWM
⚫ GND          ──→  Breadboard (-)
```

### Micro Servo Motor SG90

```text
🔴 Pin 1 (+)    ──→  Breadboard (+)
🟠 Pin 2 (PWM)  ──→  UNO D9 PWM
⚫ Pin 3 (-)    ──→  Breadboard (-)
```

### Temperature & Humidity Sensor DHT11

```text
🔴 VCC          ──→  Breadboard (+)
🟢 DATA         ──→  UNO D4
⚫ GND          ──→  Breadboard (-)
```
