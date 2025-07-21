/**
 * @file performance_monitor.hpp
 * @brief Performance monitoring component for SIREN
 * @author KostasAndroulidakis
 * @date 2025
 *
 * Single responsibility: Monitor and collect system performance metrics.
 */

#pragma once

#include <chrono>
#include <mutex>
#include <functional>
#include "data/sonar_types.hpp"
#include "utils/statistics_calculator.hpp"

namespace siren::core {

/**
 * @brief Performance monitor with single responsibility
 *
 * Handles only performance metrics collection and monitoring.
 * Thread-safe metrics management.
 */
class PerformanceMonitor {
public:
    /// Performance update callback type
    using MetricsCallback = std::function<void(const data::PerformanceMetrics&)>;

    /**
     * @brief Constructor
     */
    PerformanceMonitor();

    /**
     * @brief Start performance monitoring
     */
    void start();

    /**
     * @brief Stop performance monitoring
     */
    void stop();

    /**
     * @brief Update metrics with new measurements
     * @param processing_time_us Processing time in microseconds
     */
    void recordProcessingTime(uint64_t processing_time_us);

    /**
     * @brief Record message processing
     */
    void recordMessage();

    /**
     * @brief Update active connections count
     * @param count Current active connections
     */
    void updateActiveConnections(uint16_t count);

    /**
     * @brief Update serial status
     * @param status Current serial connection status
     */
    void updateSerialStatus(data::SerialStatus status);

    /**
     * @brief Get current performance metrics (thread-safe)
     */
    data::PerformanceMetrics getCurrentMetrics() const;

    /**
     * @brief Set metrics update callback
     * @param callback Function to call when metrics are updated
     */
    void setMetricsCallback(MetricsCallback callback);

    /**
     * @brief Reset all metrics counters
     */
    void resetMetrics();

    /**
     * @brief Get detailed latency statistics
     * @return Current latency statistics from StatisticsCalculator
     */
    utils::UInt32StatsCalculator::Statistics getLatencyStatistics() const;

private:
    mutable std::mutex metrics_mutex_;
    data::PerformanceMetrics current_metrics_;

    // Timing tracking
    std::chrono::steady_clock::time_point start_time_;
    std::chrono::steady_clock::time_point last_update_;

    // Message counting
    uint64_t total_messages_;
    uint64_t messages_since_last_update_;

    // Military-grade statistics calculators
    utils::UInt32StatsCalculator latency_calculator_;
    utils::UInt32StatsCalculator throughput_calculator_;
    utils::UInt64StatsCalculator memory_calculator_;

    // Callback
    MetricsCallback metrics_callback_;

    // State
    bool monitoring_;

    /**
     * @brief Update calculated metrics
     */
    void updateCalculatedMetrics();

    /**
     * @brief Calculate messages per second
     */
    void calculateMessagesPerSecond();

    /**
     * @brief Update memory usage estimation
     */
    void updateMemoryUsage();
};

} // namespace siren::core