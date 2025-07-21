/**
 * @file server.cpp
 * @brief Implementation of military-grade WebSocket server
 * @author KostasAndroulidakis
 * @date 2025
 *
 * High-performance WebSocket server for real-time sonar data streaming
 * with military-grade reliability and error handling.
 */

#include "websocket/server.hpp"
#include "utils/json_serializer.hpp"
#include "constants/message.hpp"
#include "utils/error_handler.hpp"
#include <iostream>
#include <chrono>

namespace siren::websocket {

namespace http = beast::http;  // Local alias for HTTP functionality
namespace cnst = siren::constants;  // Local alias for constants


// WebSocketServer Implementation

WebSocketServer::WebSocketServer(boost::asio::io_context& io_context, uint16_t port)
    : io_context_(io_context)
    , port_(port)
    , running_(false)
    , shutdown_requested_(false)
{
    std::cout << cnst::message::websocket_status::SERVER_PREFIX << " "
              << cnst::message::websocket_status::INITIALIZING_SERVER << " " << port << std::endl;


    // Create specialized managers - SRP compliant components
    connection_acceptor_ = std::make_unique<ConnectionAcceptor>(io_context_, port_);
    session_manager_ = std::make_unique<SessionManager>();
    message_broadcaster_ = std::make_unique<MessageBroadcaster>();
    statistics_collector_ = std::make_unique<StatisticsCollector>();
    event_handler_ = std::make_unique<ServerEventHandler>(session_manager_, statistics_collector_);
}

WebSocketServer::~WebSocketServer() {
    if (running_.load()) {
        stop();
    }
}

bool WebSocketServer::initialize() {
    try {
        // Initialize connection acceptor
        if (!connection_acceptor_->initialize()) {
            utils::ErrorHandler::handleInitializationError(cnst::message::websocket_status::SERVER_PREFIX,
                                                            "connection_acceptor",
                                                            "Connection acceptor initialization failed");
            return false;
        }

        // Initialize message broadcaster
        if (!message_broadcaster_->initialize()) {
            utils::ErrorHandler::handleInitializationError(cnst::message::websocket_status::SERVER_PREFIX,
                                                            "message_broadcaster",
                                                            "Message broadcaster initialization failed");
            return false;
        }

        // Initialize statistics collector
        if (!statistics_collector_->initialize()) {
            utils::ErrorHandler::handleInitializationError(cnst::message::websocket_status::SERVER_PREFIX,
                                                            "statistics_collector",
                                                            "Statistics collector initialization failed");
            return false;
        }

        // Set up component callbacks - delegate to event handler
        connection_acceptor_->setAcceptCallback(
            [this](tcp::socket socket) {
                event_handler_->onConnectionAccepted(std::move(socket), weak_from_this());
            });

        connection_acceptor_->setErrorCallback(
            [this](const std::string& error_message, beast::error_code ec) {
                event_handler_->onConnectionError(error_message, ec);
            });

        session_manager_->setSessionCallback(
            [this](const std::string& endpoint, bool connected) {
                event_handler_->onSessionEvent(endpoint, connected);
            });

        message_broadcaster_->setBroadcastCallback(
            [this](size_t sessions_reached) {
                event_handler_->onBroadcastCompleted(sessions_reached);
            });

        std::cout << cnst::message::websocket_status::SERVER_PREFIX << " "
                  << cnst::message::websocket_status::SERVER_INITIALIZED << " " << port_ << std::endl;
        return true;

    } catch (const std::exception& e) {
        utils::ErrorHandler::handleException(cnst::message::websocket_status::SERVER_PREFIX,
                                              cnst::message::websocket_status::INIT_FAILED_EXCEPTION,
                                              e, data::ErrorSeverity::FATAL);
        return false;
    }
}

bool WebSocketServer::start() {
    if (running_.load()) {
        utils::ErrorHandler::handleSystemError(cnst::message::websocket_status::SERVER_PREFIX,
                                                cnst::message::websocket_status::SERVER_ALREADY_RUNNING,
                                                data::ErrorSeverity::WARNING);
        return true;
    }

    try {
        // Start connection acceptor
        if (!connection_acceptor_->start()) {
            utils::ErrorHandler::handleSystemError(cnst::message::websocket_status::SERVER_PREFIX,
                                                    "Connection acceptor start failed",
                                                    data::ErrorSeverity::ERROR);
            return false;
        }

        // Start message broadcaster
        if (!message_broadcaster_->start()) {
            utils::ErrorHandler::handleSystemError(cnst::message::websocket_status::SERVER_PREFIX,
                                                    "Message broadcaster start failed",
                                                    data::ErrorSeverity::ERROR);
            connection_acceptor_->stop();
            return false;
        }

        // Start statistics collector
        if (!statistics_collector_->start()) {
            utils::ErrorHandler::handleSystemError(cnst::message::websocket_status::SERVER_PREFIX,
                                                    "Statistics collector start failed",
                                                    data::ErrorSeverity::ERROR);
            connection_acceptor_->stop();
            message_broadcaster_->stop();
            return false;
        }

        running_.store(true);

        std::cout << cnst::message::websocket_status::SERVER_PREFIX << " "
                  << cnst::message::websocket_status::SERVER_STARTED << " " << port_ << std::endl;

        return true;

    } catch (const std::exception& e) {
        utils::ErrorHandler::handleException(cnst::message::websocket_status::SERVER_PREFIX,
                                              cnst::message::websocket_status::START_FAILED_EXCEPTION,
                                              e, data::ErrorSeverity::FATAL);
        return false;
    }
}

void WebSocketServer::stop() {
    if (!running_.load()) {
        return;
    }

    std::cout << cnst::message::websocket_status::SERVER_PREFIX << " "
              << cnst::message::websocket_status::STOPPING_SERVER << std::endl;

    shutdown_requested_.store(true);
    running_.store(false);

    // Stop specialized managers
    if (connection_acceptor_) {
        connection_acceptor_->stop();
    }

    if (message_broadcaster_) {
        message_broadcaster_->stop();
    }

    if (session_manager_) {
        session_manager_->closeAllSessions();
    }

    if (statistics_collector_) {
        statistics_collector_->stop();
    }

    std::cout << cnst::message::websocket_status::SERVER_PREFIX << " "
              << cnst::message::websocket_status::SERVER_STOPPED << std::endl;
}

bool WebSocketServer::isRunning() const noexcept {
    return running_.load();
}

void WebSocketServer::broadcastSonarData(const data::SonarDataPoint& data) {
    if (!running_.load() || !message_broadcaster_) {
        return;
    }

    // Get active sessions from session manager
    auto active_sessions = session_manager_->getActiveSessions();

    // Broadcast through message broadcaster
    message_broadcaster_->broadcastSonarData(data, active_sessions);

}

void WebSocketServer::broadcastPerformanceMetrics(const data::PerformanceMetrics& metrics) {
    if (!running_.load() || !message_broadcaster_) {
        return;
    }

    // Get active sessions from session manager
    auto active_sessions = session_manager_->getActiveSessions();

    // Broadcast through message broadcaster
    message_broadcaster_->broadcastPerformanceMetrics(metrics, active_sessions);
}

size_t WebSocketServer::getActiveConnections() const noexcept {
    return session_manager_ ? session_manager_->getActiveSessionCount() : 0;
}

data::WebSocketStatistics WebSocketServer::getStatistics() const {
    if (statistics_collector_) {
        return statistics_collector_->getStatistics(getActiveConnections());
    }
    return data::WebSocketStatistics{};
}

void WebSocketServer::setConnectionCallback(ConnectionCallback callback) {
    // Delegate to event handler - maintain SRP
    if (event_handler_) {
        event_handler_->setConnectionCallback(std::move(callback));
    }
}

// Event handling methods extracted to ServerEventHandler class

void WebSocketServer::removeSession(std::shared_ptr<WebSocketSession> session) {
    if (session_manager_) {
        session_manager_->removeSession(session);
    }
}


} // namespace siren::websocket