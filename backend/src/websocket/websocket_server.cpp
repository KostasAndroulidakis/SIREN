/**
 * @file websocket_server.cpp
 * @brief Implementation of military-grade WebSocket server
 * @author unoRadar Project
 * @date 2025
 *
 * High-performance WebSocket server for real-time radar data streaming
 * with military-grade reliability and error handling.
 */

#include "websocket/websocket_server.hpp"
#include "utils/json_serializer.hpp"
#include <iostream>
#include <chrono>

namespace unoradar::websocket {

namespace http = beast::http;  // Local alias for HTTP functionality

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
        client_endpoint_ = "unknown";
    }
}

void WebSocketSession::start() {
    std::cout << "[WebSocketSession] 🚀 Starting session for client: " << client_endpoint_ << std::endl;

    // Set WebSocket options
    ws_.set_option(websocket::stream_base::timeout::suggested(
        beast::role_type::server));

    // Set a decorator to change the Server of the handshake
    ws_.set_option(websocket::stream_base::decorator(
        [](websocket::response_type& res) {
            res.set(http::field::server, "unoRadar Military-Grade Server");
        }));

    // Accept the websocket handshake
    ws_.async_accept(
        beast::bind_front_handler(&WebSocketSession::onAccept, shared_from_this()));
}

void WebSocketSession::onAccept(beast::error_code ec) {
    if (ec) {
        handleError("WebSocket handshake failed", ec);
        return;
    }

    std::cout << "[WebSocketSession] ✅ WebSocket handshake complete for: " << client_endpoint_ << std::endl;

    // Start reading messages
    ws_.async_read(
        buffer_,
        beast::bind_front_handler(&WebSocketSession::onRead, shared_from_this()));
}

void WebSocketSession::onRead(beast::error_code ec, std::size_t bytes_transferred) {
    if (ec) {
        if (ec == websocket::error::closed) {
            std::cout << "[WebSocketSession] 🔌 Client disconnected: " << client_endpoint_ << std::endl;
        } else {
            handleError("Read error", ec);
        }
        return;
    }

    // Process incoming message (for now, just log it)
    std::string message = beast::buffers_to_string(buffer_.data());
    std::cout << "[WebSocketSession] 📨 Received from " << client_endpoint_ << ": " << message << std::endl;

    // Clear buffer for next message
    buffer_.consume(bytes_transferred);

    // Continue reading
    ws_.async_read(
        buffer_,
        beast::bind_front_handler(&WebSocketSession::onRead, shared_from_this()));
}

void WebSocketSession::sendRadarData(const data::RadarDataPoint& data) {
    if (!is_alive_.load() || closing_.load()) {
        return;
    }

    std::string json_data = serializeRadarData(data);

    {
        std::lock_guard<std::mutex> lock(queue_mutex_);
        message_queue_.push(json_data);
    }

    processNextMessage();
}

void WebSocketSession::sendPerformanceMetrics(const data::PerformanceMetrics& metrics) {
    if (!is_alive_.load() || closing_.load()) {
        return;
    }

    std::string json_data = serializePerformanceMetrics(metrics);

    {
        std::lock_guard<std::mutex> lock(queue_mutex_);
        message_queue_.push(json_data);
    }

    processNextMessage();
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
        handleError("Write error", ec);
        return;
    }

    // Process next message if available
    processNextMessage();
}

