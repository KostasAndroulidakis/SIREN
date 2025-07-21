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

// WebSocketSession Implementation

WebSocketSession::WebSocketSession(tcp::socket&& socket,
                                 std::weak_ptr<WebSocketServer> server_weak_ptr)
    : ws_(std::move(socket))
    , server_weak_ptr_(server_weak_ptr)
    , is_alive_(true)
    , closing_(false)
    , write_in_progress_(false)
{
    // Get client endpoint for logging
    try {
        auto endpoint = ws_.next_layer().socket().remote_endpoint();
        client_endpoint_ = endpoint.address().to_string() + ":" + std::to_string(endpoint.port());
    } catch (...) {
        client_endpoint_ = cnst::message::websocket_status::UNKNOWN_CLIENT;
    }
}

void WebSocketSession::start() {
    std::cout << cnst::message::websocket_status::SESSION_PREFIX << " "
              << cnst::message::websocket_status::SESSION_STARTING << ": " << client_endpoint_ << std::endl;

    // Set WebSocket options
    ws_.set_option(websocket::stream_base::timeout::suggested(
        beast::role_type::server));

    // Set a decorator to change the Server of the handshake
    ws_.set_option(websocket::stream_base::decorator(
        [](websocket::response_type& res) {
            res.set(http::field::server, cnst::message::version::SERVER_NAME);
        }));

    // Accept the websocket handshake
    ws_.async_accept(
        beast::bind_front_handler(&WebSocketSession::onAccept, shared_from_this()));
}

void WebSocketSession::onAccept(beast::error_code ec) {
    if (ec) {
        utils::ErrorHandler::handleBoostError(cnst::message::websocket_status::SESSION_PREFIX,
                                               cnst::message::websocket_status::HANDSHAKE_FAILED_ERROR, ec,
                                               data::ErrorSeverity::ERROR);
        return;
    }

    std::cout << cnst::message::websocket_status::SESSION_PREFIX << " "
              << cnst::message::websocket_status::HANDSHAKE_COMPLETE << ": " << client_endpoint_ << std::endl;

    // Start reading messages
    ws_.async_read(
        buffer_,
        beast::bind_front_handler(&WebSocketSession::onRead, shared_from_this()));
}

void WebSocketSession::onRead(beast::error_code ec, std::size_t bytes_transferred) {
    if (ec) {
        if (ec == websocket::error::closed) {
            std::cout << cnst::message::websocket_status::SESSION_PREFIX << " "
                      << cnst::message::websocket_status::CLIENT_DISCONNECTED << ": " << client_endpoint_ << std::endl;
        } else {
            utils::ErrorHandler::handleBoostError(cnst::message::websocket_status::SESSION_PREFIX,
                                                   "Read error", ec,
                                                   data::ErrorSeverity::ERROR);
        }
        return;
    }

    // Process incoming message (for now, just log it)
    std::string message = beast::buffers_to_string(buffer_.data());
    std::cout << cnst::message::websocket_status::SESSION_PREFIX << " "
              << cnst::message::websocket_status::RECEIVED_MESSAGE << " " << client_endpoint_
              << ": " << message << std::endl;

    // Clear buffer for next message
    buffer_.consume(bytes_transferred);

    // Continue reading
    ws_.async_read(
        buffer_,
        beast::bind_front_handler(&WebSocketSession::onRead, shared_from_this()));
}

void WebSocketSession::sendSonarData(const data::SonarDataPoint& data) {
    if (!is_alive_.load() || closing_.load()) {
        return;
    }

    enqueueMessage(utils::JsonSerializer::serialize(data));
}

void WebSocketSession::sendPerformanceMetrics(const data::PerformanceMetrics& metrics) {
    if (!is_alive_.load() || closing_.load()) {
        return;
    }

    enqueueMessage(utils::JsonSerializer::serialize(metrics));
}

void WebSocketSession::sendMessage(const std::string& message) {
    if (!is_alive_.load() || closing_.load()) {
        return;
    }

    enqueueMessage(message);
}

