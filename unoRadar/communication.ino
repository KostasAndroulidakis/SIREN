/**
 * @file communication.ino
 * @brief Serial communication module for unoRadar system
 * @author unoRadar Project
 * @date 2025
 * 
 * Handles all serial communication protocols for transmitting radar data
 * to external systems. Provides a standardized data format for angle
 * and distance measurements.
 */

// Communication configuration constants
const int SERIAL_BAUD_RATE = 9600; ///< Serial communication baud rate

// Communication protocol constants
const char DATA_SEPARATOR = ',';  ///< Character separating angle and distance values
const char DATA_TERMINATOR = '.'; ///< Character marking end of data transmission

/**
 * @brief Initialize serial communication
 * 
 * Configures the serial port with the system's standard baud rate.
 * Must be called before any serial communication attempts.
 */
void initCommunication() {
  Serial.begin(SERIAL_BAUD_RATE);
}

/**
 * @brief Send radar measurement data via serial
 * @param angle The servo angle in degrees (0-180)
 * @param distance The measured distance in centimeters
 * 
 * Transmits formatted radar data using the protocol: "angle,distance."
 * Example output: "90,150." represents 90 degrees, 150cm distance
 */
void sendRadarData(int angle, int distance) {
  Serial.print(angle);
  Serial.print(DATA_SEPARATOR);
  Serial.print(distance);
  Serial.print(DATA_TERMINATOR);
}