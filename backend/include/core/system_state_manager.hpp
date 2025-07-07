/**
 * @file system_state_manager.hpp
 * @brief System state management component for unoRadar
 * @author unoRadar Project
 * @date 2025
 *
 * Single responsibility: Manage system operational states with thread-safe transitions.
 */

#pragma once

#include <atomic>
#include <functional>
#include <string>

namespace unoradar::core {

/**
 * @brief System state manager with single responsibility
 *
 * Handles only system state transitions and notifications.
 * Thread-safe atomic state management.
 */
class SystemStateManager {
public:
    /// System operational states
    enum class SystemState {
        INITIALIZING,   ///< System starting up
        RUNNING,        ///< Normal operation
        PAUSING,        ///< Graceful pause in progress
        PAUSED,         ///< System paused
        STOPPING,       ///< Shutdown in progress
        STOPPED,        ///< System stopped
        ERROR           ///< Critical error state
    };

    /// State change callback type
    using StateChangeCallback = std::function<void(SystemState old_state, SystemState new_state)>;

    /**
     * @brief Constructor with initial state
     */
    explicit SystemStateManager(SystemState initial_state = SystemState::INITIALIZING);

    /**
     * @brief Get current system state
     */
    SystemState getCurrentState() const noexcept;

    /**
     * @brief Update system state atomically
     * @param new_state New state to transition to
     * @return true if transition was valid and successful
     */
    bool updateState(SystemState new_state);

    /**
     * @brief Check if system is operational
     */
    bool isOperational() const noexcept;

    /**
     * @brief Check if system is in error state
     */
    bool isInErrorState() const noexcept;

    /**
     * @brief Set state change callback
     * @param callback Function to call on state changes
     */
    void setStateChangeCallback(StateChangeCallback callback);

    /**
     * @brief Get string representation of state
     * @param state State to convert to string
     */
    static std::string stateToString(SystemState state);

private:
    std::atomic<SystemState> current_state_;
    StateChangeCallback state_change_callback_;

    /**
     * @brief Validate state transition
     * @param from Current state
     * @param to Target state
     * @return true if transition is valid
     */
    bool isValidTransition(SystemState from, SystemState to) const;
};

} // namespace unoradar::core