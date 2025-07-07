/**
 * @file master_controller.hpp
 * @brief Master controller for unoRadar military-grade server
 * @author unoRadar Project
 * @date 2025
 *
 * Central orchestrator following Master-Slave architecture pattern.
 * Coordinates all system components with military-grade performance.
 */

#pragma once

#include <memory>
#include <atomic>
#include <thread>
#include <boost/asio.hpp>

#include "utils/constants.hpp"
#include "data/radar_types.hpp"

namespace unoradar::core {

// Forward declarations
class SerialInterface;
class WebSocketServer;
class DataProcessor;
class Logger;

/**
 * @brief Master controller orchestrating all radar system components
 *
 * Implements event-driven architecture with real-time performance guarantees.
 * Single responsibility: coordinate and monitor all subsystems.
 */
class MasterController {
public:
    /// System operational states
    enum class SystemState {
        INITIALIZING,   ///< System starting up
        RUNNING,        ///< Normal operation
        PAUSING,        ///< Graceful pause in progress
        PAUSED,         ///< System paused
        STOPPING,       ///< Shutdown in progress
        STOPPED,        ///< System stopped
        ERROR           ///< Critical error state
    };

    /**
     * @brief Construct master controller with configuration
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
    SystemState getSystemState() const noexcept;

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
    // Core components
    std::unique_ptr<boost::asio::io_context> io_context_;
    std::unique_ptr<boost::asio::steady_timer> heartbeat_timer_;
    std::unique_ptr<boost::asio::steady_timer> metrics_timer_;

    // Subsystem components (will be implemented later)
    // std::unique_ptr<SerialInterface> serial_interface_;
    // std::unique_ptr<WebSocketServer> websocket_server_;
    // std::unique_ptr<DataProcessor> data_processor_;
    // std::unique_ptr<Logger> logger_;

    // System state management
    std::atomic<SystemState> system_state_;
    std::atomic<bool> shutdown_requested_;
    std::atomic<bool> healthy_;

    // Performance monitoring
    mutable std::mutex metrics_mutex_;
    data::PerformanceMetrics current_metrics_;
    std::chrono::steady_clock::time_point last_metrics_update_;

    // Threading
    std::vector<std::thread> worker_threads_;

    /**
     * @brief Initialize I/O context and timers
     */
    void initializeIOContext();

    /**
     * @brief Initialize all subsystems
     */
    bool initializeSubsystems();

    /**
     * @brief Set up periodic tasks (heartbeat, metrics)
     */
    void setupPeriodicTasks();

    /**
     * @brief Heartbeat callback for system monitoring
     */
    void onHeartbeat(const boost::system::error_code& error);

    /**
     * @brief Metrics collection callback
     */
    void onMetricsUpdate(const boost::system::error_code& error);

    /**
     * @brief Update system state atomically
     */
    void updateSystemState(SystemState new_state);

    /**
     * @brief Handle system errors
     */
    void handleSystemError(const std::string& error_message,
                          data::ErrorSeverity severity);

    /**
     * @brief Cleanup all resources
     */
    void cleanup();

    /**
     * @brief Start worker threads for concurrent operations
     */
    void startWorkerThreads();

    /**
     * @brief Stop and join all worker threads
     */
    void stopWorkerThreads();

    /**
     * @brief Worker thread function
     */
    void workerThreadFunction();

    /**
     * @brief Update performance metrics
     */
    void updatePerformanceMetrics();

    /**
     * @brief Check system health status
     */
    void performHealthCheck();
};

} // namespace unoradar::core