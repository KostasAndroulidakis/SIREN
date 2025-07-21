/**
 * @file data_broadcast_coordinator.cpp
 * @brief Implementation of WebSocket data broadcast coordinator - MISRA C++ compliant
 * @author KostasAndroulidakis
 * @date 2025
 *
 * Single Responsibility: Data broadcast coordination
 */

#include "websocket/data_broadcast_coordinator.hpp"
#include "websocket/session_manager.hpp"
#include "websocket/message_broadcaster.hpp"
#include <iostream>

namespace siren::websocket {

// SSOT for broadcast coordinator constants (MISRA C++ Rule 5.0.1)
namespace {
    constexpr const char* COMPONENT_NAME = "DataBroadcastCoordinator";
}

DataBroadcastCoordinator::DataBroadcastCoordinator(
    std::unique_ptr<SessionManager>& session_manager,
    std::unique_ptr<MessageBroadcaster>& message_broadcaster)
    : session_manager_(session_manager)
    , message_broadcaster_(message_broadcaster)
{
    std::cout << "[" << COMPONENT_NAME << "] Initializing data broadcast coordinator" << std::endl;
}

void DataBroadcastCoordinator::broadcastSonarData(const data::SonarDataPoint& data,
                                                  const std::atomic<bool>& running) {
    if (!running.load() || !message_broadcaster_) {
        return;
    }

    // Get active sessions from session manager
    auto active_sessions = session_manager_->getActiveSessions();

    // Broadcast through message broadcaster
    message_broadcaster_->broadcastSonarData(data, active_sessions);
}

void DataBroadcastCoordinator::broadcastPerformanceMetrics(const data::PerformanceMetrics& metrics,
                                                          const std::atomic<bool>& running) {
    if (!running.load() || !message_broadcaster_) {
        return;
    }

    // Get active sessions from session manager
    auto active_sessions = session_manager_->getActiveSessions();

    // Broadcast through message broadcaster
    message_broadcaster_->broadcastPerformanceMetrics(metrics, active_sessions);
}

} // namespace siren::websocket