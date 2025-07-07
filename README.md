# unoRadar

A minimal radar system built from first principles.

An ultrasonic sensor sweeps 180 degrees on a servo motor, measuring distances.

The Arduino transmits data through a C++ backend to a C++ frontend.

Military-grade aspirations within consumer hardware constraints.

Simple components, precise measurements, real-time visualization.

## Hardware

### Development board

- Elegoo UNO R3 (Arduino UNO R3 Clone)

### Micro Servo Motor

- SG90

### Ultrasonic Sensor

- HC-SR04s

## Software

### Firmware

- Language: C++

### Backend

- Language: C++

### Frontend

- Language: TypeScript, CSS
- Frameworks: Remix, Tailwind CSS

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
