/**
 * @file statistics_collector.cpp
 * @brief Implementation of WebSocket server statistics collector - MISRA C++ compliant
 * @author KostasAndroulidakis
 * @date 2025
 *
 * Single Responsibility: Collect and manage WebSocket server statistics ONLY
 */

#include "websocket/statistics_collector.hpp"
#include "utils/error_handler.hpp"
#include <iostream>

namespace siren::websocket {

StatisticsCollector::StatisticsCollector()
    : running_(false)
    , initialized_(false)
    , connections_accepted_(0)
    , connections_rejected_(0)
    , connections_closed_(0)
    , messages_broadcast_(0)
    , errors_occurred_(0)
    , start_time_(std::chrono::steady_clock::now())
    , stats_mutex_()
{
    std::cout << "[" << COMPONENT_NAME << "] Initializing statistics collector" << std::endl;
}

StatisticsCollector::~StatisticsCollector() {
    if (running_.load()) {
        stop();
    }
}

bool StatisticsCollector::initialize() {
    if (initialized_.load()) {
        utils::ErrorHandler::handleSystemError(COMPONENT_NAME,
                                             "Already initialized",
                                             data::ErrorSeverity::WARNING);
        return true;
    }

    try {
        // Initialize all counters
        initializeCounters();

        // Record start time
        start_time_ = std::chrono::steady_clock::now();

        initialized_.store(true);

        std::cout << "[" << COMPONENT_NAME << "] Initialized successfully" << std::endl;
        return true;

    } catch (const std::exception& e) {
        utils::ErrorHandler::handleException(COMPONENT_NAME, "initialization", e,
                                           data::ErrorSeverity::FATAL);
        return false;
    }
}

bool StatisticsCollector::start() {
    if (!initialized_.load()) {
        utils::ErrorHandler::handleSystemError(COMPONENT_NAME,
                                             "Not initialized - cannot start",
                                             data::ErrorSeverity::ERROR);
        return false;
    }

    if (running_.load()) {
        utils::ErrorHandler::handleSystemError(COMPONENT_NAME,
                                             "Already running",
                                             data::ErrorSeverity::WARNING);
        return true;
    }

    try {
        running_.store(true);

        std::cout << "[" << COMPONENT_NAME << "] Started successfully" << std::endl;
        return true;

    } catch (const std::exception& e) {
        utils::ErrorHandler::handleException(COMPONENT_NAME, "start operation", e,
                                           data::ErrorSeverity::ERROR);
        return false;
    }
}

void StatisticsCollector::stop() {
    if (!running_.load()) {
        return;
    }

    std::cout << "[" << COMPONENT_NAME << "] Stopping statistics collection..." << std::endl;

    running_.store(false);

    // Print final statistics
    const auto final_stats = getStatistics(0); // No active connections when stopping
    std::cout << "[" << COMPONENT_NAME << "] Final statistics:" << std::endl;
    std::cout << "  - Connections accepted: " << final_stats.connections_accepted << std::endl;
    std::cout << "  - Connections rejected: " << final_stats.connections_rejected << std::endl;
    std::cout << "  - Messages broadcast: " << final_stats.messages_broadcast << std::endl;
    std::cout << "  - Errors occurred: " << final_stats.errors_occurred << std::endl;
    std::cout << "  - Uptime: " << final_stats.uptime_seconds << " seconds" << std::endl;

    std::cout << "[" << COMPONENT_NAME << "] Stopped" << std::endl;
}

bool StatisticsCollector::isRunning() const noexcept {
    return running_.load();
}

void StatisticsCollector::recordConnectionAccepted() {
    if (running_.load()) {
        connections_accepted_.fetch_add(1);
    }
}

void StatisticsCollector::recordConnectionRejected() {
    if (running_.load()) {
        connections_rejected_.fetch_add(1);
    }
}

void StatisticsCollector::recordConnectionClosed() {
    if (running_.load()) {
        connections_closed_.fetch_add(1);
    }
}

void StatisticsCollector::recordMessageBroadcast() {
    if (running_.load()) {
        messages_broadcast_.fetch_add(1);
    }
}

void StatisticsCollector::recordError() {
    if (running_.load()) {
        errors_occurred_.fetch_add(1);
    }
}

data::WebSocketStatistics StatisticsCollector::getStatistics(size_t active_connections) const {
    // Create statistics snapshot (thread-safe with atomic operations)
    data::WebSocketStatistics stats{};

    // Atomic loads (thread-safe)
    stats.connections_accepted = connections_accepted_.load();
    stats.connections_rejected = connections_rejected_.load();
    stats.connections_closed = connections_closed_.load();
    stats.messages_broadcast = messages_broadcast_.load();
    stats.errors_occurred = errors_occurred_.load();

    // Current active connections (provided by caller)
    stats.active_connections = active_connections;

    // Calculate uptime
    stats.uptime_seconds = getUptimeSeconds();

    return stats;
}

void StatisticsCollector::resetStatistics() {
    std::lock_guard<std::mutex> lock(stats_mutex_);

    std::cout << "[" << COMPONENT_NAME << "] Resetting all statistics..." << std::endl;

    // Reset all counters atomically
    initializeCounters();

    // Reset start time
    start_time_ = std::chrono::steady_clock::now();

    std::cout << "[" << COMPONENT_NAME << "] Statistics reset complete" << std::endl;
}

uint64_t StatisticsCollector::getUptimeSeconds() const noexcept {
    const auto now = std::chrono::steady_clock::now();
    const auto uptime = std::chrono::duration_cast<std::chrono::seconds>(
        now - start_time_).count();
    return static_cast<uint64_t>(uptime);
}

void StatisticsCollector::initializeCounters() {
    // Initialize all atomic counters to zero (SSOT for initialization)
    connections_accepted_.store(0);
    connections_rejected_.store(0);
    connections_closed_.store(0);
    messages_broadcast_.store(0);
    errors_occurred_.store(0);
}

bool StatisticsCollector::validateStatistics() const {
    // Basic consistency checks
    const uint64_t accepted = connections_accepted_.load();
    const uint64_t rejected = connections_rejected_.load();
    const uint64_t closed = connections_closed_.load();
    const uint64_t errors = errors_occurred_.load();

    // Validate that closed connections doesn't exceed accepted connections
    if (closed > accepted) {
        utils::ErrorHandler::handleSystemError(COMPONENT_NAME,
                                             "Inconsistent statistics: closed > accepted",
                                             data::ErrorSeverity::WARNING);
        return false;
    }

    // Log validation success for debugging
    std::cout << "[" << COMPONENT_NAME << "] Statistics validation passed" << std::endl;
    return true;
}

} // namespace siren::websocket