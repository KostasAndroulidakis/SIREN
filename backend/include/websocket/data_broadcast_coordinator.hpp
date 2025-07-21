/**
 * @file data_broadcast_coordinator.hpp
 * @brief WebSocket data broadcast coordination - MISRA C++ compliant
 * @author KostasAndroulidakis
 * @date 2025
 *
 * Single Responsibility: Coordinate data broadcasting between components ONLY
 *
 * RESPONSIBILITIES:
 * - Coordinate sonar data broadcasting
 * - Coordinate performance metrics broadcasting
 * - Manage session-to-broadcaster communication
 * - Handle broadcast state validation
 *
 * NOT RESPONSIBLE FOR:
 * - Server lifecycle (handled by ServerLifecycleManager)
 * - Event handling (handled by ServerEventHandler)
 * - Session management (handled by SessionManager)
 * - Message broadcasting (handled by MessageBroadcaster)
 *
 * MISRA C++ Compliance:
 * - Rule 5.0.1: All constants defined, no magic numbers
 * - Rule 21.2.1: RAII for all resources
 * - Rule 8.4.1: Single responsibility per class
 */

#pragma once

#include <memory>
#include <atomic>

#include "data/sonar_types.hpp"

namespace siren::websocket {

// Forward declarations
class SessionManager;
class MessageBroadcaster;

/**
 * @brief WebSocket data broadcast coordinator with single responsibility
 *
 * Coordinates data broadcasting between session manager and message broadcaster
 * without mixing concerns with server lifecycle or event handling.
 *
 * Single Responsibility: Data broadcast coordination
 */
class DataBroadcastCoordinator {
public:
    /**
     * @brief Constructor - RAII initialization
     * @param session_manager Reference to session manager for active sessions
     * @param message_broadcaster Reference to message broadcaster
     */
    explicit DataBroadcastCoordinator(std::shared_ptr<SessionManager>& session_manager,
                                     std::unique_ptr<MessageBroadcaster>& message_broadcaster);

    /**
     * @brief Destructor - RAII cleanup
     */
    ~DataBroadcastCoordinator() = default;

    // MISRA C++ Rule 12.1.1: Disable copy/move for resource management
    DataBroadcastCoordinator(const DataBroadcastCoordinator&) = delete;
    DataBroadcastCoordinator& operator=(const DataBroadcastCoordinator&) = delete;
    DataBroadcastCoordinator(DataBroadcastCoordinator&&) = delete;
    DataBroadcastCoordinator& operator=(DataBroadcastCoordinator&&) = delete;

    /**
     * @brief Coordinate sonar data broadcast (SSOT for sonar data broadcasting)
     * @param data Sonar data point to broadcast
     * @param running Reference to server running state for validation
     */
    void broadcastSonarData(const data::SonarDataPoint& data, 
                           const std::atomic<bool>& running);

    /**
     * @brief Coordinate performance metrics broadcast (SSOT for metrics broadcasting)
     * @param metrics Performance metrics to broadcast
     * @param running Reference to server running state for validation
     */
    void broadcastPerformanceMetrics(const data::PerformanceMetrics& metrics,
                                   const std::atomic<bool>& running);

private:
    // Component references - not owned, avoid circular dependencies
    std::shared_ptr<SessionManager>& session_manager_;
    std::unique_ptr<MessageBroadcaster>& message_broadcaster_;
};

} // namespace siren::websocket