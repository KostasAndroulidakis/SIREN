# SIREN

Sonic Imaging for Range Exploration and Navigation

![SIREN Logo](siren-logo.png)

#### Video Demo

<>

#### Description

SIREN is an ultrasonic radar system built with Arduino that detects objects within a 180-degree field of view. The system combines multiple sensors to achieve accurate distance measurements by compensating for environmental conditions in real-time.

## What It Does

The system rotates an ultrasonic sensor using a servo motor, sweeping from 0 to 180 degrees and back. At each angle, it measures the distance to the nearest object and transmits the data over serial connection. When an object comes within 100cm, an alert system activates with LED and buzzer feedback that increases in urgency as the object gets closer.

What makes this project unique is the environmental compensation: a temperature and humidity sensor continuously monitors air conditions to calculate the actual speed of sound, rather than assuming a fixed value of 343 m/s. This can improve measurement accuracy by up to 5% in varying conditions.

## Hardware Components

- **Elegoo UNO R3** - Arduino-compatible microcontroller
- **HC-SR04** - Ultrasonic distance sensor (2-400cm range, 15° beam angle)
- **SG90** - Micro servo motor for 180° rotation
- **DHT11** - Temperature and humidity sensor
- **Passive Buzzer** - Audio alert
- **Red LED** - Visual alert
- **Push Button** - Start/stop control

## The Physics

### Speed of Sound

Sound doesn't travel at a constant speed. It varies with air conditions:

```
c = 331.3 + (0.606 × T) + (0.0124 × RH)
```

Where:

- `c` = speed of sound (m/s)
- `T` = temperature (°C)
- `RH` = relative humidity (%)

At 20°C and 50% humidity, sound travels at ~344 m/s. At 0°C, it's only 331 m/s. This 4% difference translates to 4cm error at 1 meter distance. SIREN compensates for this automatically.

### Distance Calculation

The ultrasonic sensor emits a 40kHz pulse and measures how long it takes for the echo to return:

```
distance = (time × speed_of_sound) / 2
```

We divide by 2 because the sound travels to the object AND back, covering twice the actual distance.

## Software Architecture

The firmware is organized into modular components, each handling a specific responsibility:

### Core Files

**firmware.ino** - Main entry point. Initializes all components and runs the main loop. Handles the state machine for start/stop functionality and coordinates the scanning process.

**config.h** - Central configuration file containing all pin definitions and shared data structures. Makes it easy to adapt the project to different wiring configurations.

### Sensor Modules

**Ultrasonic.h / Ultrasonic.cpp** - Driver for the HC-SR04 sensor. Implements the trigger-echo sequence and converts time measurements to distance. Includes timeout handling for out-of-range objects.

**DHTSensor.h / DHTSensor.cpp** - Non-blocking driver for the DHT11 sensor. Uses a caching pattern to avoid blocking the main loop: readings are cached and only updated every 2 seconds (the minimum interval allowed by the sensor's datasheet).

**SpeedOfSound.h / SpeedOfSound.cpp** - Calculates the speed of sound based on current temperature and humidity. Contains the physics formula with documentation explaining the derivation.

### Actuator Modules

**Servo.h / Servo.cpp** - Wrapper for the SG90 servo motor. Implements detach/attach pattern to avoid PWM interference with ultrasonic timing measurements. The Arduino's Timer1 is used by both the Servo library and pulseIn(), so we temporarily disable the servo during distance readings.

**Alert.h / Alert.cpp** - Manages the LED and buzzer alert system. Implements three zones:

- **Safe (>100cm)**: No alert
- **Warning (10-100cm)**: Blinking/beeping at 60-150 BPM, faster as objects get closer
- **Danger (≤10cm)**: Continuous LED and buzzer

Uses `tone()` function for the passive buzzer to generate audible 2kHz signal.

### Input Module

**Button.h / Button.cpp** - Debounced button input for start/stop control. Implements software debouncing with 50ms delay to filter mechanical contact bounce. The debounce logic preserves button press events even if they occur during the cooldown period.

### Orchestration

**Scanner.h / Scanner.cpp** - Coordinates the scanning process. Performs bidirectional sweeps (0→180→0) and outputs data in CSV format: `angle,distance,humidity,temperatureC,temperatureF`. Checks for button interrupts during scan to allow stopping mid-sweep.

## Design Decisions

**Why detach the servo during measurements?**
The Servo library and pulseIn() both use timer interrupts. Running them simultaneously causes jitter in timing measurements, leading to distance errors. Detaching the servo temporarily eliminates this interference.

**Why 60ms delay between steps?**
The HC-SR04 datasheet recommends at least 60ms between measurements. The SG90 servo also needs time to physically reach each position (~1.7ms per degree). 60ms satisfies both requirements.

**Why cache DHT readings?**
The DHT11 requires minimum 1 second between readings and each read blocks for ~25ms. Caching allows us to reuse recent readings without blocking the radar sweep.

**Why median filtering was not implemented?**
While median filtering could reduce noise, testing showed single readings were sufficiently stable for this application. The added complexity and ~4ms delay per reading were not justified.

**Why manual debouncing instead of a library?**
For a CS50 project, implementing debouncing manually demonstrates understanding of timing-based input filtering, rather than hiding it behind a library abstraction.

## Output Format

Serial output at 115200 baud, CSV format:

```
angle,distance,humidity,temperatureC,temperatureF
0,45.23,52.00,24.30,75.74
1,45.18,52.00,24.30,75.74
...
```

Distance of `-1` indicates no object detected or out-of-range reading.

## Future Improvements

- Processing/Python visualization frontend
- Multiple ultrasonic sensors for faster scanning
- Data logging to SD card
- WiFi transmission for remote monitoring
