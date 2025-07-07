/**
 * @file serial_interface.hpp
 * @brief Military-grade serial communication interface for Arduino radar
 * @author unoRadar Project
 * @date 2025
 *
 * Handles high-performance serial communication with Arduino UNO R3.
 * Implements protocol: "Angle: X - Distance: Y" at 9600 baud.
 */

#pragma once

#include <memory>
#include <string>
#include <functional>
#include <atomic>
#include <mutex>
#include <boost/asio.hpp>

#include "data/radar_types.hpp"
#include "serial/arduino_protocol_parser.hpp"

namespace unoradar::serial {

/**
 * @brief Serial communication interface for Arduino radar system
 *
 * Military-grade serial interface with:
 * - Asynchronous I/O operations
 * - Real-time data parsing
 * - Error recovery and reconnection
 * - Performance monitoring
 */
class SerialInterface {
public:
    /// Connection states
    enum class ConnectionState {
        DISCONNECTED,   ///< No connection
        CONNECTING,     ///< Connection in progress
        CONNECTED,      ///< Active connection
        RECONNECTING,   ///< Attempting reconnection
        ERROR           ///< Connection error
    };

    /// Data callback function type
    using DataCallback = std::function<void(const data::RadarDataPoint&)>;

    /// Error callback function type
    using ErrorCallback = std::function<void(const std::string&, data::ErrorSeverity)>;

    /**
     * @brief Constructor
     * @param io_context Boost.Asio I/O context for async operations
     */
    explicit SerialInterface(boost::asio::io_context& io_context);

    /**
     * @brief Destructor - ensures clean shutdown
     */
    ~SerialInterface();

    // Non-copyable, non-movable
    SerialInterface(const SerialInterface&) = delete;
    SerialInterface& operator=(const SerialInterface&) = delete;
    SerialInterface(SerialInterface&&) = delete;
    SerialInterface& operator=(SerialInterface&&) = delete;

    /**
     * @brief Initialize serial interface
     * @param port_name Serial port name (e.g., "/dev/tty.usbmodem14101")
     * @return true if initialization successful
     */
    bool initialize(const std::string& port_name);

    /**
     * @brief Start serial communication
     * @return true if started successfully
     */
    bool start();

    /**
     * @brief Stop serial communication gracefully
     */
    void stop();

    /**
     * @brief Check if connected to Arduino
     */
    bool isConnected() const noexcept;

    /**
     * @brief Get current connection state
     */
    ConnectionState getConnectionState() const noexcept;

    /**
     * @brief Set data received callback
     * @param callback Function to call when radar data is received
     */
    void setDataCallback(DataCallback callback);

    /**
     * @brief Set error callback
     * @param callback Function to call when errors occur
     */
    void setErrorCallback(ErrorCallback callback);

    /**
     * @brief Get serial communication statistics
     */
    data::SerialStatistics getStatistics() const;

    /**
     * @brief Send command to Arduino (for debugging/control)
     * @param command Command string to send
     */
    void sendCommand(const std::string& command);

    /**
     * @brief Auto-detect Arduino port
     * @return Port name if found, empty string if not found
     */
    static std::string autoDetectArduinoPort();

private:
    // Core components
    boost::asio::io_context& io_context_;
    std::unique_ptr<boost::asio::serial_port> serial_port_;
    std::unique_ptr<boost::asio::steady_timer> reconnect_timer_;

    // State management
    std::atomic<ConnectionState> connection_state_;
    std::atomic<bool> shutdown_requested_;
    std::string port_name_;

    // Data handling
    static constexpr size_t BUFFER_SIZE = 256;
    std::array<char, BUFFER_SIZE> read_buffer_;
    std::string message_buffer_;
    std::unique_ptr<ArduinoProtocolParser> protocol_parser_;

    // Callbacks
    DataCallback data_callback_;
    ErrorCallback error_callback_;

    // Statistics and monitoring
    mutable std::mutex stats_mutex_;
    data::SerialStatistics statistics_;
    std::chrono::steady_clock::time_point last_data_time_;
    std::chrono::steady_clock::time_point connection_start_time_;

    /**
     * @brief Configure serial port settings
     */
    bool configurePort();

    /**
     * @brief Start asynchronous read operation
     */
    void startAsyncRead();

    /**
     * @brief Handle received data
     * @param error Boost.System error code
     * @param bytes_transferred Number of bytes received
     */
    void handleRead(const boost::system::error_code& error, std::size_t bytes_transferred);

    /**
     * @brief Process complete message from Arduino
     * @param message Complete message string
     */
    void processMessage(const std::string& message);

    /**
     * @brief Handle connection errors and attempt recovery
     * @param error_message Error description
     * @param severity Error severity level
     */
    void handleConnectionError(const std::string& error_message,
                              data::ErrorSeverity severity);

    /**
     * @brief Attempt to reconnect to Arduino
     */
    void attemptReconnection();

    /**
     * @brief Reconnection timer callback
     * @param error Timer error code
     */
    void onReconnectTimer(const boost::system::error_code& error);

    /**
     * @brief Update connection state atomically
     * @param new_state New connection state
     */
    void updateConnectionState(ConnectionState new_state);

    /**
     * @brief Update communication statistics
     */
    void updateStatistics();


    /**
     * @brief Get list of available serial ports
     * @return Vector of port names
     */
    static std::vector<std::string> getAvailablePorts();

    /**
     * @brief Check if port is Arduino device
     * @param port_name Port to check
     * @return true if likely Arduino device
     */
    static bool isArduinoPort(const std::string& port_name);
};

} // namespace unoradar::serial