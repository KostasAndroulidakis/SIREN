// Button.h

#ifndef BUTTON_H
#define BUTTON_H

#include "config.h"

class Button {
public:
    void init();
    bool isPressed();      // Returns true once per press (debounced)
    
private:
    bool lastState;
    unsigned long lastDebounceTime;
};

#endif