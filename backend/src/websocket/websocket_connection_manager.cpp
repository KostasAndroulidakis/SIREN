/**
 * @file websocket_connection_manager.cpp
 * @brief Implementation of military-grade WebSocket connection manager
 * @author unoRadar Project
 * @date 2025
 *
 * Specialized manager for TCP connection acceptance and initial handshake
 * coordination. Follows SRP by focusing exclusively on connection lifecycle.
 */

#include "websocket/websocket_connection_manager.hpp"
#include <iostream>

namespace unoradar::websocket {

WebSocketConnectionManager::WebSocketConnectionManager(boost::asio::io_context& io_context, uint16_t port)
    : io_context_(io_context)
    , port_(port)
    , active_(false)
    , shutdown_requested_(false)
{
    std::cout << "[WebSocketConnectionManager] Initializing connection manager on port " << port << std::endl;
}

WebSocketConnectionManager::~WebSocketConnectionManager() {
    if (active_.load()) {
        stop();
    }
}

bool WebSocketConnectionManager::initialize() {
    try {
        // Create TCP acceptor
        acceptor_ = std::make_unique<tcp::acceptor>(io_context_);

        // Configure acceptor
        tcp::endpoint endpoint(tcp::v4(), port_);
        acceptor_->open(endpoint.protocol());
        acceptor_->set_option(boost::asio::socket_base::reuse_address(true));
        acceptor_->bind(endpoint);

        std::cout << "[WebSocketConnectionManager] ✅ Connection manager initialized on port " << port_ << std::endl;
        return true;

    } catch (const std::exception& e) {
        handleError("Connection manager initialization failed: " + std::string(e.what()), {});
        return false;
    }
}

bool WebSocketConnectionManager::start() {
    if (active_.load()) {
        std::cout << "[WebSocketConnectionManager] ⚠️ Connection manager already active" << std::endl;
        return true;
    }

    try {
        // Start listening for connections
        acceptor_->listen(boost::asio::socket_base::max_listen_connections);

        active_.store(true);
        shutdown_requested_.store(false);

        std::cout << "[WebSocketConnectionManager] 🚀 Connection manager started - listening on port " << port_ << std::endl;

        // Begin accepting connections
        startAccept();

        return true;

    } catch (const std::exception& e) {
        handleError("Connection manager start failed: " + std::string(e.what()), {});
        return false;
    }
}

void WebSocketConnectionManager::stop() {
    if (!active_.load()) {
        return;
    }

    std::cout << "[WebSocketConnectionManager] 🛑 Stopping connection manager..." << std::endl;

    shutdown_requested_.store(true);
    active_.store(false);

    // Close acceptor
    if (acceptor_) {
        try {
            acceptor_->close();
        } catch (const std::exception& e) {
            std::cout << "[WebSocketConnectionManager] ⚠️ Error closing acceptor: " << e.what() << std::endl;
        }
    }

    std::cout << "[WebSocketConnectionManager] ✅ Connection manager stopped" << std::endl;
}

bool WebSocketConnectionManager::isActive() const noexcept {
    return active_.load();
}

uint16_t WebSocketConnectionManager::getPort() const noexcept {
    return port_;
}

void WebSocketConnectionManager::setAcceptCallback(AcceptCallback callback) {
    accept_callback_ = std::move(callback);
}

void WebSocketConnectionManager::setErrorCallback(ErrorCallback callback) {
    error_callback_ = std::move(callback);
}

void WebSocketConnectionManager::startAccept() {
    if (shutdown_requested_.load()) {
        return;
    }

    acceptor_->async_accept(
        [this](beast::error_code ec, tcp::socket socket) {
            onAccept(ec, std::move(socket));
        });
}

void WebSocketConnectionManager::onAccept(beast::error_code ec, tcp::socket socket) {
    if (ec) {
        handleError("Accept error", ec);
    } else {
        // Notify callback with accepted socket
        if (accept_callback_) {
            accept_callback_(std::move(socket));
        }
    }

    // Continue accepting new connections
    startAccept();
}

void WebSocketConnectionManager::handleError(const std::string& error_message, beast::error_code ec) {
    std::cout << "[WebSocketConnectionManager] ❌ " << error_message;
    if (ec) {
        std::cout << ": " << ec.message();
    }
    std::cout << std::endl;

    // Notify error callback
    if (error_callback_) {
        error_callback_(error_message, ec);
    }
}

} // namespace unoradar::websocket