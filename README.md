# unoRadar

unoRadar is a hardware-software project that combines Arduino-based radar functionality with a TypeScript Remix web app for data visualization.

The system uses an ultrasonic sensor mounted on a servo motor to perform 180-degree sweeps and transmit distance measurements to a web interface.

## Tech Stack

### Hardware

- Language: C/C++ but stored in Arduino `ino` files
- IDE: Arduino IDE

### Software

- Language: TypeScript
- Framework: Remix
- IDE: VSCode

## Parts &  PIN Connections

### Elegoo UNO R3 (Arduino UNO clone)

| From            | To                |
| --------------- | ----------------- |
| 5V (Power)      | Breadboard (+)    |
| GND (Power)     | Breadboard (-)    |

### Ultrasonic sensor HC-SR04

| From            | To                |
| --------------- | ----------------- |
| VCC             | Breadboard (+)    |
| Trig            | UNO D2            |
| Echo            | UNO D3 PWM        |
| GND             | Breadboard (-)    |

### Micro Servo Motor SG90

| From            | To                |
| --------------- | ----------------- |
| 1 (+)           | Breadboard (+)    |
| 2 pulse         | UNO D9 PWM        |
| 3 (-)           | Breadboard (-)    |