void WebSocketSession::close() {
    if (closing_.exchange(true)) {
        return; // Already closing
    }

    std::cout << "[WebSocketSession] 🔌 Closing connection to: " << client_endpoint_ << std::endl;

    // Close the WebSocket
    ws_.async_close(websocket::close_code::normal,
        [self = shared_from_this()](beast::error_code ec) {
            if (ec) {
                std::cout << "[WebSocketSession] ⚠️ Error closing connection: " << ec.message() << std::endl;
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
    std::cout << "[WebSocketSession] ❌ " << error_message << " (" << client_endpoint_
              << "): " << ec.message() << std::endl;

    is_alive_.store(false);

    // Notify server to remove this session
    if (auto server = server_weak_ptr_.lock()) {
        server->removeSession(shared_from_this());
    }
}

std::string WebSocketSession::serializeRadarData(const data::RadarDataPoint& data) {
    return utils::JsonSerializer::serialize(data);
}

std::string WebSocketSession::serializePerformanceMetrics(const data::PerformanceMetrics& metrics) {
    return utils::JsonSerializer::serialize(metrics);
}

// WebSocketServer Implementation

WebSocketServer::WebSocketServer(boost::asio::io_context& io_context, uint16_t port)
    : io_context_(io_context)
    , port_(port)
    , running_(false)
    , shutdown_requested_(false)
    , server_start_time_(std::chrono::steady_clock::now())
{
    std::cout << "[WebSocketServer] Initializing military-grade WebSocket server on port " << port << std::endl;

    // Initialize statistics
    statistics_ = data::WebSocketStatistics{};
}

WebSocketServer::~WebSocketServer() {
    if (running_.load()) {
        stop();
    }
}

bool WebSocketServer::initialize() {
    try {
        // Create acceptor
        acceptor_ = std::make_unique<tcp::acceptor>(io_context_);

        // Open acceptor
        tcp::endpoint endpoint(tcp::v4(), port_);
        acceptor_->open(endpoint.protocol());

        // Set socket options
        acceptor_->set_option(boost::asio::socket_base::reuse_address(true));

        // Bind to endpoint
        acceptor_->bind(endpoint);

        std::cout << "[WebSocketServer] ✅ WebSocket server initialized on port " << port_ << std::endl;
        return true;

    } catch (const std::exception& e) {
        handleError("WebSocket server initialization failed: " + std::string(e.what()), {});
        return false;
    }
}

bool WebSocketServer::start() {
    if (running_.load()) {
        std::cout << "[WebSocketServer] ⚠️ Server already running" << std::endl;
        return true;
    }

    try {
        // Start listening
        acceptor_->listen(boost::asio::socket_base::max_listen_connections);

        running_.store(true);
        server_start_time_ = std::chrono::steady_clock::now();

        std::cout << "[WebSocketServer] 🚀 WebSocket server started - listening on port " << port_ << std::endl;

        // Start accepting connections
        startAccept();

        return true;

    } catch (const std::exception& e) {
        handleError("WebSocket server start failed: " + std::string(e.what()), {});
        return false;
    }
}

void WebSocketServer::stop() {
    if (!running_.load()) {
        return;
    }

    std::cout << "[WebSocketServer] 🛑 Stopping WebSocket server..." << std::endl;

    shutdown_requested_.store(true);
    running_.store(false);

    // Close acceptor
    if (acceptor_) {
        try {
            acceptor_->close();
        } catch (const std::exception& e) {
            std::cout << "[WebSocketServer] ⚠️ Error closing acceptor: " << e.what() << std::endl;
        }
    }

    // Close all active sessions
    {
        std::lock_guard<std::mutex> lock(sessions_mutex_);
        for (auto& session : active_sessions_) {
            session->close();
        }
        active_sessions_.clear();
    }

    std::cout << "[WebSocketServer] ✅ WebSocket server stopped" << std::endl;
}

bool WebSocketServer::isRunning() const noexcept {
    return running_.load();
}

void WebSocketServer::broadcastRadarData(const data::RadarDataPoint& data) {
    std::lock_guard<std::mutex> lock(sessions_mutex_);

    for (auto& session : active_sessions_) {
        if (session->isAlive()) {
            session->sendRadarData(data);
        }
    }

    // Update statistics
    {
        std::lock_guard<std::mutex> stats_lock(stats_mutex_);
        statistics_.messages_sent++;
    }
}

void WebSocketServer::broadcastPerformanceMetrics(const data::PerformanceMetrics& metrics) {
    std::lock_guard<std::mutex> lock(sessions_mutex_);

    for (auto& session : active_sessions_) {
        if (session->isAlive()) {
            session->sendPerformanceMetrics(metrics);
        }
    }
}

size_t WebSocketServer::getActiveConnections() const noexcept {
    std::lock_guard<std::mutex> lock(sessions_mutex_);
    return active_sessions_.size();
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

void WebSocketServer::startAccept() {
    if (shutdown_requested_.load()) {
        return;
    }

    acceptor_->async_accept(
        [self = shared_from_this()](beast::error_code ec, tcp::socket socket) {
            self->onAccept(ec, std::move(socket));
        });
}

void WebSocketServer::onAccept(beast::error_code ec, tcp::socket socket) {
    if (ec) {
        handleError("Accept error", ec);
    } else {
        // Create new session
        auto session = std::make_shared<WebSocketSession>(std::move(socket), weak_from_this());

        // Add to active sessions
        addSession(session);

        // Start the session
        session->start();

        std::cout << "[WebSocketServer] 🔌 New client connected: " << session->getClientEndpoint()
                  << " (total: " << getActiveConnections() << ")" << std::endl;
    }

    // Continue accepting new connections
    startAccept();
}

void WebSocketServer::addSession(std::shared_ptr<WebSocketSession> session) {
    {
        std::lock_guard<std::mutex> lock(sessions_mutex_);
        active_sessions_.insert(session);
    }

    // Update statistics
    {
        std::lock_guard<std::mutex> stats_lock(stats_mutex_);
        statistics_.connections_accepted++;
    }

    // Notify callback
    if (connection_callback_) {
        connection_callback_(session->getClientEndpoint(), true);
    }
}

void WebSocketServer::removeSession(std::shared_ptr<WebSocketSession> session) {
    {
        std::lock_guard<std::mutex> lock(sessions_mutex_);
        active_sessions_.erase(session);
    }

    std::cout << "[WebSocketServer] 🔌 Client disconnected: " << session->getClientEndpoint()
              << " (remaining: " << getActiveConnections() << ")" << std::endl;

    // Notify callback
    if (connection_callback_) {
        connection_callback_(session->getClientEndpoint(), false);
    }
}

void WebSocketServer::cleanupClosedSessions() {
    std::lock_guard<std::mutex> lock(sessions_mutex_);

    auto it = active_sessions_.begin();
    while (it != active_sessions_.end()) {
        if (!(*it)->isAlive()) {
            it = active_sessions_.erase(it);
        } else {
            ++it;
        }
    }
}

void WebSocketServer::handleError(const std::string& error_message, beast::error_code ec) {
    std::cout << "[WebSocketServer] ❌ " << error_message;
    if (ec) {
        std::cout << ": " << ec.message();
    }
    std::cout << std::endl;

    // Update error statistics
    {
        std::lock_guard<std::mutex> lock(stats_mutex_);
        statistics_.connection_errors++;
    }
}

} // namespace unoradar::websocket