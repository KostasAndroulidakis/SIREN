// config.h
// Central configuration for ultrasonic radar system
// All pin definitions and shared structs

#ifndef CONFIG_H
#define CONFIG_H

// ============================================
// PIN DEFINITIONS
// ============================================
// Using constexpr for type safety and compiler optimization.
// uint8_t is sufficient for pin numbers (0-255).

static constexpr uint8_t TRIG_PIN = 2;      // Ultrasonic Sensor HC-SR04
static constexpr uint8_t ECHO_PIN = 8;

// Direct port manipulation for TRIG (D2 = PORTD bit 2)
#define TRIG_PORT PORTD
#define TRIG_DDR  DDRD
#define TRIG_BIT  2

static constexpr uint8_t SERVO_PIN = 9;     // Servo Motor SG90

static constexpr uint8_t DHT_PIN = 4;       // Temperature & Humidity Sensor DHT11

static constexpr uint8_t BUZZER_PIN = 3;    // Passive Buzzer

static constexpr uint8_t LED_PIN = 13;      // Red LED

static constexpr uint8_t BUTTON_PIN = 6;    // Button

// ============================================
// TIMING CONSTANTS
// ============================================

static constexpr uint32_t SERIAL_BAUD = 115200;

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