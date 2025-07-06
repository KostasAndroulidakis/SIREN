# unoRadar

A minimal radar system built from first principles. An ultrasonic sensor sweeps 180 degrees on a servo motor, measuring distances. The Arduino transmits data through a C++ server to a TypeScript web interface.

Military-grade aspirations within consumer hardware constraints.

Simple components, precise measurements, real-time visualization.

## Hardware

- __Development board:__    Elegoo UNO R3 (Arduino UNO R3 Clone)
- __Micro Servo Motor:__    SG90
- __Ultrasonic Sensor:__    HC-SR04s

## Software

### Development Board

- Language: C++

### Backend

- Language: C++

### Frontend

- Language: TypeScript, CSS
- Frameworks: Remix, Tailwind CSS

## PIN Connections

### Elegoo UNO R3 (Arduino UNO clone)

```text
5V (Power)  ──→  Breadboard (+)
GND (Power) ──→  Breadboard (-)
```

### Ultrasonic sensor HC-SR04

```text
VCC   ──→  Breadboard (+)
Trig  ──→  UNO D2
Echo  ──→  UNO D3 PWM
GND   ──→  Breadboard (-)
```

### Micro Servo Motor SG90

```text
Pin 1 (+)    ──→  Breadboard (+)
Pin 2 (PWM)  ──→  UNO D9 PWM
Pin 3 (-)    ──→  Breadboard (-)
```
