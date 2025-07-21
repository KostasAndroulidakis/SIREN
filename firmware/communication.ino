/**
 * @file communication.ino
 * @brief Serial communication module for SIREN system
 * @author KostasAndroulidakis
 * @date 2025
 *
 * Handles all serial communication protocols for transmitting sonar data
 * to external systems. Provides a standardized data format for angle
 * and distance measurements.
 */

// Communication configuration constants
const int SERIAL_BAUD_RATE = 9600;   ///< Reliable serial communication (Arduino standard)

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
 * @brief Send sonar measurement data via serial
 * @param angle The servo angle in degrees (0-180)
 * @param distance The measured distance in centimeters
 *
 * Transmits formatted sonar data in human-readable format.
 * Example output: "Angle: 90 - Distance: 150"
 */
void sendSonarData(int angle, int distance) {
  Serial.print("Angle: ");
  Serial.print(angle);
  Serial.print(" - Distance: ");
  Serial.println(distance);
}
