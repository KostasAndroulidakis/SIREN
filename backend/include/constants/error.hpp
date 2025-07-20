/**
 * @file error.hpp
 * @brief Error handling, logging, and recovery configuration for military-grade radar
 * @author unoRadar Defense Systems
 * @date 2025
 * @classification UNCLASSIFIED
 *
 * Error codes, retry mechanisms, and logging configuration.
 * Defines military-grade error handling and recovery strategies.
 *
 * MISRA C++ Compliance: All constants are constexpr and strongly typed
 * SRP: Single responsibility - error handling and logging only
 */

#pragma once

#include <cstdint>
#include <cstddef>
#include <chrono>

namespace siren { namespace constants { namespace error {

/// Error handling configuration
namespace handling {
    /// Maximum retry attempts for recoverable errors
    constexpr uint8_t MAX_RETRY_ATTEMPTS = 3;

    /// Base retry delay in milliseconds (exponential backoff)
    constexpr uint32_t BASE_RETRY_DELAY_MS = 100;

    /// Maximum retry delay in milliseconds (exponential backoff cap)
    constexpr uint32_t MAX_RETRY_DELAY_MS = 5000;

    /// Maximum error message length (security constraint)
    constexpr size_t MAX_ERROR_MESSAGE_LENGTH = 512;

    /// Base error code for ErrorHandler system
    constexpr uint32_t ERROR_CODE_BASE = 1000;

    /// Error recovery delay for MasterController
    constexpr auto ERROR_RECOVERY_DELAY = std::chrono::milliseconds(100);
}

/// Error code categorization
namespace codes {
    /// Serial communication error base code
    constexpr uint32_t SERIAL_ERROR_BASE = 1000;

    /// Network communication error base code
    constexpr uint32_t NETWORK_ERROR_BASE = 2000;

    /// Data processing error base code
    constexpr uint32_t DATA_ERROR_BASE = 3000;

    /// System-level error base code
    constexpr uint32_t SYSTEM_ERROR_BASE = 4000;
}

/// Logging configuration
namespace logging {
    /// Maximum log file size in bytes (10MB)
    constexpr uint32_t MAX_LOG_FILE_SIZE_BYTES = 10 * 1024 * 1024;

    /// Number of log files to rotate
    constexpr uint8_t LOG_FILE_ROTATION_COUNT = 5;

    /// Log flush interval in seconds
    constexpr uint32_t LOG_FLUSH_INTERVAL_SEC = 5;

    /// Maximum log line length
    constexpr uint16_t MAX_LOG_LINE_LENGTH = 2048;
}

} } } // namespace siren::constants::error