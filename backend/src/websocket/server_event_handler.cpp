/**
 * @file server_event_handler.cpp
 * @brief Implementation of WebSocket server event handler - MISRA C++ compliant
 * @author KostasAndroulidakis
 * @date 2025
 *
 * Single Responsibility: WebSocket server event processing
 */

#include "websocket/server_event_handler.hpp"
#include "websocket/session.hpp"
#include "websocket/session_manager.hpp"
#include "websocket/statistics_collector.hpp"
#include "utils/error_handler.hpp"
#include "constants/message.hpp"
#include <iostream>

namespace siren::websocket {

namespace cnst = siren::constants;

// SSOT for event handler constants (MISRA C++ Rule 5.0.1)
namespace {
    constexpr const char* COMPONENT_NAME = "ServerEventHandler";
}

ServerEventHandler::ServerEventHandler(std::shared_ptr<SessionManager> session_manager,
                                     std::shared_ptr<StatisticsCollector> statistics_collector)
    : session_manager_(session_manager)
    , statistics_collector_(statistics_collector)
    , connection_callback_(nullptr)
{
    std::cout << "[" << COMPONENT_NAME << "] Initializing server event handler" << std::endl;
}

void ServerEventHandler::onConnectionAccepted(tcp::socket socket,
                                              std::weak_ptr<WebSocketServer> server_weak_ptr) {
    try {
        // Get session manager
        auto session_manager = session_manager_.lock();
        if (!session_manager) {
            utils::ErrorHandler::handleSystemError(COMPONENT_NAME,
                                                  "Session manager unavailable for connection acceptance",
                                                  data::ErrorSeverity::ERROR);
            return;
        }

        // Create new session through session manager
        auto session = session_manager->createSession(std::move(socket), server_weak_ptr);
        if (!session) {
            utils::ErrorHandler::handleSystemError(COMPONENT_NAME,
                                                  "Failed to create session for accepted connection",
                                                  data::ErrorSeverity::ERROR);
            return;
        }

        // Start the session
        session->start();

        // Log connection acceptance
        std::cout << cnst::message::websocket_status::SERVER_PREFIX << " "
                  << cnst::message::websocket_status::NEW_CLIENT_CONNECTED << ": " << session->getClientEndpoint()
                  << " " << cnst::message::websocket_status::TOTAL_CLIENTS << " " << getActiveConnections() << ")" << std::endl;

        // Update statistics
        if (auto stats_collector = statistics_collector_.lock()) {
            stats_collector->recordConnectionAccepted();
        }

    } catch (const std::exception& e) {
        utils::ErrorHandler::handleException(COMPONENT_NAME,
                                           "connection acceptance processing", e,
                                           data::ErrorSeverity::ERROR);
    }
}

void ServerEventHandler::onConnectionError(const std::string& error_message, beast::error_code ec) {
    try {
        // Handle connection errors with appropriate severity
        if (ec) {
            utils::ErrorHandler::handleBoostError(cnst::message::websocket_status::SERVER_PREFIX,
                                                 error_message, ec,
                                                 data::ErrorSeverity::ERROR);
        } else {
            utils::ErrorHandler::handleSystemError(cnst::message::websocket_status::SERVER_PREFIX,
                                                  error_message,
                                                  data::ErrorSeverity::ERROR);
        }

    } catch (const std::exception& e) {
        utils::ErrorHandler::handleException(COMPONENT_NAME,
                                           "connection error processing", e,
                                           data::ErrorSeverity::WARNING);
    }
}

void ServerEventHandler::onSessionEvent(const std::string& endpoint, bool connected) {
    try {
        // Notify external callback if set (SSOT for external notification)
        if (connection_callback_) {
            connection_callback_(endpoint, connected);
        }

        // Log session event for debugging
        std::cout << "[" << COMPONENT_NAME << "] Session event: " << endpoint
                  << " " << (connected ? "connected" : "disconnected") << std::endl;

    } catch (const std::exception& e) {
        utils::ErrorHandler::handleException(COMPONENT_NAME,
                                           "session event processing for " + endpoint, e,
                                           data::ErrorSeverity::WARNING);
    }
}

void ServerEventHandler::onBroadcastCompleted(size_t sessions_reached) {
    try {
        // Log broadcast completion for debugging
        std::cout << "[" << COMPONENT_NAME << "] Broadcast completed, reached "
                  << sessions_reached << " sessions" << std::endl;

        // Future extension point: Additional metrics tracking for broadcasts
        // This method provides a clean extension point for broadcast analytics

    } catch (const std::exception& e) {
        utils::ErrorHandler::handleException(COMPONENT_NAME,
                                           "broadcast completion processing", e,
                                           data::ErrorSeverity::WARNING);
    }
}

void ServerEventHandler::setConnectionCallback(ConnectionCallback callback) {
    connection_callback_ = std::move(callback);
}

size_t ServerEventHandler::getActiveConnections() const {
    // Get connection count from session manager
    if (auto session_manager = session_manager_.lock()) {
        return session_manager->getActiveSessionCount();
    }
    return 0;
}

} // namespace siren::websocket