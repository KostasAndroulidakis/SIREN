/**
 * @file performance.hpp
 * @brief Real-time performance requirements and memory management for military-grade sonar
 * @author SIREN Defense Systems
 * @date 2025
 * @classification UNCLASSIFIED
 *
 * Performance constraints, timing requirements, and memory management specifications.
 * Defines military-grade response times and deterministic memory allocation.
 *
 * MISRA C++ Compliance: All constants are constexpr and strongly typed
 * SRP: Single responsibility - performance and memory management only
 */

#pragma once

#include <cstdint>
#include <cstddef>

namespace siren { namespace constants { namespace performance {

/// Real-time performance requirements
namespace timing {
    /// Target event loop iteration time in microseconds (military-grade)
    constexpr uint32_t TARGET_LOOP_TIME_US = 1000;  // 1ms target

    /// Maximum acceptable latency in microseconds (hard constraint)
    constexpr uint32_t MAX_LATENCY_US = 5000;  // 5ms maximum

    /// Performance monitoring interval in seconds
    constexpr uint32_t MONITORING_INTERVAL_SEC = 1;

    /// Thread pool size for concurrent operations
    constexpr uint8_t THREAD_POOL_SIZE = 4;

    /// High-priority thread stack size in bytes
    constexpr uint32_t HIGH_PRIORITY_STACK_SIZE = 65536;  // 64KB
}

/// Memory management configuration
namespace memory {
    /// Memory allocation block size in bytes (deterministic allocation)
    constexpr uint32_t BLOCK_SIZE_BYTES = 1024;

    /// Estimated memory per WebSocket connection in bytes
    constexpr uint32_t ESTIMATED_CONNECTION_MEMORY_BYTES = 8192;  // 8KB per connection

    /// Base system memory usage estimate in bytes
    constexpr uint32_t BASE_USAGE_BYTES = 1048576;  // 1MB base usage
}

/// Buffer and queue sizing
namespace buffers {
    /// Serial receive buffer size in bytes
    constexpr uint32_t SERIAL_BUFFER_SIZE_BYTES = 1024;

    /// Sonar data point buffer size (number of points)
    constexpr uint32_t SONAR_DATA_BUFFER_SIZE = 1000;

    /// WebSocket message queue size
    constexpr uint32_t WEBSOCKET_QUEUE_SIZE = 100;

    /// Error log buffer size
    constexpr uint32_t ERROR_LOG_BUFFER_SIZE = 500;

    /// Performance metrics history size (1 hour at 1Hz)
    constexpr uint32_t METRICS_HISTORY_SIZE = 3600;
}

/// System optimization constants
namespace optimization {
    /// Moving average calculation factor (exponential moving average)
    constexpr double MOVING_AVERAGE_ALPHA = 0.1;

    /// CPU spin prevention divisor
    constexpr uint32_t SPIN_PREVENTION_DIVISOR = 10;

    /// Health check timeout threshold
    constexpr uint32_t HEALTH_CHECK_TIMEOUT_SEC = 5;

    /// Boost version display divisors (for version parsing)
    constexpr uint32_t BOOST_VERSION_MAJOR_DIVISOR = 100000;
    constexpr uint32_t BOOST_VERSION_MINOR_DIVISOR = 100;
    constexpr uint32_t BOOST_VERSION_MINOR_MODULO = 1000;
    constexpr uint32_t BOOST_VERSION_PATCH_MODULO = 100;
}

} } } // namespace siren::constants::performance