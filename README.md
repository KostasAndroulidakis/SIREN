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

```text
c = 331.3 + (0.606 × T) + (0.0124 × RH)
```

Where:

- `c` = speed of sound (m/s)
- `T` = temperature (°C)
- `RH` = relative humidity (%)

At 20°C and 50% humidity, sound travels at ~344 m/s. At 0°C, it's only 331 m/s. This 4% difference translates to 4cm error at 1 meter distance. SIREN compensates for this automatically.

### Distance Calculation

The ultrasonic sensor emits a 40kHz pulse and measures how long it takes for the echo to return:

```text
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

### Why refactor from .ino to .h/.cpp?

Initially, I wrote the entire project using multiple `.ino` files in plain C. However, I discovered that the Arduino IDE compiles `.ino` files in alphabetical order and concatenates them into a single file. This created dependency issues and unpredictable behavior when code in one file referenced something defined in another.

After researching solutions, I found that the recommended approach is to have a single `.ino` file for orchestration, with all other code in proper `.h` (header) and `.cpp` (implementation) files. This gives explicit control over compilation order through `#include` directives and follows standard C++ practices.

### Why Object-Oriented Programming with classes?

I considered three approaches:

1. **Procedural C** - Simple functions with global state. Easy to write but leads to scattered state and naming conflicts as the project grows.

2. **Structs with function pointers** - More organized but verbose syntax and manual "method" binding.

3. **C++ Classes** - Clean encapsulation with clear ownership of state and behavior.

I chose classes because they naturally group related data and functions together. Each hardware component becomes a self-contained unit: the `Ultrasonic` class owns its pin configuration and measurement logic, the `Alert` class owns its state and timing. This makes the code easier to understand and modify.

### Why modular design?

The project follows the Single Responsibility Principle (SRP): each module does one thing and does it well. The `Ultrasonic` class only measures distance. The `Alert` class only handles LED and buzzer. The `Scanner` class only orchestrates the sweep. This separation means I can modify or debug one component without affecting others.

### Why centralize constants in config.h?

All pin definitions and shared structures live in `config.h`. This follows the Single Source of Truth (SSOT) principle - there's exactly one place where pin assignments are defined. Without this, pin numbers would be scattered across multiple files, making wiring changes error-prone. With `config.h`, I can see all hardware connections at a glance and change them in one place.

### Why struct with pointer for THReading?

The `THReading` struct holds temperature and humidity data. I pass it by pointer (`THReading*`) rather than by value for two reasons:

1. **Efficiency** - Passing a pointer (2 bytes on Arduino) is cheaper than copying the entire struct (13 bytes: 3 floats + 1 bool).

2. **Mutability** - The `DHTSensor::read()` function needs to modify the struct to return new readings. Passing by pointer allows this without using return values for multiple pieces of data.

I considered alternatives:

- **Global variables** - Would work but creates hidden dependencies and makes testing harder.
- **Return struct by value** - Copies data unnecessarily and doesn't clearly indicate the function modifies its argument.
- **Separate pointer parameters** - Would require `read(float* temp, float* humidity, bool* valid)` which is verbose and error-prone.

The struct-with-pointer approach keeps related data together while remaining efficient.

### Why detach the servo during measurements?

The Servo library and pulseIn() both use timer interrupts. Running them simultaneously causes jitter in timing measurements, leading to distance errors. Detaching the servo temporarily eliminates this interference.

### Why 60ms delay between steps?

The HC-SR04 datasheet recommends at least 60ms between measurements. The SG90 servo also needs time to physically reach each position (~1.7ms per degree). 60ms satisfies both requirements.

### Why cache DHT readings?

The DHT11 requires minimum 1 second between readings and each read blocks for ~25ms. Caching allows us to reuse recent readings without blocking the radar sweep.

### Why manual debouncing instead of a library?

For a CS50 project, implementing debouncing manually demonstrates understanding of timing-based input filtering, rather than hiding it behind a library abstraction.

### Why use F() macro for string literals?

Arduino Uno has only 2KB of RAM but 32KB of Flash memory. By default, string literals like `Serial.println("text")` are copied to RAM at startup and stay there throughout execution. With many debug messages, this quickly consumes precious RAM needed for variables and stack.

The `F()` macro tells the compiler to keep strings in Flash and read them directly when needed. This simple change - `Serial.println(F("text"))` - reduced RAM usage from 643 bytes (31%) to 321 bytes (15%), freeing 322 bytes for runtime operations. The tradeoff is slightly slower string access, but Serial communication is already slow enough that this is imperceptible.

### Why use direct port manipulation instead of digitalWrite()?

The Arduino `digitalWrite()` function is convenient but slow. It performs multiple operations: looking up which register corresponds to the pin, checking for PWM conflicts, disabling interrupts, and finally changing the bit. This takes approximately 6μs per call.

Direct port manipulation like `PORTD |= (1 << 2)` compiles to a single assembly instruction, executing in approximately 0.125μs - about 50 times faster. For the ultrasonic sensor trigger sequence, which requires precise timing, this improved consistency in measurements. The tradeoff is reduced portability (code is specific to ATmega328P) and readability, but for a hardware project targeting a specific board, this is acceptable.

## What I Learned

This project significantly deepened my understanding of Object-Oriented Programming. Coming from CS50's C-based curriculum, I was familiar with structs and pointers, but classes were new territory.

**Public vs Private**: I learned that `private` members are the class's internal state - things the outside world shouldn't touch directly. `public` members are the interface - how other code interacts with the class. For example, in `Alert`, the `active` and `state` variables are private because external code shouldn't manipulate them directly. Only `update()`, `stop()`, and `init()` are public because those are the legitimate ways to control the alert.

**Encapsulation**: Each class hides its complexity. The `Scanner` doesn't know how `Ultrasonic` measures distance - it just calls `getDistance()`. This separation made debugging much easier. When distance readings were wrong, I knew the problem was in `Ultrasonic`, not scattered across the codebase.

**Building on CS50**: The pointer concepts from CS50 (weeks 4-5) directly applied here. Understanding that `THReading* result` means "the address of a THReading struct" made the transition to C++ much smoother. Classes are essentially structs that also contain functions - the mental model I built in CS50 extended naturally.

## AI Assistance Disclosure

As permitted by CS50's final project guidelines, I used AI assistance (Claude by Anthropic) for specific parts of this project:

1. **C++ Syntax for Refactoring** - I understood the logic I wanted to implement but lacked C++ syntax knowledge. AI helped translate my C/.ino code into proper .h/.cpp class structure.

2. **Code Comments** - AI helped write comprehensive comments explaining the physics, timing calculations, and design decisions throughout the codebase.

The core logic, hardware design, architecture decisions, and debugging were my own work. AI served as a syntax reference and documentation aid, not as a problem-solver.

## Output Format

Serial output at 115200 baud, CSV format:

```text
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

## Documentation

- **[Hardware](docs/hardware.md)** - Component specifications and wiring diagrams
