// config.h

#ifndef CONFIG_H
#define CONFIG_H

// ============================================
// PIN DEFINITIONS
// ============================================

// Ultrasonic Sensor HC-SR04
#define TRIG_PIN 2
#define ECHO_PIN 3

// Servo Motor SG90
#define SERVO_PIN 9

// Temperature & Humidity Sensor DHT11
#define DHT_PIN 4
#define DHT_TYPE DHT11

// Passive Buzzer
#define BUZZER_PIN 8

// Red LED
#define LED_PIN 13

// Button
#define BUTTON_PIN 6

// ============================================
// SENSOR CONSTANTS
// ============================================

// Ultrasonic
#define MIN_DISTANCE 2          // Minimum distance in cm
#define MAX_DISTANCE 400        // Maximum distance in cm

// Servo
#define SERVO_MIN_ANGLE 0
#define SERVO_MAX_ANGLE 180
#define SERVO_STEP 1
#define SERVO_DELAY 60          // Delay between steps (ms)

// DHT11
#define DHT_TYPE DHT11

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