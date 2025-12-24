// Alert.h

#ifndef ALERT_H
#define ALERT_H

#include "config.h"

class Alert {
public:
    void init();
    void update(float distance);  // Call every loop iteration
    void stop();
    
private:
    bool active;
    bool state;  // LED/Buzzer on or off
    unsigned long lastToggle;
    unsigned int getIntervalMs(float distance);
};

#endif