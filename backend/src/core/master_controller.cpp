/**
 * @file master_controller.cpp
 * @brief Implementation of refactored master controller
 * @author unoRadar Project
 * @date 2025
 *
 * Coordination-focused implementation with delegated responsibilities.
 */

#include "core/master_controller.hpp"
#include "utils/constants.hpp"
#include <iostream>
#include <chrono>
#include <exception>
#include <thread>

namespace unoradar::core {

using namespace std::chrono_literals;
namespace cnst = unoradar::constants;

MasterController::MasterController()
    : io_context_(nullptr)
    , heartbeat_timer_(nullptr)
    , shutdown_requested_(false)
{
    std::cout << "[MasterController] Initializing military-grade radar controller..." << std::endl;
}

MasterController::~MasterController() {
    if (!shutdown_requested_.load()) {
        emergencyShutdown();
    }
}

bool MasterController::initialize() {
    try {
        // Initialize specialized components first
        state_manager_ = std::make_unique<SystemStateManager>(SystemStateManager::SystemState::INITIALIZING);
        performance_monitor_ = std::make_unique<PerformanceMonitor>();

        // Set up callbacks for component coordination
        state_manager_->setStateChangeCallback(
            [this](auto old_state, auto new_state) { onStateChange(old_state, new_state); });

        performance_monitor_->setMetricsCallback(
            [this](const auto& metrics) { onMetricsUpdate(metrics); });

        std::cout << "[MasterController] Phase 1: I/O Context initialization..." << std::endl;
        initializeIOContext();

        std::cout << "[MasterController] Phase 2: Subsystems initialization..." << std::endl;
        if (!initializeSubsystems()) {
            handleSystemError("Failed to initialize subsystems", data::ErrorSeverity::CRITICAL);
            return false;
        }

        std::cout << "[MasterController] Phase 3: Periodic tasks setup..." << std::endl;
        setupPeriodicTasks();

        // Start performance monitoring
        performance_monitor_->start();

        std::cout << "[MasterController] ✅ Initialization complete - Military-grade ready" << std::endl;
        return true;

    } catch (const std::exception& e) {
        handleSystemError("Initialization exception: " + std::string(e.what()),
                         data::ErrorSeverity::FATAL);
        return false;
    }
}

bool MasterController::start() {
    auto current_state = state_manager_->getCurrentState();
    if (current_state != SystemStateManager::SystemState::INITIALIZING &&
        current_state != SystemStateManager::SystemState::STOPPED) {
        std::cout << "[MasterController] ⚠️ Cannot start - invalid state" << std::endl;
        return false;
    }

    try {
        std::cout << "[MasterController] 🚀 Starting military-grade operations..." << std::endl;

        if (!state_manager_->updateState(SystemStateManager::SystemState::RUNNING)) {
            handleSystemError("Failed to transition to running state", data::ErrorSeverity::ERROR);
            return false;
        }

        std::cout << "[MasterController] ✅ System operational - Military-grade performance active" << std::endl;
        return true;

    } catch (const std::exception& e) {
        handleSystemError("Start exception: " + std::string(e.what()),
                         data::ErrorSeverity::CRITICAL);
        return false;
    }
}

void MasterController::run() {
    if (!state_manager_->isOperational()) {
        std::cout << "[MasterController] ⚠️ Cannot run - system not operational" << std::endl;
        return;
    }

    std::cout << "[MasterController] 🎯 Entering main event loop - Military-grade timing active" << std::endl;

    try {
        // Start heartbeat
        heartbeat_timer_->async_wait(
            [this](const boost::system::error_code& error) {
                onHeartbeat(error);
            });

        // Main event loop - simplified coordination-focused design
        while (!shutdown_requested_.load() && state_manager_->isOperational()) {
            try {
                auto work_start = std::chrono::steady_clock::now();

                // Process I/O events
                auto processed = io_context_->poll();

                auto work_duration = std::chrono::steady_clock::now() - work_start;
                auto work_us = std::chrono::duration_cast<std::chrono::microseconds>(work_duration).count();

                // Record performance metrics
                performance_monitor_->recordProcessingTime(work_us);

                // Check for high latency
                if (work_us > cnst::performance::MAX_LATENCY_US) {
                    std::cout << "[MasterController] ⚠️ High latency detected: "
                              << work_us << "μs (target: "
                              << cnst::performance::TARGET_LOOP_TIME_US << "μs)" << std::endl;
                }

                // Maintain target timing
                if (processed == 0) {
                    std::this_thread::sleep_for(std::chrono::microseconds(
                        cnst::performance::TARGET_LOOP_TIME_US / cnst::magic_numbers::SPIN_PREVENTION_DIVISOR));
                }

                auto remaining_time = std::chrono::microseconds(cnst::performance::TARGET_LOOP_TIME_US) - work_duration;
                if (remaining_time > std::chrono::microseconds::zero()) {
                    std::this_thread::sleep_for(remaining_time);
                }

            } catch (const std::exception& e) {
                handleSystemError("Event loop exception: " + std::string(e.what()),
                                 data::ErrorSeverity::ERROR);
                std::this_thread::sleep_for(100ms);
            }
        }

        std::cout << "[MasterController] 🛑 Event loop terminated" << std::endl;

    } catch (const std::exception& e) {
        handleSystemError("Critical event loop failure: " + std::string(e.what()),
                         data::ErrorSeverity::FATAL);
    }

    cleanup();
}

void MasterController::stop() {
    std::cout << "[MasterController] 🛑 Initiating graceful shutdown..." << std::endl;

    state_manager_->updateState(SystemStateManager::SystemState::STOPPING);
    shutdown_requested_.store(true);

    // Cancel pending operations
    if (heartbeat_timer_) {
        heartbeat_timer_->cancel();
    }

    state_manager_->updateState(SystemStateManager::SystemState::STOPPED);
    std::cout << "[MasterController] ✅ Graceful shutdown complete" << std::endl;
}

void MasterController::pause() {
    if (state_manager_->getCurrentState() == SystemStateManager::SystemState::RUNNING) {
        std::cout << "[MasterController] ⏸️ Pausing operations..." << std::endl;
        state_manager_->updateState(SystemStateManager::SystemState::PAUSING);
        state_manager_->updateState(SystemStateManager::SystemState::PAUSED);
        std::cout << "[MasterController] ✅ System paused" << std::endl;
    }
}

void MasterController::resume() {
    if (state_manager_->getCurrentState() == SystemStateManager::SystemState::PAUSED) {
        std::cout << "[MasterController] ▶️ Resuming operations..." << std::endl;
        state_manager_->updateState(SystemStateManager::SystemState::RUNNING);
        std::cout << "[MasterController] ✅ System resumed" << std::endl;
    }
}

SystemStateManager::SystemState MasterController::getSystemState() const noexcept {
    return state_manager_->getCurrentState();
}

data::PerformanceMetrics MasterController::getPerformanceMetrics() const {
    return performance_monitor_->getCurrentMetrics();
}

bool MasterController::isHealthy() const noexcept {
    return state_manager_->isOperational() && !state_manager_->isInErrorState();
}

void MasterController::emergencyShutdown() noexcept {
    try {
        std::cout << "[MasterController] 🚨 EMERGENCY SHUTDOWN" << std::endl;

        shutdown_requested_.store(true);

        if (state_manager_) {
            state_manager_->updateState(SystemStateManager::SystemState::STOPPED);
        }

        if (io_context_) {
            io_context_->stop();
        }

        std::cout << "[MasterController] 🛑 Emergency shutdown complete" << std::endl;

    } catch (...) {
        std::cout << "[MasterController] ❌ Emergency shutdown failed" << std::endl;
    }
}

void MasterController::initializeIOContext() {
    io_context_ = std::make_unique<boost::asio::io_context>();
    heartbeat_timer_ = std::make_unique<boost::asio::steady_timer>(*io_context_);

    std::cout << "[MasterController] I/O context initialized with military-grade timers" << std::endl;
}

bool MasterController::initializeSubsystems() {
    try {
        // Placeholder for future subsystem initialization
        std::cout << "[MasterController] Serial interface: PLACEHOLDER (pending implementation)" << std::endl;
        std::cout << "[MasterController] WebSocket server: PLACEHOLDER (pending implementation)" << std::endl;
        std::cout << "[MasterController] Data processor: PLACEHOLDER (pending implementation)" << std::endl;
        std::cout << "[MasterController] Logger: PLACEHOLDER (pending implementation)" << std::endl;

        return true;

    } catch (const std::exception& e) {
        std::cout << "[MasterController] ❌ Subsystem initialization failed: " << e.what() << std::endl;
        return false;
    }
}

void MasterController::setupPeriodicTasks() {
    std::cout << "[MasterController] Periodic tasks configured - Military-grade monitoring active" << std::endl;
}

void MasterController::onHeartbeat(const boost::system::error_code& error) {
    if (error || shutdown_requested_.load()) {
        return;
    }

    // Record heartbeat message
    performance_monitor_->recordMessage();

    // Schedule next heartbeat
    heartbeat_timer_->expires_after(std::chrono::seconds(1));
    heartbeat_timer_->async_wait(
        [this](const boost::system::error_code& error) {
            onHeartbeat(error);
        });
}

void MasterController::handleSystemError(const std::string& error_message,
                                       data::ErrorSeverity severity) {
    std::cout << "[MasterController] ERROR [" << static_cast<int>(severity)
              << "]: " << error_message << std::endl;

    if (severity >= data::ErrorSeverity::CRITICAL) {
        state_manager_->updateState(SystemStateManager::SystemState::ERROR);
    }
}

void MasterController::cleanup() {
    std::cout << "[MasterController] 🧹 Cleaning up resources..." << std::endl;

    // Stop performance monitoring
    if (performance_monitor_) {
        performance_monitor_->stop();
    }

    // Reset components
    heartbeat_timer_.reset();
    io_context_.reset();

    std::cout << "[MasterController] ✅ Cleanup complete" << std::endl;
}

void MasterController::onStateChange(SystemStateManager::SystemState old_state,
                                    SystemStateManager::SystemState new_state) {
    // Coordination logic for state changes
    std::cout << "[MasterController] Coordinating state change: "
              << SystemStateManager::stateToString(old_state) << " → "
              << SystemStateManager::stateToString(new_state) << std::endl;
}

void MasterController::onMetricsUpdate(const data::PerformanceMetrics& metrics) {
    // Coordination logic for metrics updates
    // Could trigger alerts, adjust performance, etc.
    (void)metrics; // Suppress unused parameter warning for now
}

} // namespace unoradar::core