// Button.cpp

#include <Arduino.h>
#include "Button.h"

#define DEBOUNCE_DELAY 50  // ms

void Button::init() {
    pinMode(BUTTON_PIN, INPUT_PULLUP);  // Active LOW
    lastState = HIGH;
    lastDebounceTime = 0;
}

bool Button::isPressed() {
    bool reading = digitalRead(BUTTON_PIN);
    unsigned long now = millis();
    
    // Button pressed (HIGH -> LOW transition)
    if (lastState == HIGH && reading == LOW) {
        if (now - lastDebounceTime > DEBOUNCE_DELAY) {
            lastDebounceTime = now;
            lastState = reading;
            return true;
        }
        return false;  // Within debounce period - don't update lastState yet
    }
    
    lastState = reading;
    return false;
}