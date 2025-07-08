/**
 * @file master_controller.hpp
 * @brief Refactored master controller with single responsibility
 * @author unoRadar Project
 * @date 2025
 *
 * Single responsibility: Coordinate and orchestrate system components.
 * State management, performance monitoring, and thread management delegated to specialized components.
 */

#pragma once

#include <memory>
#include <boost/asio.hpp>

#include "data/radar_types.hpp"
#include "core/system_state_manager.hpp"
#include "core/performance_monitor.hpp"
#include "serial/serial_interface.hpp"
#include "websocket/server.hpp"

namespace unoradar::core {

// Forward declarations
class DataProcessor;
class Logger;

/**
 * @brief Master controller with single responsibility: coordination
 *
 * Orchestrates system components without handling their internal concerns.
 * Delegates specialized responsibilities to dedicated components.
 */
class MasterController {
public:
    /**
     * @brief Construct master controller
     */
    explicit MasterController();

    /**
     * @brief Destructor - ensures clean shutdown
     */
    ~MasterController();

    // Non-copyable, non-movable (singleton-like behavior)
    MasterController(const MasterController&) = delete;
    MasterController& operator=(const MasterController&) = delete;
    MasterController(MasterController&&) = delete;
    MasterController& operator=(MasterController&&) = delete;

    /**
     * @brief Initialize all system components
     * @return true if initialization successful
     */
    bool initialize();

    /**
     * @brief Start the radar system operation
     * @return true if started successfully
     */
    bool start();

    /**
     * @brief Stop the radar system gracefully
     */
    void stop();

    /**
     * @brief Pause radar operation (maintains connections)
     */
    void pause();

    /**
     * @brief Resume radar operation from paused state
     */
    void resume();

    /**
     * @brief Get current system state
     */
    SystemStateManager::SystemState getSystemState() const noexcept;

    /**
     * @brief Get current performance metrics
     */
    data::PerformanceMetrics getPerformanceMetrics() const;

    /**
     * @brief Check if system is healthy
     */
    bool isHealthy() const noexcept;

    /**
     * @brief Run the main event loop (blocking)
     *
     * This is the main entry point for system operation.
     * Runs until stop() is called or critical error occurs.
     */
    void run();

    /**
     * @brief Emergency shutdown (immediate stop)
     */
    void emergencyShutdown() noexcept;

private:
    // Core I/O components
    std::unique_ptr<boost::asio::io_context> io_context_;
    std::unique_ptr<boost::asio::steady_timer> heartbeat_timer_;

    // Specialized responsibility components
    std::unique_ptr<SystemStateManager> state_manager_;
    std::unique_ptr<PerformanceMonitor> performance_monitor_;

    // Subsystem components
    std::unique_ptr<serial::SerialInterface> serial_interface_;
    std::unique_ptr<websocket::WebSocketServer> websocket_server_;
    // std::unique_ptr<DataProcessor> data_processor_;      // Will be implemented later
    // std::unique_ptr<Logger> logger_;                     // Will be implemented later

    // Shutdown coordination
    std::atomic<bool> shutdown_requested_;

    /**
     * @brief Initialize I/O context and timers
     */
    void initializeIOContext();

    /**
     * @brief Initialize all subsystems
     */
    bool initializeSubsystems();

    /**
     * @brief Set up periodic tasks (heartbeat)
     */
    void setupPeriodicTasks();

    /**
     * @brief Heartbeat callback for system monitoring
     */
    void onHeartbeat(const boost::system::error_code& error);

    // handleSystemError removed - now using centralized ErrorHandler utility

    /**
     * @brief Cleanup all resources
     */
    void cleanup();

    /**
     * @brief State change callback from SystemStateManager
     */
    void onStateChange(SystemStateManager::SystemState old_state,
                      SystemStateManager::SystemState new_state);

    /**
     * @brief Metrics update callback from PerformanceMonitor
     */
    void onMetricsUpdate(const data::PerformanceMetrics& metrics);

    /**
     * @brief Radar data callback from SerialInterface
     */
    void onRadarData(const data::RadarDataPoint& radar_data);

    /**
     * @brief Serial error callback from SerialInterface
     */
    void onSerialError(const std::string& error_message, data::ErrorSeverity severity);
};

} // namespace unoradar::core