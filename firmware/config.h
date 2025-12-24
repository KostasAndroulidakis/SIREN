// config.h
// Central configuration for ultrasonic radar system
// All pin definitions and shared structs

#ifndef CONFIG_H
#define CONFIG_H

// ============================================
// PIN DEFINITIONS
// ============================================

#define TRIG_PIN 2      // Ultrasonic Sensor HC-SR04
#define ECHO_PIN 3

#define SERVO_PIN 9     // Servo Motor SG90

#define DHT_PIN 4       // Temperature & Humidity Sensor DHT11

#define BUZZER_PIN 8    // Passive Buzzer

#define LED_PIN 13      // Red LED

#define BUTTON_PIN 6    // Button

// ============================================
// TIMING CONSTANTS
// ============================================

#define SERIAL_BAUD 115200

// ============================================
// STRUCTS
// ============================================

// Temperature & Humidity reading structure
typedef struct {
    float humidity;       // Relative humidity (%)
    float temperatureC;   // Temperature in Celsius
    float temperatureF;   // Temperature in Fahrenheit
    bool valid;           // Reading successful flag
} THReading;

#endif