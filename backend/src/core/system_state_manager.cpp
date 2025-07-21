/**
 * @file system_state_manager.cpp
 * @brief Implementation of system state manager
 * @author KostasAndroulidakis
 * @date 2025
 */

#include "core/system_state_manager.hpp"
#include <iostream>

namespace siren::core {

SystemStateManager::SystemStateManager(SystemState initial_state)
    : current_state_(initial_state)
{
}

SystemStateManager::SystemState SystemStateManager::getCurrentState() const noexcept {
    return current_state_.load();
}

bool SystemStateManager::updateState(SystemState new_state) {
    SystemState old_state = current_state_.load();

    // Validate transition
    if (!isValidTransition(old_state, new_state)) {
        std::cout << "[SystemStateManager] ❌ Invalid state transition: "
                  << stateToString(old_state) << " → " << stateToString(new_state) << std::endl;
        return false;
    }

    // Perform atomic state change
    if (current_state_.compare_exchange_strong(old_state, new_state)) {
        std::cout << "[SystemStateManager] State transition: "
                  << stateToString(old_state) << " → " << stateToString(new_state) << std::endl;

        // Notify callback if set
        if (state_change_callback_) {
            state_change_callback_(old_state, new_state);
        }

        return true;
    }

    return false; // State changed between load and compare_exchange
}

bool SystemStateManager::isOperational() const noexcept {
    SystemState state = current_state_.load();
    return state == SystemState::RUNNING || state == SystemState::PAUSED;
}

bool SystemStateManager::isInErrorState() const noexcept {
    return current_state_.load() == SystemState::ERROR;
}

void SystemStateManager::setStateChangeCallback(StateChangeCallback callback) {
    state_change_callback_ = std::move(callback);
}

std::string SystemStateManager::stateToString(SystemState state) {
    switch (state) {
        case SystemState::INITIALIZING: return "INITIALIZING";
        case SystemState::RUNNING: return "RUNNING";
        case SystemState::PAUSING: return "PAUSING";
        case SystemState::PAUSED: return "PAUSED";
        case SystemState::STOPPING: return "STOPPING";
        case SystemState::STOPPED: return "STOPPED";
        case SystemState::ERROR: return "ERROR";
        default: return "UNKNOWN";
    }
}

bool SystemStateManager::isValidTransition(SystemState from, SystemState to) const {
    // Define valid state transitions
    switch (from) {
        case SystemState::INITIALIZING:
            return to == SystemState::RUNNING || to == SystemState::ERROR || to == SystemState::STOPPED;

        case SystemState::RUNNING:
            return to == SystemState::PAUSING || to == SystemState::STOPPING || to == SystemState::ERROR || to == SystemState::STOPPED;

        case SystemState::PAUSING:
            return to == SystemState::PAUSED || to == SystemState::RUNNING || to == SystemState::ERROR;

        case SystemState::PAUSED:
            return to == SystemState::RUNNING || to == SystemState::STOPPING || to == SystemState::ERROR;

        case SystemState::STOPPING:
            return to == SystemState::STOPPED || to == SystemState::ERROR;

        case SystemState::STOPPED:
            return to == SystemState::INITIALIZING; // Allow restart

        case SystemState::ERROR:
            return to == SystemState::STOPPED || to == SystemState::INITIALIZING; // Allow recovery

        default:
            return false;
    }
}

} // namespace siren::core