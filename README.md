# SIREN

Sonic Imaging for Range Exploration and Navigation

![SIREN Logo](siren-logo.png)

#### Video Demo

[YouTube Video](https://youtu.be/8TxDGz97N5Q)

#### Description

SIREN is an ultrasonic radar system built with Arduino that detects objects within a 180-degree field of view. The system combines multiple sensors to achieve accurate distance measurements by compensating for environmental conditions in real-time.

## Why This Project?

I'm 37, a music producer with an audio engineering background, and I'm preparing to study Electrical & Computer Engineering. Before CS50, I had earned Python certifications (PCEP, PCAP, freeCodeCamp) and bought an Elegoo starter kit to teach myself C and embedded basics.

I chose to build a radar for two reasons. First, I've always been drawn to military technology. Second, I wanted to go deeper than Arduino tutorials usually go. Most examples use `digitalWrite()` and `delay()` without explaining what actually happens at the hardware level. I wanted to understand what's under the hood.

This project forced me to read datasheets, manipulate registers directly, and think about timing at the microsecond level. It's the kind of challenge I was looking for.

## What It Does

The system rotates an ultrasonic sensor using a servo motor, sweeping from 10 to 170 degrees and back. At each angle, it measures the distance to the nearest object and transmits the data over serial connection. When an object comes within 100cm, an alert system activates with LED and buzzer feedback that increases in urgency as the object gets closer.

A temperature and humidity sensor monitors air conditions to calculate the actual speed of sound, rather than assuming a fixed 343 m/s. This can improve measurement accuracy by up to 5% in varying conditions.

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

**Ultrasonic.h / Ultrasonic.cpp** - Driver for the HC-SR04 sensor. Uses Timer1 Input Capture for precise pulse timing. Includes timeout handling for out-of-range objects.

**DHTSensor.h / DHTSensor.cpp** - Non-blocking driver for the DHT11 sensor. Uses a caching pattern to avoid blocking the main loop: readings are cached and only updated every 2 seconds.

**SpeedOfSound.h / SpeedOfSound.cpp** - Calculates the speed of sound based on current temperature and humidity.

### Actuator Modules

**Servo.h / Servo.cpp** - Wrapper for the SG90 servo motor. Implements detach/attach pattern because both Servo library and our ultrasonic code use Timer1.

**Alert.h / Alert.cpp** - Manages the LED and buzzer alert system. Three zones:

- **Safe (>100cm)**: No alert
- **Warning (10-100cm)**: Blinking/beeping at 60-238 BPM, faster as objects get closer
- **Danger (≤10cm)**: Continuous LED and buzzer

### Input Module

**Button.h / Button.cpp** - Debounced button input for start/stop control. Implements software debouncing with 50ms delay to filter mechanical contact bounce.

### Orchestration

**Scanner.h / Scanner.cpp** - Coordinates the scanning process. Performs bidirectional sweeps (10→170→10) and outputs data in CSV format.

## Output Format

Serial output at 115200 baud, CSV format:

```text
angle,distance,humidity,temperatureC,temperatureF
0,45.23,52.00,24.30,75.74
1,45.18,52.00,24.30,75.74
...
```

Distance of `-1` indicates no object detected or out-of-range reading.

## Design Decisions

### Code Organization

Initially, I wrote everything in multiple `.ino` files. Bad idea. The Arduino IDE compiles them alphabetically and concatenates them, which created dependency nightmares. After some frustration, I refactored to a single `.ino` with proper `.h/.cpp` pairs. This gave me explicit control over compilation order and forced me to think about interfaces.

I went with C++ classes over plain C structs because they naturally group related data and functions. Each hardware component is self-contained: `Ultrasonic` owns its pin configuration and measurement logic, `Alert` owns its state and timing. The code follows Single Responsibility Principle — each module does one thing well. All pin definitions live in `config.h` (Single Source of Truth), so wiring changes happen in one place.

### Bare-Metal Optimizations

The Arduino framework is convenient but hides a lot. I wanted to see what's underneath, so I replaced several abstractions with direct hardware access:

**Direct port manipulation** - `digitalWrite()` takes ~6μs because it does pin lookups, PWM checks, and interrupt handling. Direct register access like `PORTD |= (1 << 2)` compiles to one instruction at ~0.06μs. For the ultrasonic trigger sequence, this consistency matters.

**Timer1 Input Capture** - The standard `pulseIn()` polls in a software loop, introducing ±4μs jitter. Timer1's Input Capture feature records timestamps in hardware, giving ±0.5μs resolution. This required rewiring ECHO to pin D8 (the ICP1 pin) and understanding how to save/restore timer configuration since the Servo library also uses Timer1.

**PINB toggle trick** - Writing to PINx toggles the corresponding PORTx bit. Instead of an if/else for LED blinking, `PINB = (1 << 5)` does the job in one instruction.

**Custom main()** - Arduino's hidden `main()` calls `setup()`, then loops calling `loop()` and `serialEventRun()`. I wrote my own `main()` to know exactly what runs and when. The performance gain is negligible, but the transparency matters to me.

### Other Choices

**constexpr over #define** - Type-safe constants catch bugs that preprocessor macros miss.

**F() macro for strings** - Keeps string literals in Flash instead of copying to RAM. Reduced RAM usage from 643 bytes to 321 bytes.

**Servo detach during measurement** - Both Servo library and Timer1 Input Capture need Timer1. Detaching the servo frees the timer for accurate pulse measurement.

**60ms step delay** - HC-SR04 datasheet recommends 60ms between measurements. The servo also needs time to physically move. 60ms satisfies both.

**DHT caching** - DHT11 requires 1 second between reads and blocks for ~25ms. Caching lets the radar sweep continue smoothly.

**10°-170° servo range** - The SG90 is rated for 0-180° but cheap clones (like those in the Elegoo kit) often have mechanical stops before reaching the extremes. Pushing against these stops causes whining, overheating, and eventual gear damage. Using 10-170° avoids this with minimal loss of coverage.

## What I Learned

Coming into this project, I knew Python reasonably well and had played with Arduino tutorials, but I'd never gone this deep into embedded systems.

The biggest time sink was figuring out how to organize the code. I obsessed over keeping things modular — Single Responsibility, Single Source of Truth — and it paid off during debugging. When distance readings were wrong, I knew the problem was in `Ultrasonic.cpp`, not scattered across the codebase. But getting there meant refactoring from `.ino` spaghetti to proper `.h/.cpp` structure, which meant learning C++ syntax I'd never seen before.

The embedded concepts were the real education: GPIO, registers, interrupts, timers, serial protocols. I finally understand what `PORTB |= (1 << 5)` actually does — it's bit manipulation at a memory-mapped I/O address, same pointer concepts from CS50 weeks 4-5 but applied to hardware. Timer1 Input Capture was particularly satisfying to figure out. The datasheet was dense, but once it clicked, I understood how hardware can do precise timing independently of the CPU.

I also learned some C++ along the way. Classes, public/private, constructors, dependency injection. It's not deep knowledge, but enough to write modular embedded code.

The audio engineering background helped more than expected. Years of working with signal flow, timing, and understanding how sound behaves made the physics intuitive. Calculating speed of sound compensation felt natural.

## AI Assistance Disclosure

As permitted by CS50's final project guidelines, I used AI assistance (Claude by Anthropic) for:

1. **C++ Syntax** - I knew what I wanted to implement but not the C++ syntax. AI helped translate my logic into proper class structure.

2. **Code Review** - AI reviewed for bugs and suggested bare-metal optimizations like Timer1 Input Capture.

The architecture, hardware design, debugging, and problem-solving were my own work.

## Future Improvements

- Visualization

## Documentation

- **[Hardware](docs/hardware.md)** - Component specifications and wiring diagrams
