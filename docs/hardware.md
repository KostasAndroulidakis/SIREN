# Hardware

## Parts

### Development board

- Elegoo UNO R3 (Arduino UNO R3 Clone)

### Micro Servo Motor

- SG90

### Ultrasonic Sensor

- HC-SR04s

### Temperature & Humidity Sensor

- DHT11

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
