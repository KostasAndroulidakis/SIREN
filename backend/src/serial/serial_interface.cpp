/**
 * @file serial_interface.cpp
 * @brief Implementation of military-grade serial communication interface
 * @author SIREN Project
 * @date 2025
 *
 * High-performance Arduino communication with real-time data parsing.
 */

#include "serial/serial_interface.hpp"
#include "constants/communication.hpp"
#include "constants/performance.hpp"
#include "constants/hardware.hpp"
#include <iostream>
#include <chrono>
#include <filesystem>
#include <algorithm>

namespace siren::serial {

namespace constants = siren::constants;

SerialInterface::SerialInterface(boost::asio::io_context& io_context)
    : io_context_(io_context)
    , serial_port_(nullptr)
    , reconnect_timer_(nullptr)
    , connection_state_(ConnectionState::DISCONNECTED)
    , shutdown_requested_(false)
    , protocol_parser_(std::make_unique<ArduinoProtocolParser>())
    , last_data_time_(std::chrono::steady_clock::now())
    , connection_start_time_(std::chrono::steady_clock::now())
{
    std::cout << "[SerialInterface] Initializing military-grade serial communication..." << std::endl;

    // Initialize statistics
    statistics_ = data::SerialStatistics{};
}

SerialInterface::~SerialInterface() {
    if (connection_state_.load() != ConnectionState::DISCONNECTED) {
        stop();
    }
}

bool SerialInterface::initialize(const std::string& port_name) {
    try {
        port_name_ = port_name;

        std::cout << "[SerialInterface] Initializing port: " << port_name << std::endl;

        // Create serial port
        serial_port_ = std::make_unique<boost::asio::serial_port>(io_context_);

        // Create reconnect timer
        reconnect_timer_ = std::make_unique<boost::asio::steady_timer>(io_context_);

        std::cout << "[SerialInterface] ✅ Initialization complete" << std::endl;
        return true;

    } catch (const std::exception& e) {
        handleConnectionError("Initialization failed: " + std::string(e.what()),
                            data::ErrorSeverity::CRITICAL);
        return false;
    }
}

bool SerialInterface::start() {
    try {
        updateConnectionState(ConnectionState::CONNECTING);

        std::cout << "[SerialInterface] 🚀 Opening serial port: " << port_name_ << std::endl;

        // Open the serial port
        serial_port_->open(port_name_);

        if (!serial_port_->is_open()) {
            handleConnectionError("Failed to open serial port", data::ErrorSeverity::ERROR);
            return false;
        }

        // Configure port settings
        if (!configurePort()) {
            handleConnectionError("Failed to configure serial port", data::ErrorSeverity::ERROR);
            return false;
        }

        // Update state and start reading
        updateConnectionState(ConnectionState::CONNECTED);
        connection_start_time_ = std::chrono::steady_clock::now();

        // Clear any existing data in the buffer
        message_buffer_.clear();

        // Start asynchronous reading
        startAsyncRead();

        std::cout << "[SerialInterface] ✅ Serial communication started - Ready for Arduino data" << std::endl;
        return true;

    } catch (const std::exception& e) {
        handleConnectionError("Start failed: " + std::string(e.what()),
                            data::ErrorSeverity::CRITICAL);
        return false;
    }
}

void SerialInterface::stop() {
    std::cout << "[SerialInterface] 🛑 Stopping serial communication..." << std::endl;

    shutdown_requested_.store(true);

    // Cancel reconnect timer
    if (reconnect_timer_) {
        reconnect_timer_->cancel();
    }

    // Close serial port
    if (serial_port_ && serial_port_->is_open()) {
        try {
            serial_port_->cancel();
            serial_port_->close();
        } catch (const std::exception& e) {
            std::cout << "[SerialInterface] ⚠️ Error closing port: " << e.what() << std::endl;
        }
    }

    updateConnectionState(ConnectionState::DISCONNECTED);
    std::cout << "[SerialInterface] ✅ Serial communication stopped" << std::endl;
}

bool SerialInterface::isConnected() const noexcept {
    return connection_state_.load() == ConnectionState::CONNECTED;
}

SerialInterface::ConnectionState SerialInterface::getConnectionState() const noexcept {
    return connection_state_.load();
}

void SerialInterface::setDataCallback(DataCallback callback) {
    data_callback_ = std::move(callback);
}

void SerialInterface::setErrorCallback(ErrorCallback callback) {
    error_callback_ = std::move(callback);
}

data::SerialStatistics SerialInterface::getStatistics() const {
    std::lock_guard<std::mutex> lock(stats_mutex_);

    // Update uptime
    auto now = std::chrono::steady_clock::now();
    auto uptime = std::chrono::duration_cast<std::chrono::seconds>(
        now - connection_start_time_).count();

    auto stats = statistics_;
    stats.uptime_seconds = static_cast<uint64_t>(uptime);

    // Calculate messages per second
    if (uptime > 0) {
        stats.messages_per_second = static_cast<double>(stats.messages_received) / uptime;
    }

    return stats;
}

void SerialInterface::sendCommand(const std::string& command) {
    if (!isConnected()) {
        std::cout << "[SerialInterface] ⚠️ Cannot send command - not connected" << std::endl;
        return;
    }

    try {
        std::string cmd_with_newline = command + constants::communication::serial::COMMAND_TERMINATOR;
        boost::asio::write(*serial_port_, boost::asio::buffer(cmd_with_newline));

        {
            std::lock_guard<std::mutex> lock(stats_mutex_);
            statistics_.messages_sent++;
        }

        std::cout << "[SerialInterface] 📤 Sent command: " << command << std::endl;

    } catch (const std::exception& e) {
        handleConnectionError("Failed to send command: " + std::string(e.what()),
                            data::ErrorSeverity::ERROR);
    }
}

std::string SerialInterface::autoDetectArduinoPort() {
    std::cout << "[SerialInterface] 🔍 Auto-detecting Arduino port..." << std::endl;

    auto ports = getAvailablePorts();

    for (const auto& port : ports) {
        if (isArduinoPort(port)) {
            std::cout << "[SerialInterface] ✅ Found Arduino at: " << port << std::endl;
            return port;
        }
    }

    std::cout << "[SerialInterface] ❌ No Arduino port detected" << std::endl;
    return "";
}

bool SerialInterface::configurePort() {
    try {
        // Set baud rate
        serial_port_->set_option(boost::asio::serial_port_base::baud_rate(constants::communication::serial::BAUD_RATE));

        // Set character size (8 bits)
        serial_port_->set_option(boost::asio::serial_port_base::character_size(8));

        // Set parity (none)
        serial_port_->set_option(boost::asio::serial_port_base::parity(
            boost::asio::serial_port_base::parity::none));

        // Set stop bits (one)
        serial_port_->set_option(boost::asio::serial_port_base::stop_bits(
            boost::asio::serial_port_base::stop_bits::one));

        // Set flow control (none)
        serial_port_->set_option(boost::asio::serial_port_base::flow_control(
            boost::asio::serial_port_base::flow_control::none));

        std::cout << "[SerialInterface] Port configured: " << constants::communication::serial::BAUD_RATE
                  << " baud, 8N1, no flow control" << std::endl;

        return true;

    } catch (const std::exception& e) {
        std::cout << "[SerialInterface] ❌ Port configuration failed: " << e.what() << std::endl;
        return false;
    }
}

void SerialInterface::startAsyncRead() {
    if (shutdown_requested_.load() || !serial_port_ || !serial_port_->is_open()) {
        return;
    }

    serial_port_->async_read_some(
        boost::asio::buffer(read_buffer_, BUFFER_SIZE),
        [this](const boost::system::error_code& error, std::size_t bytes_transferred) {
            handleRead(error, bytes_transferred);
        });
}

void SerialInterface::handleRead(const boost::system::error_code& error,
                                std::size_t bytes_transferred) {
    if (shutdown_requested_.load()) {
        return;
    }

    if (error) {
        handleConnectionError("Read error: " + error.message(), data::ErrorSeverity::ERROR);
        attemptReconnection();
        return;
    }

    if (bytes_transferred > 0) {
        auto processing_start = std::chrono::steady_clock::now();

        // Add received data to buffer
        message_buffer_.append(read_buffer_.data(), bytes_transferred);

        // Process complete messages (ending with newline)
        size_t pos = 0;
        while ((pos = message_buffer_.find('\n')) != std::string::npos) {
            std::string message = message_buffer_.substr(0, pos);
            message_buffer_.erase(0, pos + 1);

            // Remove carriage return if present
            if (!message.empty() && message.back() == '\r') {
                message.pop_back();
            }

            if (!message.empty()) {
                processMessage(message);
            }
        }

        // Prevent buffer from growing too large
        if (message_buffer_.size() > BUFFER_SIZE * 2) {
            message_buffer_.clear();
            std::cout << "[SerialInterface] ⚠️ Message buffer cleared - size exceeded limit" << std::endl;
        }

        // Update statistics
        auto processing_time = std::chrono::duration_cast<std::chrono::microseconds>(
            std::chrono::steady_clock::now() - processing_start).count();

        {
            std::lock_guard<std::mutex> lock(stats_mutex_);
            // Use exponential moving average instead of simple /2 division
            auto alpha = constants::performance::optimization::MOVING_AVERAGE_ALPHA;
            statistics_.avg_processing_time_us = static_cast<uint32_t>(
                alpha * static_cast<double>(processing_time) +
                (1.0 - alpha) * static_cast<double>(statistics_.avg_processing_time_us)
            );
        }

        last_data_time_ = std::chrono::steady_clock::now();
    }

    // Continue reading
    startAsyncRead();
}


void SerialInterface::processMessage(const std::string& message) {
    auto radar_data = protocol_parser_->parseRadarData(message);

    if (radar_data.has_value()) {
        // Update statistics
        {
            std::lock_guard<std::mutex> lock(stats_mutex_);
            statistics_.messages_received++;
            statistics_.last_message_time = std::chrono::steady_clock::now();
        }

        // Call data callback if set
        if (data_callback_) {
            data_callback_(radar_data.value());
        }

        std::cout << "[SerialInterface] 📡 Radar data: angle="
                  << radar_data->angle << "°, distance=" << radar_data->distance << "cm" << std::endl;
    } else {
        // Update parse error statistics
        {
            std::lock_guard<std::mutex> lock(stats_mutex_);
            statistics_.parse_errors++;
        }
    }
}

void SerialInterface::handleConnectionError(const std::string& error_message,
                                          data::ErrorSeverity severity) {
    std::cout << "[SerialInterface] ERROR [" << static_cast<int>(severity)
              << "]: " << error_message << std::endl;

    {
        std::lock_guard<std::mutex> lock(stats_mutex_);
        statistics_.connection_errors++;
    }

    if (error_callback_) {
        error_callback_(error_message, severity);
    }

    if (severity >= data::ErrorSeverity::ERROR) {
        updateConnectionState(ConnectionState::ERROR);
    }
}

void SerialInterface::attemptReconnection() {
    if (shutdown_requested_.load()) {
        return;
    }

    updateConnectionState(ConnectionState::RECONNECTING);

    std::cout << "[SerialInterface] 🔄 Attempting reconnection in "
              << constants::communication::serial::RECONNECT_DELAY.count() << " seconds..." << std::endl;

    reconnect_timer_->expires_after(constants::communication::serial::RECONNECT_DELAY);
    reconnect_timer_->async_wait(
        [this](const boost::system::error_code& error) {
            onReconnectTimer(error);
        });
}

void SerialInterface::onReconnectTimer(const boost::system::error_code& error) {
    if (error || shutdown_requested_.load()) {
        return;
    }

    std::cout << "[SerialInterface] 🔄 Reconnecting..." << std::endl;

    // Close existing connection
    if (serial_port_ && serial_port_->is_open()) {
        try {
            serial_port_->close();
        } catch (...) {
            // Ignore close errors during reconnection
        }
    }

    // Attempt to restart
    if (start()) {
        std::cout << "[SerialInterface] ✅ Reconnection successful" << std::endl;
    } else {
        std::cout << "[SerialInterface] ❌ Reconnection failed, retrying..." << std::endl;
        attemptReconnection();
    }
}

void SerialInterface::updateConnectionState(ConnectionState new_state) {
    auto old_state = connection_state_.exchange(new_state);

    if (old_state != new_state) {
        std::cout << "[SerialInterface] State transition: "
                  << static_cast<int>(old_state) << " → "
                  << static_cast<int>(new_state) << std::endl;
    }
}


std::vector<std::string> SerialInterface::getAvailablePorts() {
    std::vector<std::string> ports;

    try {
        // MISRA C++ compliance: Cross-platform serial device detection
        const std::string device_path = constants::hardware::platform::SERIAL_DEVICE_PATH;

        // Platform-specific port detection
        if constexpr (constants::hardware::platform::IS_WINDOWS) {
            // Windows COM port detection
            for (int i = 1; i <= 256; ++i) {
                std::string port_name = "COM" + std::to_string(i);
                // Note: Windows port validation would require additional platform-specific code
                // For now, we'll use a simplified approach
                ports.push_back(port_name);
            }
        } else {
            // Unix-like systems (macOS, Linux)
            for (const auto& entry : std::filesystem::directory_iterator(device_path)) {
                const std::string filename = entry.path().filename().string();

                // Look for USB serial devices (cross-platform patterns)
                if (filename.find("tty.usb") == 0 ||
                    filename.find("cu.usb") == 0 ||
                    filename.find("tty.usbmodem") == 0 ||
                    filename.find("cu.usbmodem") == 0 ||
                    filename.find("ttyUSB") == 0 ||
                    filename.find("ttyACM") == 0) {
                    ports.push_back(entry.path().string());
                }
            }
        }

        // Sort ports for consistent ordering
        std::sort(ports.begin(), ports.end());

    } catch (const std::exception& e) {
        std::cout << "[SerialInterface] ⚠️ Error scanning ports: " << e.what() << std::endl;
    }

    return ports;
}

bool SerialInterface::isArduinoPort(const std::string& port_name) {
    // Check if the port name contains common Arduino identifiers
    return port_name.find("usbmodem") != std::string::npos ||
           port_name.find("usbserial") != std::string::npos ||
           (port_name.find("usb") != std::string::npos &&
            port_name.find("tty") != std::string::npos);
}

} // namespace siren::serial