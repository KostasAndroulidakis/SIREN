/**
 * @file server.cpp
 * @brief Implementation of military-grade WebSocket server
 * @author SIREN Project
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
    , server_start_time_(std::chrono::steady_clock::now())
{
    std::cout << cnst::message::websocket_status::SERVER_PREFIX << " "
              << cnst::message::websocket_status::INITIALIZING_SERVER << " " << port << std::endl;

    // Initialize statistics
    statistics_ = data::WebSocketStatistics{};

    // Create specialized managers - SRP compliant components
    connection_acceptor_ = std::make_unique<ConnectionAcceptor>(io_context_, port_);
    session_manager_ = std::make_unique<WebSocketSessionManager>();
    message_broadcaster_ = std::make_unique<WebSocketMessageBroadcaster>();
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
                                                            cnst::message::websocket_status::INIT_FAILED_BROADCASTER);
            return false;
        }

        // Set up component callbacks
        connection_acceptor_->setAcceptCallback(
            [this](tcp::socket socket) {
                onConnectionAccepted(std::move(socket));
            });

        connection_acceptor_->setErrorCallback(
            [this](const std::string& error_message, beast::error_code ec) {
                onConnectionError(error_message, ec);
            });

        session_manager_->setSessionCallback(
            [this](const std::string& endpoint, bool connected) {
                onSessionEvent(endpoint, connected);
            });

        message_broadcaster_->setBroadcastCallback(
            [this](size_t sessions_reached) {
                onBroadcastCompleted(sessions_reached);
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
                                                    cnst::message::websocket_status::START_FAILED_BROADCASTER,
                                                    data::ErrorSeverity::ERROR);
            connection_manager_->stop();
            return false;
        }

        running_.store(true);
        server_start_time_ = std::chrono::steady_clock::now();

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
    if (connection_manager_) {
        connection_manager_->stop();
    }

    if (message_broadcaster_) {
        message_broadcaster_->stop();
    }

    if (session_manager_) {
        session_manager_->closeAllSessions();
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

    // Update statistics (centralized statistics update)
    updateStatistics();
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
    std::lock_guard<std::mutex> lock(stats_mutex_);

    auto stats = statistics_;
    stats.active_connections = getActiveConnections();

    // Calculate uptime
    auto now = std::chrono::steady_clock::now();
    auto uptime = std::chrono::duration_cast<std::chrono::seconds>(
        now - server_start_time_).count();
    stats.uptime_seconds = static_cast<uint64_t>(uptime);

    return stats;
}

void WebSocketServer::setConnectionCallback(ConnectionCallback callback) {
    connection_callback_ = std::move(callback);
}

void WebSocketServer::onConnectionAccepted(tcp::socket socket) {
    // Create new session through session manager
    auto session = session_manager_->createSession(std::move(socket), weak_from_this());

    // Start the session
    session->start();

    std::cout << cnst::message::websocket_status::SERVER_PREFIX << " "
              << cnst::message::websocket_status::NEW_CLIENT_CONNECTED << ": " << session->getClientEndpoint()
              << " " << cnst::message::websocket_status::TOTAL_CLIENTS << " " << getActiveConnections() << ")" << std::endl;

    // Update statistics
    {
        std::lock_guard<std::mutex> stats_lock(stats_mutex_);
        statistics_.connections_accepted++;
    }
}

void WebSocketServer::onConnectionError(const std::string& error_message, beast::error_code ec) {
    if (ec) {
        utils::ErrorHandler::handleBoostError(cnst::message::websocket_status::SERVER_PREFIX,
                                               error_message, ec,
                                               data::ErrorSeverity::ERROR);
    } else {
        utils::ErrorHandler::handleSystemError(cnst::message::websocket_status::SERVER_PREFIX,
                                                error_message,
                                                data::ErrorSeverity::ERROR);
    }

    // Update error statistics (centralized statistics update)
    updateStatistics();
}

void WebSocketServer::onSessionEvent(const std::string& endpoint, bool connected) {
    // Notify callback
    if (connection_callback_) {
        connection_callback_(endpoint, connected);
    }
}

void WebSocketServer::onBroadcastCompleted(size_t sessions_reached) {
    // Update broadcast statistics if needed
    // This can be extended for additional metrics tracking
}

void WebSocketServer::removeSession(std::shared_ptr<WebSocketSession> session) {
    if (session_manager_) {
        session_manager_->removeSession(session);
    }
}

void WebSocketServer::updateStatistics() {
    // Periodic cleanup of closed sessions
    if (session_manager_) {
        session_manager_->cleanupClosedSessions();
    }
}

} // namespace siren::websocket