// Button.h
// Debounced Push Button Input
//
// PURPOSE:
// Provides clean, single-trigger button press detection.
// Filters out mechanical contact bounce that would cause
// multiple false triggers from a single press.
//
// THE BOUNCE PROBLEM:
// When a mechanical button is pressed, the metal contacts
// don't make clean contact immediately. They "bounce" -
// connecting and disconnecting rapidly for 1-50ms before
// settling. Without debouncing, one press might register
// as 5-10 presses.
//
//   Physical press:     ___########___
//   Electrical signal:  ___##_#_##_#___  (bouncing)
//   After debounce:     ___########___   (clean)
//
// DEBOUNCE STRATEGY:
// We use a 50ms cooldown after detecting a transition.
// Any transitions during cooldown are ignored.
// The press event is preserved until cooldown expires.
//
// ACTIVE LOW CONFIGURATION:
// Button uses INPUT_PULLUP, meaning:
//   - Released: pin reads HIGH (pulled up internally)
//   - Pressed:  pin reads LOW  (connected to ground)

#ifndef BUTTON_H
#define BUTTON_H

#include "config.h"

class Button {
public:
    void init();
    bool isPressed();      // Returns true ONCE per press (edge-triggered, debounced)
    
private:
    bool lastState;                 // Previous reading (HIGH = released, LOW = pressed)
    unsigned long lastDebounceTime; // Timestamp of last valid transition
};

#endif