/**
 * @file master_controller.cpp
 * @brief Implementation of military-grade master controller
 * @author unoRadar Project
 * @date 2025
 *
 * Heart of the radar system - coordinates all components with deterministic timing.
 */

#include "core/master_controller.hpp"
#include <iostream>
#include <chrono>
#include <exception>

namespace unoradar::core {

using namespace std::chrono_literals;
namespace cfg = unoradar::config;
namespace cnst = unoradar::constants;

MasterController::MasterController()
    : io_context_(nullptr)
    , heartbeat_timer_(nullptr)
    , metrics_timer_(nullptr)
    , system_state_(SystemState::INITIALIZING)
    , shutdown_requested_(false)
    , healthy_(false)
    , last_metrics_update_(std::chrono::steady_clock::now())
{
    std::cout << "[MasterController] Initializing military-grade radar controller..." << std::endl;
}

MasterController::~MasterController() {
    if (system_state_.load() != SystemState::STOPPED) {
        emergencyShutdown();
    }
}

bool MasterController::initialize() {
    try {
        updateSystemState(SystemState::INITIALIZING);

        std::cout << "[MasterController] Phase 1: I/O Context initialization..." << std::endl;
        initializeIOContext();

        std::cout << "[MasterController] Phase 2: Subsystems initialization..." << std::endl;
        if (!initializeSubsystems()) {
            handleSystemError("Failed to initialize subsystems", data::ErrorSeverity::CRITICAL);
            return false;
        }

        std::cout << "[MasterController] Phase 3: Periodic tasks setup..." << std::endl;
        setupPeriodicTasks();

        healthy_.store(true);
        std::cout << "[MasterController] ✅ Initialization complete - Military-grade ready" << std::endl;

        return true;

    } catch (const std::exception& e) {
        handleSystemError("Initialization exception: " + std::string(e.what()),
                         data::ErrorSeverity::FATAL);
        return false;
    }
}

bool MasterController::start() {
    if (system_state_.load() != SystemState::INITIALIZING &&
        system_state_.load() != SystemState::STOPPED) {
        std::cout << "[MasterController] ⚠️ Cannot start - invalid state" << std::endl;
        return false;
    }

    try {
        std::cout << "[MasterController] 🚀 Starting military-grade operations..." << std::endl;

        updateSystemState(SystemState::RUNNING);
        startWorkerThreads();

        std::cout << "[MasterController] ✅ System operational - Military-grade performance active" << std::endl;
        return true;

    } catch (const std::exception& e) {
        handleSystemError("Start exception: " + std::string(e.what()),
                         data::ErrorSeverity::CRITICAL);
        return false;
    }
}

void MasterController::run() {
    if (system_state_.load() != SystemState::RUNNING) {
        std::cout << "[MasterController] ⚠️ Cannot run - system not started" << std::endl;
        return;
    }

    std::cout << "[MasterController] 🎯 Entering main event loop - Military-grade timing active" << std::endl;

    try {
        // Start periodic heartbeat and metrics collection
        heartbeat_timer_->async_wait(
            [this](const boost::system::error_code& error) {
                onHeartbeat(error);
            });

        metrics_timer_->async_wait(
            [this](const boost::system::error_code& error) {
                onMetricsUpdate(error);
            });

        // Main event loop - runs until shutdown
        while (!shutdown_requested_.load() && healthy_.load()) {
            try {
                // Run I/O operations with military-grade timing
                auto work_start = std::chrono::steady_clock::now();

                // Process events with timeout to ensure responsiveness
                auto processed = io_context_->poll();

                auto work_duration = std::chrono::steady_clock::now() - work_start;
                auto work_us = std::chrono::duration_cast<std::chrono::microseconds>(work_duration).count();

                // Military-grade performance monitoring
                if (work_us > cnst::performance::MAX_LATENCY_US) {
                    std::cout << "[MasterController] ⚠️ High latency detected: "
                              << work_us << "μs (target: "
                              << cnst::performance::TARGET_LOOP_TIME_US << "μs)" << std::endl;
                }

                // If no work was done, sleep briefly to prevent CPU spinning
                if (processed == 0) {
                    std::this_thread::sleep_for(std::chrono::microseconds(cnst::performance::TARGET_LOOP_TIME_US / 10));
                }

                // Maintain target loop timing
                auto remaining_time = std::chrono::microseconds(cnst::performance::TARGET_LOOP_TIME_US) - work_duration;
                if (remaining_time > std::chrono::microseconds::zero()) {
                    std::this_thread::sleep_for(remaining_time);
                }

            } catch (const std::exception& e) {
                handleSystemError("Event loop exception: " + std::string(e.what()),
                                 data::ErrorSeverity::ERROR);

                // Brief pause before continuing to prevent error floods
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

    updateSystemState(SystemState::STOPPING);
    shutdown_requested_.store(true);

    // Cancel all pending operations
    if (heartbeat_timer_) heartbeat_timer_->cancel();
    if (metrics_timer_) metrics_timer_->cancel();

    // Stop worker threads
    stopWorkerThreads();

    updateSystemState(SystemState::STOPPED);
    std::cout << "[MasterController] ✅ Graceful shutdown complete" << std::endl;
}

void MasterController::pause() {
    if (system_state_.load() == SystemState::RUNNING) {
        std::cout << "[MasterController] ⏸️ Pausing operations..." << std::endl;
        updateSystemState(SystemState::PAUSING);
        // Implementation would pause data collection while maintaining connections
        updateSystemState(SystemState::PAUSED);
        std::cout << "[MasterController] ✅ System paused" << std::endl;
    }
}

void MasterController::resume() {
    if (system_state_.load() == SystemState::PAUSED) {
        std::cout << "[MasterController] ▶️ Resuming operations..." << std::endl;
        updateSystemState(SystemState::RUNNING);
        std::cout << "[MasterController] ✅ System resumed" << std::endl;
    }
}

MasterController::SystemState MasterController::getSystemState() const noexcept {
    return system_state_.load();
}

data::PerformanceMetrics MasterController::getPerformanceMetrics() const {
    std::lock_guard<std::mutex> lock(metrics_mutex_);
    return current_metrics_;
}

bool MasterController::isHealthy() const noexcept {
    return healthy_.load() && system_state_.load() == SystemState::RUNNING;
}

void MasterController::emergencyShutdown() noexcept {
    try {
        std::cout << "[MasterController] 🚨 EMERGENCY SHUTDOWN" << std::endl;

        shutdown_requested_.store(true);
        healthy_.store(false);
        updateSystemState(SystemState::STOPPED);

        if (io_context_) {
            io_context_->stop();
        }

        // Force stop worker threads
        for (auto& thread : worker_threads_) {
            if (thread.joinable()) {
                thread.join();
            }
        }
        worker_threads_.clear();

        std::cout << "[MasterController] 🛑 Emergency shutdown complete" << std::endl;

    } catch (...) {
        // Cannot throw from noexcept function
        std::cout << "[MasterController] ❌ Emergency shutdown failed" << std::endl;
    }
}

void MasterController::initializeIOContext() {
    io_context_ = std::make_unique<boost::asio::io_context>();

    heartbeat_timer_ = std::make_unique<boost::asio::steady_timer>(*io_context_);
    metrics_timer_ = std::make_unique<boost::asio::steady_timer>(*io_context_);

    std::cout << "[MasterController] I/O context initialized with military-grade timers" << std::endl;
}

bool MasterController::initializeSubsystems() {
    try {
        // For now, we'll implement placeholder initialization
        // Real implementations will be added in subsequent components

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
    // Initialize performance metrics
    current_metrics_ = data::PerformanceMetrics{};
    current_metrics_.serial_status = data::SerialStatus::DISCONNECTED;

    std::cout << "[MasterController] Periodic tasks configured - Military-grade monitoring active" << std::endl;
}

void MasterController::onHeartbeat(const boost::system::error_code& error) {
    if (error || shutdown_requested_.load()) {
        return; // Timer was cancelled or shutdown requested
    }

    // Perform health check
    performHealthCheck();

    // Schedule next heartbeat
    heartbeat_timer_->expires_after(std::chrono::seconds(1));
    heartbeat_timer_->async_wait(
        [this](const boost::system::error_code& error) {
            onHeartbeat(error);
        });
}

void MasterController::onMetricsUpdate(const boost::system::error_code& error) {
    if (error || shutdown_requested_.load()) {
        return; // Timer was cancelled or shutdown requested
    }

    updatePerformanceMetrics();

    // Schedule next metrics update
    metrics_timer_->expires_after(std::chrono::seconds(cnst::performance::MONITORING_INTERVAL_SEC));
    metrics_timer_->async_wait(
        [this](const boost::system::error_code& error) {
            onMetricsUpdate(error);
        });
}

void MasterController::updateSystemState(SystemState new_state) {
    auto old_state = system_state_.exchange(new_state);

    if (old_state != new_state) {
        std::cout << "[MasterController] State transition: "
                  << static_cast<int>(old_state) << " → "
                  << static_cast<int>(new_state) << std::endl;
    }
}

void MasterController::handleSystemError(const std::string& error_message,
                                       data::ErrorSeverity severity) {
    std::cout << "[MasterController] ERROR [" << static_cast<int>(severity)
              << "]: " << error_message << std::endl;

    if (severity >= data::ErrorSeverity::CRITICAL) {
        healthy_.store(false);
        updateSystemState(SystemState::ERROR);
    }
}

void MasterController::cleanup() {
    std::cout << "[MasterController] 🧹 Cleaning up resources..." << std::endl;

    stopWorkerThreads();

    // Reset all components
    heartbeat_timer_.reset();
    metrics_timer_.reset();
    io_context_.reset();

    std::cout << "[MasterController] ✅ Cleanup complete" << std::endl;
}

void MasterController::startWorkerThreads() {
    const auto thread_count = cnst::performance::THREAD_POOL_SIZE;

    std::cout << "[MasterController] Starting " << thread_count << " worker threads..." << std::endl;

    worker_threads_.reserve(thread_count);
    for (size_t i = 0; i < thread_count; ++i) {
        worker_threads_.emplace_back(&MasterController::workerThreadFunction, this);
    }

    std::cout << "[MasterController] ✅ Worker threads started" << std::endl;
}

void MasterController::stopWorkerThreads() {
    std::cout << "[MasterController] Stopping worker threads..." << std::endl;

    if (io_context_) {
        io_context_->stop();
    }

    for (auto& thread : worker_threads_) {
        if (thread.joinable()) {
            thread.join();
        }
    }

    worker_threads_.clear();
    std::cout << "[MasterController] ✅ Worker threads stopped" << std::endl;
}

void MasterController::workerThreadFunction() {
    try {
        // Worker threads process I/O operations
        if (io_context_) {
            io_context_->run();
        }
    } catch (const std::exception& e) {
        handleSystemError("Worker thread exception: " + std::string(e.what()),
                         data::ErrorSeverity::ERROR);
    }
}

void MasterController::updatePerformanceMetrics() {
    std::lock_guard<std::mutex> lock(metrics_mutex_);

    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
        now - last_metrics_update_).count();

    // Update metrics (placeholder values for now)
    current_metrics_.messages_per_second = 0; // Will be updated by actual components
    current_metrics_.avg_latency_us = cnst::performance::TARGET_LOOP_TIME_US;
    current_metrics_.max_latency_us = cnst::performance::TARGET_LOOP_TIME_US;
    current_metrics_.memory_usage_bytes = 0; // Will be calculated
    current_metrics_.active_connections = 0; // Will be updated by WebSocket server
    current_metrics_.timestamp = now;

    last_metrics_update_ = now;
}

void MasterController::performHealthCheck() {
    // Basic health check - will be expanded with actual component health
    bool overall_health = true;

    // Check system state
    auto state = system_state_.load();
    if (state == SystemState::ERROR || state == SystemState::STOPPED) {
        overall_health = false;
    }

    // Check if we're responsive (not hanging)
    auto now = std::chrono::steady_clock::now();
    auto metrics_age = std::chrono::duration_cast<std::chrono::seconds>(
        now - last_metrics_update_).count();

    if (metrics_age > 5) { // No metrics update in 5 seconds = potential hang
        overall_health = false;
    }

    healthy_.store(overall_health);

    if (!overall_health) {
        std::cout << "[MasterController] ⚠️ Health check failed" << std::endl;
    }
}

} // namespace unoradar::core