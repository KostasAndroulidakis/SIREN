// Button.cpp
// Debounced Push Button Implementation

#include <Arduino.h>
#include "Button.h"

// DEBOUNCE TIMING:
// 50ms is a safe value that filters most mechanical bounce
// while still feeling responsive to the user.
// Typical bounce duration: 1-20ms
// Human perception threshold: ~100ms
#define DEBOUNCE_DELAY 50  // ms

void Button::init() {
    // INPUT_PULLUP enables internal ~20kΩ pull-up resistor
    // This means: released = HIGH, pressed = LOW
    // No external resistor needed in the circuit
    pinMode(BUTTON_PIN, INPUT_PULLUP);  // Active LOW
    lastState = HIGH;           // Assume button starts released
    lastDebounceTime = 0;
    Serial.println(F("Button initialized")); 
}

bool Button::isPressed() {
    bool reading = (PIND & (1 << 6));  // D6 = PORTD bit 6, inverted for active LOW
    unsigned long now = millis();
    
    // EDGE DETECTION:
    // We only care about HIGH→LOW transitions (button press)
    // LOW→HIGH (button release) is ignored
    if (lastState == HIGH && reading == LOW) {

        // DEBOUNCE CHECK:
        // Has enough time passed since last valid press?
        if (now - lastDebounceTime > DEBOUNCE_DELAY) {
            lastDebounceTime = now;
            lastState = reading;
            return true;    // Valid press detected
        }

        // Within debounce period - ignore this press.
        // We don't update lastState here so that we can
        // detect a genuine new press after cooldown.
        return false;
    }
    
    // No transition detected (or it's a release) - safe to update state
    lastState = reading;
    return false;
}