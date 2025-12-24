// Alert.cpp

#include <Arduino.h>
#include "Alert.h"

#define ALERT_THRESHOLD 100.0  // cm - start warning
#define DANGER_THRESHOLD 10.0  // cm - constant alarm
#define BASE_BPM 60            // BPM at 100cm
#define MS_PER_MINUTE 60000
#define BUZZER_FREQ 2000       // Hz - audible frequency for passive buzzer

void Alert::init() {
    pinMode(LED_PIN, OUTPUT);
    pinMode(BUZZER_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);
    noTone(BUZZER_PIN);
    active = false;
    state = false;
    lastToggle = 0;
}

unsigned int Alert::getIntervalMs(float distance) {
    // BPM increases by 1 for each cm closer
    // At 100cm: 60 BPM, at 11cm: 149 BPM
    int bpm = BASE_BPM + (int)(ALERT_THRESHOLD - distance);
    return MS_PER_MINUTE / bpm / 2;  // /2 because we toggle twice per beat
}

void Alert::update(float distance) {
    // Invalid reading - stop alert (conservative approach)
    if (distance < 0) {
        stop();
        return;
    }
    
    // Safe zone - no alert
    if (distance > ALERT_THRESHOLD) {
        stop();
        return;
    }
    
    // Danger zone - constant alarm
    if (distance <= DANGER_THRESHOLD) {
        active = true;
        state = true;
        digitalWrite(LED_PIN, HIGH);
        tone(BUZZER_PIN, BUZZER_FREQ);
        return;
    }
    
    // Warning zone (10 < distance <= 100) - blink/beep at increasing rate
    active = true;
    unsigned long now = millis();
    unsigned int interval = getIntervalMs(distance);
    
    if (now - lastToggle >= interval) {
        lastToggle = now;
        state = !state;
        digitalWrite(LED_PIN, state);
        if (state) {
            tone(BUZZER_PIN, BUZZER_FREQ);
        } else {
            noTone(BUZZER_PIN);
        }
    }
}

void Alert::stop() {
    if (active) {
        active = false;
        state = false;
        digitalWrite(LED_PIN, LOW);
        noTone(BUZZER_PIN);
    }
}