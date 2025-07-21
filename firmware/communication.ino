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

/**
 * @brief Send enhanced sonar data with environmental information
 * @param angle The servo angle in degrees (0-180)
 * @param distance The measured distance in centimeters
 * @param temperature Temperature in Celsius
 * @param humidity Humidity percentage
 *
 * Transmits comprehensive sensor data including environmental conditions.
 * Example output: "Angle: 90 - Distance: 150 - Temp: 23.5 - Humidity: 65.2"
 */
void sendEnhancedSonarData(int angle, int distance, float temperature, float humidity) {
  Serial.print("Angle: ");
  Serial.print(angle);
  Serial.print(" - Distance: ");
  Serial.print(distance);
  Serial.print(" - Temp: ");
  Serial.print(temperature, 1);  // 1 decimal place precision
  Serial.print(" - Humidity: ");
  Serial.println(humidity, 1);   // 1 decimal place precision
}

/**
 * @brief Send calibrated sonar data with real-time sound speed
 * @param angle The servo angle in degrees (0-180)
 * @param distance The calibrated distance in centimeters
 * @param temperature Temperature in Celsius
 * @param humidity Humidity percentage
 * @param soundSpeed Real-time sound speed in cm/microsecond
 *
 * MISRA C++ compliant transmission of calibrated sonar measurements.
 * Example output: "Angle: 90 - Distance: 150 - Temp: 23.5 - Humidity: 65.2 - SoundSpeed: 0.03456"
 */
void sendCalibratedSonarData(int angle, int distance, float temperature, float humidity, float soundSpeed) {
  Serial.print("Angle: ");
  Serial.print(angle);
  Serial.print(" - Distance: ");
  Serial.print(distance);
  Serial.print(" - Temp: ");
  Serial.print(temperature, 1);      // 1 decimal place precision
  Serial.print(" - Humidity: ");
  Serial.print(humidity, 1);         // 1 decimal place precision
  Serial.print(" - SoundSpeed: ");
  Serial.println(soundSpeed, 5);     // 5 decimal places for sound speed precision
}

/**
 * @brief Send environmental data only
 * @param temperature Temperature in Celsius  
 * @param humidity Humidity percentage
 *
 * Transmits environmental sensor data for system monitoring.
 * Example output: "Temp: 23.5 - Humidity: 65.2"
 */
void sendEnvironmentalData(float temperature, float humidity) {
  Serial.print("Temp: ");
  Serial.print(temperature, 1);
  Serial.print(" - Humidity: ");
  Serial.println(humidity, 1);
}
