// Alert.cpp
// Proximity Alert System Implementation

#include <Arduino.h>
#include "Alert.h"

// Direct port manipulation for LED
// D13 = PORTB bit 5 (13 - 8 = 5)
#define LED_BIT 5

// ZONE THRESHOLDS
#define ALERT_THRESHOLD 100.0   // cm - below this, warning zone starts
#define DANGER_THRESHOLD 10.0   // cm - below this, constant alarm

// BPM SETTINGS
#define BASE_BPM 60             // Starting BPM at alert threshold (100cm)
#define MS_PER_MINUTE 60000     // Milliseconds in one minute

// BUZZER FREQUENCY
// Passive buzzers need a frequency to vibrate. 2kHz is in the
// range where human hearing is most sensitive (2-4kHz).
#define BUZZER_FREQ 2000

void Alert::init() {
    pinMode(LED_PIN, OUTPUT);
    pinMode(BUZZER_PIN, OUTPUT);
    PORTB &= ~(1 << 5);   // LED LOW (D13 = PORTB bit 5)
    noTone(BUZZER_PIN);
    active = false;
    state = false;
    lastToggle = 0;
    Serial.println(F("Alert system initialized"));
}

// Calculate milliseconds between toggles for given distance
// We toggle twice per beat (on and off), hence /2
unsigned int Alert::getIntervalMs(float distance) {
    // BPM increases by 1 for each cm closer
    // At 100cm: 60 BPM, at 11cm: 149 BPM
    int bpm = BASE_BPM + 2 * (int)(ALERT_THRESHOLD - distance);
    return MS_PER_MINUTE / bpm / 2;  // /2 because we toggle twice per beat
}

void Alert::update(float distance) {
    // INVALID READING HANDLING:
    // If sensor returns -1 (no echo/out of range), we stop the alert.
    // This is the conservative approach - don't alarm if we can't measure.
    // Alternative would be to maintain last state, but that could cause
    // false alarms if sensor temporarily fails.
    if (distance < 0) {
        stop();
        return;
    }
    
    // SAFE ZONE: Object beyond threshold
    if (distance > ALERT_THRESHOLD) {
        stop();
        return;
    }
    
    // DANGER ZONE: Object very close - constant alarm
    // No blinking, no timing - just full alert
    if (distance <= DANGER_THRESHOLD) {
        active = true;
        state = true;
        PORTB |= (1 << 5);    // LED HIGH
        tone(BUZZER_PIN, BUZZER_FREQ);
        return;
    }
    
    // WARNING ZONE: Object in range (10 < distance <= 100)
    // Blink/beep at rate proportional to proximity
    active = true;
    unsigned long now = millis();
    unsigned int interval = getIntervalMs(distance);
    
    // NON-BLOCKING TOGGLE:
    // Instead of using delay(), we check if enough time has passed.
    // This allows the main loop to continue running smoothly.
    if (now - lastToggle >= interval) {
        lastToggle = now;
        state = !state;

        if (state) {
            PORTB |= (1 << 5);    // LED HIGH
        } else {
            PORTB &= ~(1 << 5);   // LED LOW
        }
        
        // Buzzer follows LED state
        if (state) {
            tone(BUZZER_PIN, BUZZER_FREQ);
        } else {
            noTone(BUZZER_PIN);
        }
    }
}

void Alert::stop() {
    // Only act if currently active (avoid unnecessary writes)
    if (active) {
        active = false;
        state = false;
        PORTB &= ~(1 << 5);   // LED LOW
        noTone(BUZZER_PIN);
    }
}