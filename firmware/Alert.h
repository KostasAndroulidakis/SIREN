// Alert.h
// Proximity Alert System (LED + Buzzer)
//
// PURPOSE:
// Provides audiovisual feedback when objects are detected within
// the alert threshold. Urgency increases as objects get closer.
//
// THREE-ZONE SYSTEM:
//   Safe zone (>100cm):      No alert - LED off, buzzer silent
//   Warning zone (10-100cm): Pulsing alert - rate increases with proximity
//   Danger zone (≤10cm):     Constant alert - LED solid, buzzer continuous
//
// BPM CALCULATION:
// In warning zone, alert rate follows: BPM = 60 + (100 - distance)
//   At 100cm: 60 BPM  (1 beep per second)
//   At 50cm:  110 BPM (almost 2 beeps per second)
//   At 11cm:  149 BPM (2.5 beeps per second)
//
// PASSIVE BUZZER NOTE:
// Passive buzzers require a frequency signal to produce sound.
// We use tone() at 2kHz for maximum audibility.
// Active buzzers would use digitalWrite() instead.

#ifndef ALERT_H
#define ALERT_H

#include "config.h"

class Alert {
public:
    void init();
    void update(float distance);    // Call every loop - handles timing internally
    void stop();                    // Force stop (used when scanning stops)
    
private:
    bool active;                    // Is alert currently running?
    bool state;                     // Current toggle state (on/off)
    unsigned long lastToggle;       // Timestamp of last state change
    unsigned int getIntervalMs(float distance);  // Calculate toggle interval from distance
};

#endif