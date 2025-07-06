# unoRadar

unoRadar is a hardware-software project that combines Arduino-based radar functionality with a TypeScript Remix web app for data visualization.

The system uses an ultrasonic sensor mounted on a servo motor to perform 180-degree sweeps and transmit distance measurements to a web interface.

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
