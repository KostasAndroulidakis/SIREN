/**
 * @file performance_monitor.cpp
 * @brief Implementation of performance monitor
 * @author KostasAndroulidakis
 * @date 2025
 */

#include "core/performance_monitor.hpp"
#include "constants/performance.hpp"
#include "utils/statistics_calculator.hpp"
#include <iostream>

namespace siren::core {

namespace constants = siren::constants;

PerformanceMonitor::PerformanceMonitor()
    : current_metrics_{}
    , start_time_(std::chrono::steady_clock::now())
    , last_update_(std::chrono::steady_clock::now())
    , total_messages_(0)
    , messages_since_last_update_(0)
    , latency_calculator_(siren::utils::performance_stats::createLatencyCalculator())
    , throughput_calculator_(siren::utils::performance_stats::createThroughputCalculator())
    , memory_calculator_(siren::utils::performance_stats::createMemoryUsageCalculator())
    , monitoring_(false)
{
    std::cout << "[PerformanceMonitor] Initializing military-grade performance monitoring with StatisticsCalculator..." << std::endl;
}

void PerformanceMonitor::start() {
    std::lock_guard<std::mutex> lock(metrics_mutex_);

    if (monitoring_) {
        std::cout << "[PerformanceMonitor] ⚠️ Already monitoring" << std::endl;
        return;
    }

    monitoring_ = true;
    start_time_ = std::chrono::steady_clock::now();
    last_update_ = start_time_;

    // Reset metrics
    current_metrics_ = data::PerformanceMetrics{};
    total_messages_ = 0;
    messages_since_last_update_ = 0;

    std::cout << "[PerformanceMonitor] ✅ Performance monitoring started" << std::endl;
}

void PerformanceMonitor::stop() {
    std::lock_guard<std::mutex> lock(metrics_mutex_);

    if (!monitoring_) {
        return;
    }

    monitoring_ = false;
    std::cout << "[PerformanceMonitor] 🛑 Performance monitoring stopped" << std::endl;
}

void PerformanceMonitor::recordProcessingTime(uint64_t processing_time_us) {
    std::lock_guard<std::mutex> lock(metrics_mutex_);

    if (!monitoring_) {
        return;
    }

    // Use StatisticsCalculator instead of duplicate moving average calculation
    auto latency_stats = latency_calculator_.addSample(static_cast<uint32_t>(processing_time_us));

    // Update metrics from statistics calculator
    current_metrics_.avg_latency_us = latency_stats.exponential_average;
    current_metrics_.max_latency_us = latency_stats.maximum_value;

    updateCalculatedMetrics();
}

void PerformanceMonitor::recordMessage() {
    std::lock_guard<std::mutex> lock(metrics_mutex_);

    if (!monitoring_) {
        return;
    }

    total_messages_++;
    messages_since_last_update_++;

    updateCalculatedMetrics();
}

void PerformanceMonitor::updateActiveConnections(uint16_t count) {
    std::lock_guard<std::mutex> lock(metrics_mutex_);
    current_metrics_.active_connections = count;
    updateCalculatedMetrics();
}

void PerformanceMonitor::updateSerialStatus(data::SerialStatus status) {
    std::lock_guard<std::mutex> lock(metrics_mutex_);
    current_metrics_.serial_status = status;
    updateCalculatedMetrics();
}

data::PerformanceMetrics PerformanceMonitor::getCurrentMetrics() const {
    std::lock_guard<std::mutex> lock(metrics_mutex_);
    return current_metrics_;
}

void PerformanceMonitor::setMetricsCallback(MetricsCallback callback) {
    metrics_callback_ = std::move(callback);
}

void PerformanceMonitor::resetMetrics() {
    std::lock_guard<std::mutex> lock(metrics_mutex_);

    current_metrics_ = data::PerformanceMetrics{};
    total_messages_ = 0;
    messages_since_last_update_ = 0;
    start_time_ = std::chrono::steady_clock::now();
    last_update_ = start_time_;

    // Reset statistics calculators
    latency_calculator_.reset();
    throughput_calculator_.reset();
    memory_calculator_.reset();

    std::cout << "[PerformanceMonitor] 🔄 Metrics and statistics calculators reset" << std::endl;
}

void PerformanceMonitor::updateCalculatedMetrics() {
    // Update timestamp
    current_metrics_.timestamp = std::chrono::steady_clock::now();

    // Calculate messages per second
    calculateMessagesPerSecond();

    // Update memory usage estimation
    updateMemoryUsage();

    // Notify callback if set
    if (metrics_callback_) {
        metrics_callback_(current_metrics_);
    }
}

void PerformanceMonitor::calculateMessagesPerSecond() {
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - start_time_).count();

    if (elapsed > 0) {
        current_metrics_.messages_per_second = static_cast<uint32_t>(total_messages_ / elapsed);
    } else {
        current_metrics_.messages_per_second = 0;
    }
}

void PerformanceMonitor::updateMemoryUsage() {
    // Basic memory usage estimation (placeholder)
    // In a real implementation, this would use platform-specific APIs
    size_t estimated_usage =
        sizeof(data::PerformanceMetrics) +
        (current_metrics_.active_connections * constants::performance::memory::ESTIMATED_CONNECTION_MEMORY_BYTES) +
        constants::performance::memory::BASE_USAGE_BYTES;

    current_metrics_.memory_usage_bytes = estimated_usage;

    // Update memory statistics calculator
    memory_calculator_.addSample(estimated_usage);
}

utils::UInt32StatsCalculator::Statistics PerformanceMonitor::getLatencyStatistics() const {
    std::lock_guard<std::mutex> lock(metrics_mutex_);
    return latency_calculator_.getStatistics();
}

} // namespace siren::core