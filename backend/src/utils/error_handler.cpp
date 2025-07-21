/**
 * @file error_handler.cpp
 * @brief Implementation of centralized error handling utility
 * @author KostasAndroulidakis
 * @date 2025
 */

#include "utils/error_handler.hpp"
#include "constants/error.hpp"
#include <iostream>
#include <chrono>
#include <iomanip>
#include <sstream>

namespace siren::utils {

namespace cnst = siren::constants;

// Static member definitions
std::mutex ErrorHandler::logging_mutex_;
std::atomic<uint32_t> ErrorHandler::error_counter_{cnst::error::handling::ERROR_CODE_BASE}; // Start from 1000 for military-grade error codes

void ErrorHandler::handleSystemError(const std::string& component,
                                      const std::string& message,
                                      data::ErrorSeverity severity,
                                      uint32_t error_code) {
    if (error_code == 0) {
        error_code = error_counter_.fetch_add(1);
    }

    const std::string formatted_message = formatErrorMessage(component, severity, message, error_code);
    logError(formatted_message, severity);
}

void ErrorHandler::handleException(const std::string& component,
                                   const std::string& context,
                                   const std::exception& exception,
                                   data::ErrorSeverity severity) {
    const std::string sanitized_message = sanitizeMessage(exception.what());
    const std::string full_message = context + ": " + sanitized_message;

    handleSystemError(component, full_message, severity);
}

void ErrorHandler::handleConnectionError(const std::string& component,
                                         const std::string& connection_info,
                                         const std::string& error_message,
                                         data::ErrorSeverity severity) {
    const std::string full_message = "Connection error [" + connection_info + "]: " + error_message;
    handleSystemError(component, full_message, severity);
}

void ErrorHandler::handleInitializationError(const std::string& component,
                                              const std::string& initialization_stage,
                                              const std::string& error_message) {
    const std::string full_message = "Initialization failed at stage [" + initialization_stage + "]: " + error_message;
    handleSystemError(component, full_message, data::ErrorSeverity::FATAL);
}

void ErrorHandler::handleBoostError(const std::string& component,
                                    const std::string& operation,
                                    const std::error_code& error_code,
                                    data::ErrorSeverity severity) {
    const std::string full_message = "Boost operation [" + operation + "] failed: " +
                                     error_code.message() + " (code: " + std::to_string(error_code.value()) + ")";
    handleSystemError(component, full_message, severity);
}

std::string ErrorHandler::sanitizeMessage(const std::string& raw_message) {
    // Remove potential sensitive information for military-grade security
    std::string sanitized = raw_message;

    // Limit message length to prevent information leakage
    if (sanitized.length() > cnst::error::handling::MAX_ERROR_MESSAGE_LENGTH) {
        sanitized = sanitized.substr(0, cnst::error::handling::MAX_ERROR_MESSAGE_LENGTH) + "...";
    }

    // Remove potential file paths for security
    size_t path_pos = sanitized.find("/Users");
    if (path_pos != std::string::npos) {
        sanitized = sanitized.substr(0, path_pos) + "[SANITIZED_PATH]";
    }

    path_pos = sanitized.find("/home");
    if (path_pos != std::string::npos) {
        sanitized = sanitized.substr(0, path_pos) + "[SANITIZED_PATH]";
    }

    return sanitized;
}

std::string ErrorHandler::formatErrorMessage(const std::string& component,
                                             data::ErrorSeverity severity,
                                             const std::string& message,
                                             uint32_t error_code) {
    // Get current timestamp for military-grade logging
    auto now = std::chrono::steady_clock::now();
    auto timestamp_us = std::chrono::duration_cast<std::chrono::microseconds>(
        now.time_since_epoch()).count();

    std::ostringstream oss;
    oss << "[" << component << "] "
        << getSeveritySymbol(severity) << " "
        << "Error #" << std::setfill('0') << std::setw(4) << error_code << ": "
        << message
        << " (timestamp: " << timestamp_us << "μs)";

    return oss.str();
}

const char* ErrorHandler::getSeveritySymbol(data::ErrorSeverity severity) {
    switch (severity) {
        case data::ErrorSeverity::INFO:     return "ℹ️";
        case data::ErrorSeverity::WARNING:  return "⚠️";
        case data::ErrorSeverity::ERROR:    return "❌";
        case data::ErrorSeverity::CRITICAL: return "🔴";
        case data::ErrorSeverity::FATAL:    return "💀";
        default:                            return "❓";
    }
}

void ErrorHandler::logError(const std::string& formatted_message,
                            data::ErrorSeverity severity) {
    std::lock_guard<std::mutex> lock(logging_mutex_);

    // For FATAL and CRITICAL errors, output to stderr
    if (severity >= data::ErrorSeverity::CRITICAL) {
        std::cerr << formatted_message << std::endl;
    } else {
        std::cout << formatted_message << std::endl;
    }
}

} // namespace siren::utils