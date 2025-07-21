/**
 * @file statistics_collector.hpp
 * @brief WebSocket server statistics collector - MISRA C++ compliant
 * @author KostasAndroulidakis
 * @date 2025
 *
 * Single Responsibility: Collect and manage WebSocket server statistics ONLY
 * MISRA C++ 2023 compliant - Rule 5.0.1 (no magic numbers), Rule 21.2.1 (RAII)
 */

#pragma once

#include <atomic>
#include <mutex>
#include <chrono>
#include "data/sonar_types.hpp"

namespace siren::websocket {

/**
 * @brief WebSocket server statistics collector with single responsibility
 *
 * RESPONSIBILITIES:
 * - Collect connection statistics ONLY
 * - Track server uptime ONLY
 * - Provide thread-safe statistics access ONLY
 * - Manage statistics lifecycle ONLY
 *
 * NOT RESPONSIBLE FOR:
 * - Connection management (handled by ConnectionAcceptor)
 * - Session management (handled by SessionManager)
 * - Message broadcasting (handled by MessageBroadcaster)
 * - Server coordination (handled by WebSocketServer)
 *
 * MISRA C++ Compliance:
 * - Rule 5.0.1: All constants defined, no magic numbers
 * - Rule 21.2.1: RAII for all resources
 * - Rule 8.4.1: Single responsibility per class
 * - Rule 18.1.1: Thread-safe data access
 */
class StatisticsCollector {
public:
    /**
     * @brief Constructor - RAII initialization
     */
    explicit StatisticsCollector();

    /**
     * @brief Destructor - RAII cleanup
     */
    ~StatisticsCollector();

    // MISRA C++ Rule 12.1.1: Disable copy/move for resource management
    StatisticsCollector(const StatisticsCollector&) = delete;
    StatisticsCollector& operator=(const StatisticsCollector&) = delete;
    StatisticsCollector(StatisticsCollector&&) = delete;
    StatisticsCollector& operator=(StatisticsCollector&&) = delete;

    /**
     * @brief Initialize statistics collection
     * @return true if initialization successful
     */
    bool initialize();

    /**
     * @brief Start statistics collection
     * @return true if started successfully
     */
    bool start();

    /**
     * @brief Stop statistics collection
     */
    void stop();

    /**
     * @brief Check if statistics collection is running
     */
    bool isRunning() const noexcept;

    /**
     * @brief Record connection accepted (SSOT for connection tracking)
     */
    void recordConnectionAccepted();

    /**
     * @brief Record connection rejected (SSOT for error tracking)
     */
    void recordConnectionRejected();

    /**
     * @brief Record connection closed (SSOT for disconnection tracking)
     */
    void recordConnectionClosed();

    /**
     * @brief Record message broadcast (SSOT for message tracking)
     */
    void recordMessageBroadcast();

    /**
     * @brief Record error occurrence (SSOT for error tracking)
     */
    void recordError();

    /**
     * @brief Get current statistics snapshot (SSOT for statistics access)
     * @param active_connections Current number of active connections
     * @return Complete statistics snapshot
     */
    data::WebSocketStatistics getStatistics(size_t active_connections) const;

    /**
     * @brief Reset all statistics counters (SSOT for statistics reset)
     */
    void resetStatistics();

    /**
     * @brief Get server uptime in seconds (SSOT for uptime calculation)
     * @return Server uptime in seconds
     */
    uint64_t getUptimeSeconds() const noexcept;

private:
    // State management
    std::atomic<bool> running_;
    std::atomic<bool> initialized_;

    // Statistics storage - thread-safe atomic counters (MISRA C++ Rule 18.1.1)
    std::atomic<uint64_t> connections_accepted_;
    std::atomic<uint64_t> connections_rejected_;
    std::atomic<uint64_t> connections_closed_;
    std::atomic<uint64_t> messages_broadcast_;
    std::atomic<uint64_t> errors_occurred_;

    // Timing information
    std::chrono::steady_clock::time_point start_time_;

    // Thread-safe access for complex operations (when atomics aren't sufficient)
    mutable std::mutex stats_mutex_;

    // SSOT constants (MISRA C++ Rule 5.0.1)
    static constexpr const char* COMPONENT_NAME = "StatisticsCollector";

    /**
     * @brief Initialize all counters to zero (SSOT for initialization)
     */
    void initializeCounters();

    /**
     * @brief Validate statistics consistency (SSOT for validation)
     * @return true if statistics are consistent
     */
    bool validateStatistics() const;
};

} // namespace siren::websocket