void WebSocketSession::processNextMessage() {
    // Only process if no write is in progress
    if (write_in_progress_.exchange(true)) {
        return; // Write already in progress
    }

    std::string message;
    {
        std::lock_guard<std::mutex> lock(queue_mutex_);
        if (message_queue_.empty()) {
            write_in_progress_.store(false);
            return;
        }
        message = message_queue_.front();
        message_queue_.pop();
    }

    // Send the message
    ws_.async_write(
        boost::asio::buffer(message),
        beast::bind_front_handler(&WebSocketSession::onWrite, shared_from_this()));
}

void WebSocketSession::onWrite(beast::error_code ec, std::size_t bytes_transferred) {
    write_in_progress_.store(false);

    if (ec) {
        utils::ErrorHandler::handleBoostError(cnst::message::websocket_status::SESSION_PREFIX,
                                               cnst::message::websocket_status::WRITE_ERROR, ec,
                                               data::ErrorSeverity::ERROR);
        return;
    }

    // Process next message if available
    processNextMessage();
}

void WebSocketSession::enqueueMessage(const std::string& message) {
    {
        std::lock_guard<std::mutex> lock(queue_mutex_);
        message_queue_.push(message);
    }
    processNextMessage();
}

void WebSocketSession::close() {
    if (closing_.exchange(true)) {
        return; // Already closing
    }

    std::cout << cnst::message::websocket_status::SESSION_PREFIX << " "
              << cnst::message::websocket_status::CLOSING_CONNECTION << ": " << client_endpoint_ << std::endl;

    // Close the WebSocket
    ws_.async_close(websocket::close_code::normal,
        [self = shared_from_this()](beast::error_code ec) {
            if (ec) {
                std::cout << cnst::message::websocket_status::SESSION_PREFIX << " "
                          << cnst::message::websocket_status::ERROR_CLOSING << ": " << ec.message() << std::endl;
            }
            self->is_alive_.store(false);
        });
}

bool WebSocketSession::isAlive() const noexcept {
    return is_alive_.load() && !closing_.load();
}

std::string WebSocketSession::getClientEndpoint() const {
    return client_endpoint_;
}

void WebSocketSession::handleError(const std::string& error_message, beast::error_code ec) {
    utils::ErrorHandler::handleConnectionError(cnst::message::websocket_status::SESSION_PREFIX,
                                                client_endpoint_,
                                                error_message + ": " + ec.message(),
                                                data::ErrorSeverity::ERROR);

    is_alive_.store(false);

    // Notify server to remove this session
    if (auto server = server_weak_ptr_.lock()) {
        server->removeSession(shared_from_this());
    }
}


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

    // Create specialized managers
    connection_manager_ = std::make_unique<WebSocketConnectionManager>(io_context_, port_);
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
        // Initialize connection manager
        if (!connection_manager_->initialize()) {
            utils::ErrorHandler::handleInitializationError(cnst::message::websocket_status::SERVER_PREFIX,
                                                            "connection_manager",
                                                            cnst::message::websocket_status::INIT_FAILED_CONNECTION_MANAGER);
            return false;
        }

        // Initialize message broadcaster
        if (!message_broadcaster_->initialize()) {
            utils::ErrorHandler::handleInitializationError(cnst::message::websocket_status::SERVER_PREFIX,
                                                            "message_broadcaster",
                                                            cnst::message::websocket_status::INIT_FAILED_BROADCASTER);
            return false;
        }

        // Set up manager callbacks
        connection_manager_->setAcceptCallback(
            [this](tcp::socket socket) {
                onConnectionAccepted(std::move(socket));
            });

        connection_manager_->setErrorCallback(
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
        // Start connection manager
        if (!connection_manager_->start()) {
            utils::ErrorHandler::handleSystemError(cnst::message::websocket_status::SERVER_PREFIX,
                                                    cnst::message::websocket_status::START_FAILED_CONNECTION_MANAGER,
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