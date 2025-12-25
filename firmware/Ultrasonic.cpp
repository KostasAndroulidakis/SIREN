// Ultrasonic.cpp
// HC-SR04 driver with configurable speed of sound

#include <Arduino.h>
#include "Ultrasonic.h"

// TIMEOUT CALCULATION:
// Maximum distance is 400cm. At slowest reasonable speed of sound
// (~331 m/s at 0°C), round-trip time would be:
//   t = 2 × distance / speed = 2 × 4m / 331m/s = 0.024s = 24ms
// We use 35ms timeout for safety margin.
#define ECHO_TIMEOUT_MS 35

// UNIT CONVERSION:
// Speed of sound is in m/s, duration is in μs, we want cm.
//   distance(cm) = duration(μs) × speed(m/s) × (100cm/1m) × (1s/1000000μs)
//   distance(cm) = duration × speed × 0.0001
#define MICROSEC_TO_CM 0.0001

// TIMER1 INPUT CAPTURE:
// D8 is the ICP1 (Input Capture Pin 1) for Timer1.
// This allows hardware-assisted pulse timing, more accurate
// than software polling used by pulseIn().
//
// Timer1 config: prescaler 8 → 0.5μs per tick at 16MHz
// Max measurable pulse: 65535 × 0.5μs = 32.7ms (enough for 400cm)

void Ultrasonic::init() {
    TRIG_DDR |= (1 << TRIG_BIT);    // TRIG as OUTPUT
    pinMode(ECHO_PIN, INPUT);       // ECHO stays Arduino-style (for clarity)
    TRIG_PORT &= ~(1 << TRIG_BIT);  // TRIG LOW
    Serial.println(F("HC-SR04 initialized (Timer1 IC)"));
}

float Ultrasonic::getDistance(float soundSpeed) {
    // SAVE TIMER1 STATE:
    // Servo library also uses Timer1. We save its configuration
    // and restore it after measurement. This works because
    // servo.detach() is called before this function.
    uint8_t tccr1a_save = TCCR1A;
    uint8_t tccr1b_save = TCCR1B;
    uint8_t timsk1_save = TIMSK1;
    
    // CONFIGURE TIMER1 FOR INPUT CAPTURE:
    // - Normal mode (no PWM)
    // - Prescaler 8: 16MHz / 8 = 2MHz → 0.5μs per tick
    // - Rising edge detection first (ICES1 = 1)
    // - Polling mode (no interrupts)
    TCCR1A = 0;
    TCCR1B = (1 << CS11) | (1 << ICES1);
    TIMSK1 = 0;
    TCNT1 = 0;
    
    // TRIGGER SEQUENCE (from datasheet):
    // 1. Ensure trigger is LOW
    // 2. Send HIGH pulse for at least 10μs
    // 3. Sensor will emit 8 pulses at 40kHz
    TRIG_PORT &= ~(1 << TRIG_BIT);
    delayMicroseconds(2);

    TRIG_PORT |= (1 << TRIG_BIT);
    delayMicroseconds(12);
    TRIG_PORT &= ~(1 << TRIG_BIT);
    
    // WAIT FOR RISING EDGE (pulse start):
    // ICF1 flag is set when edge is captured
    TIFR1 = (1 << ICF1);              // Clear flag by writing 1
    uint32_t startWait = millis();
    
    while (!(TIFR1 & (1 << ICF1))) {
        if (millis() - startWait > ECHO_TIMEOUT_MS) {
            TCCR1A = tccr1a_save;     // Restore Timer1
            TCCR1B = tccr1b_save;
            TIMSK1 = timsk1_save;
            return -1;                 // Timeout - no echo
        }
    }
    uint16_t pulseStart = ICR1;        // Capture timestamp
    
    // SWITCH TO FALLING EDGE:
    TCCR1B &= ~(1 << ICES1);           // ICES1 = 0 → falling edge
    TIFR1 = (1 << ICF1);               // Clear flag
    
    // WAIT FOR FALLING EDGE (pulse end):
    while (!(TIFR1 & (1 << ICF1))) {
        if (millis() - startWait > ECHO_TIMEOUT_MS) {
            TCCR1A = tccr1a_save;
            TCCR1B = tccr1b_save;
            TIMSK1 = timsk1_save;
            return -1;
        }
    }
    uint16_t pulseEnd = ICR1;
    
    // RESTORE TIMER1:
    TCCR1A = tccr1a_save;
    TCCR1B = tccr1b_save;
    TIMSK1 = timsk1_save;
    
    // CALCULATE PULSE DURATION:
    // Handle potential timer overflow during measurement
    uint16_t ticks;
    if (pulseEnd >= pulseStart) {
        ticks = pulseEnd - pulseStart;
    } else {
        ticks = (65535 - pulseStart) + pulseEnd + 1;
    }
    
    // Convert ticks to μs: ticks × 0.5μs/tick
    float duration = ticks * 0.5f;
    
    // DISTANCE FORMULA:
    //   distance = (duration × speed × unit_conversion) / 2
    //
    // Division by 2 because sound travels:
    //   sensor → object → sensor (round trip = 2× actual distance)
    float distance = (duration * soundSpeed * MICROSEC_TO_CM) / 2;
    
    // Validate against sensor's reliable range
    if (distance < MIN_DISTANCE || distance > MAX_DISTANCE) {
        return -1;
    }
    
    return distance;
}