/**
 * @file error_handler.hpp
 * @brief Centralized error handling utility for military-grade error processing
 * @author SIREN Project
 * @date 2025
 *
 * Military-grade centralized error handling with standardized logging,
 * severity classification, and thread-safe error processing.
 */

#pragma once

#include "data/radar_types.hpp"
#include <string>
#include <exception>
#include <mutex>
#include <atomic>

namespace siren::utils {

/**
 * @brief Centralized error handling utility class
 *
 * Provides standardized error handling across all components with:
 * - Consistent error message formatting
 * - Severity-based error routing
 * - Thread-safe error processing
 * - Exception message sanitization
 * - Component source tracking
 */
class ErrorHandler {
public:
    /**
     * @brief Handle system-level errors with specified severity
     * @param component Source component name
     * @param message Error message
     * @param severity Error severity level
     * @param error_code Optional error code (auto-generated if 0)
     */
    static void handleSystemError(const std::string& component,
                                  const std::string& message,
                                  data::ErrorSeverity severity,
                                  uint32_t error_code = 0);

    /**
     * @brief Handle exceptions with automatic message sanitization
     * @param component Source component name
     * @param context Error context description
     * @param exception The caught exception
     * @param severity Error severity level
     */
    static void handleException(const std::string& component,
                                const std::string& context,
                                const std::exception& exception,
                                data::ErrorSeverity severity);

    /**
     * @brief Handle connection-related errors
     * @param component Source component name
     * @param connection_info Connection details
     * @param error_message Error description
     * @param severity Error severity level
     */
    static void handleConnectionError(const std::string& component,
                                      const std::string& connection_info,
                                      const std::string& error_message,
                                      data::ErrorSeverity severity);

    /**
     * @brief Handle initialization errors
     * @param component Source component name
     * @param initialization_stage Current initialization stage
     * @param error_message Error description
     */
    static void handleInitializationError(const std::string& component,
                                           const std::string& initialization_stage,
                                           const std::string& error_message);

    /**
     * @brief Handle boost::system error codes
     * @param component Source component name
     * @param operation Description of failed operation
     * @param error_code Boost system error code
     * @param severity Error severity level
     */
    static void handleBoostError(const std::string& component,
                                 const std::string& operation,
                                 const std::error_code& error_code,
                                 data::ErrorSeverity severity);

private:
    /// Thread-safe logging mutex
    static std::mutex logging_mutex_;

    /// Error code counter for auto-generation
    static std::atomic<uint32_t> error_counter_;

    /**
     * @brief Sanitize exception message for security
     * @param raw_message Raw exception message
     * @return Sanitized message safe for logging
     */
    static std::string sanitizeMessage(const std::string& raw_message);

    /**
     * @brief Format error message with consistent military-grade format
     * @param component Source component
     * @param severity Error severity
     * @param message Error message
     * @param error_code Error code
     * @return Formatted error message
     */
    static std::string formatErrorMessage(const std::string& component,
                                          data::ErrorSeverity severity,
                                          const std::string& message,
                                          uint32_t error_code);

    /**
     * @brief Get severity symbol for logging
     * @param severity Error severity level
     * @return Symbol representing severity level
     */
    static const char* getSeveritySymbol(data::ErrorSeverity severity);

    /**
     * @brief Log error with thread-safe output
     * @param formatted_message Pre-formatted error message
     * @param severity Error severity level
     */
    static void logError(const std::string& formatted_message,
                         data::ErrorSeverity severity);
};

} // namespace siren::utils