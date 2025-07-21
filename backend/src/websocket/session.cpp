/**
 * @file session.cpp
 * @brief Implementation of WebSocket session - MISRA C++ compliant
 * @author KostasAndroulidakis
 * @date 2025
 *
 * Single Responsibility: WebSocket protocol communication for one client
 */

#include "websocket/session.hpp"
#include "websocket/server.hpp"
#include "websocket/message_queue_manager.hpp"
#include "utils/json_serializer.hpp"
#include "utils/error_handler.hpp"
#include "constants/message.hpp"
#include "constants/communication.hpp"
#include <iostream>
#include <chrono>

namespace siren::websocket {

namespace cnst = siren::constants;

// SSOT for session constants (MISRA C++ Rule 5.0.1)
namespace {
    constexpr const char* COMPONENT_NAME = "WebSocketSession";
    constexpr auto WEBSOCKET_TIMEOUT = std::chrono::seconds(30);  // WebSocket timeout
}

WebSocketSession::WebSocketSession(tcp::socket&& socket,
                                 std::weak_ptr<WebSocketServer> server_weak_ptr)
    : ws_(std::move(socket))
    , server_weak_ptr_(server_weak_ptr)
    , client_endpoint_()
    , is_alive_(false)
    , closing_(false)
    , queue_manager_(nullptr)
    , write_in_progress_(false)
    , buffer_()
{
    try {
        // Get client endpoint information for logging
        auto endpoint = ws_.next_layer().socket().remote_endpoint();
        client_endpoint_ = endpoint.address().to_string() + ":" + std::to_string(endpoint.port());

        // Initialize queue manager with callback for client disconnection
        queue_manager_ = std::make_unique<MessageQueueManager>(
            client_endpoint_,
            [this]() {
                // Queue full callback - disconnect client
                is_alive_.store(false);
                close();
            });

        std::cout << "[" << COMPONENT_NAME << "] Session created for " << client_endpoint_ << std::endl;

    } catch (const std::exception& e) {
        utils::ErrorHandler::handleException(COMPONENT_NAME, "constructor", e,
                                           data::ErrorSeverity::WARNING);
        client_endpoint_ = "unknown";
    }
}

void WebSocketSession::start() {
    try {
        // Set WebSocket options
        ws_.set_option(websocket::stream_base::timeout::suggested(
            beast::role_type::server));

        // Set decorator for HTTP response
        ws_.set_option(websocket::stream_base::decorator(
            [](websocket::response_type& res) {
                res.set(beast::http::field::server,
                       std::string("SIREN-Military-Server"));
            }));

        // Start WebSocket handshake
        ws_.async_accept(
            [self = shared_from_this()](beast::error_code ec) {
                self->onAccept(ec);
            });

        std::cout << "[" << COMPONENT_NAME << "] Starting WebSocket handshake for "
                  << client_endpoint_ << std::endl;

    } catch (const std::exception& e) {
        handleError("Failed to start session", beast::error_code{});
    }
}

void WebSocketSession::sendSonarData(const data::SonarDataPoint& data) {
    if (!isAlive()) {
        return;
    }

    try {
        // Serialize sonar data to JSON (SSOT for sonar serialization)
        std::string message = utils::JsonSerializer::serialize(data);
        enqueueMessage(message);

    } catch (const std::exception& e) {
        utils::ErrorHandler::handleException(COMPONENT_NAME,
                                           "sonar data serialization for " + client_endpoint_,
                                           e, data::ErrorSeverity::ERROR);
    }
}

void WebSocketSession::sendPerformanceMetrics(const data::PerformanceMetrics& metrics) {
    if (!isAlive()) {
        return;
    }

    try {
        // Serialize performance metrics to JSON (SSOT for metrics serialization)
        std::string message = utils::JsonSerializer::serialize(metrics);
        enqueueMessage(message);

    } catch (const std::exception& e) {
        utils::ErrorHandler::handleException(COMPONENT_NAME,
                                           "performance metrics serialization for " + client_endpoint_,
                                           e, data::ErrorSeverity::ERROR);
    }
}

void WebSocketSession::sendMessage(const std::string& message) {
    if (!isAlive() || message.empty()) {
        return;
    }

    enqueueMessage(message);
}

void WebSocketSession::close() {
    if (closing_.exchange(true)) {
        return; // Already closing
    }

    std::cout << "[" << COMPONENT_NAME << "] Closing session for " << client_endpoint_ << std::endl;

    try {
        if (ws_.is_open()) {
            ws_.async_close(websocket::close_code::normal,
                [self = shared_from_this()](beast::error_code ec) {
                    if (ec) {
                        // Log close error but don't throw
                        std::cout << "[WebSocketSession] Close error for "
                                  << self->client_endpoint_ << ": " << ec.message() << std::endl;
                    }
                    self->is_alive_.store(false);
                });
        } else {
            is_alive_.store(false);
        }

        // Notify server to remove this session
        if (auto server = server_weak_ptr_.lock()) {
            server->removeSession(shared_from_this());
        }

    } catch (const std::exception& e) {
        utils::ErrorHandler::handleException(COMPONENT_NAME,
                                           "session closure for " + client_endpoint_,
                                           e, data::ErrorSeverity::WARNING);
        is_alive_.store(false);
    }
}

bool WebSocketSession::isAlive() const noexcept {
    return is_alive_.load() && !closing_.load();
}

std::string WebSocketSession::getClientEndpoint() const {
    return client_endpoint_;
}

void WebSocketSession::onAccept(beast::error_code ec) {
    if (ec) {
        handleError("WebSocket handshake failed", ec);
        return;
    }

    is_alive_.store(true);
    std::cout << "[" << COMPONENT_NAME << "] WebSocket handshake completed for "
              << client_endpoint_ << std::endl;

    // Start reading for incoming messages
    ws_.async_read(buffer_,
        [self = shared_from_this()](beast::error_code ec, std::size_t bytes_transferred) {
            self->onRead(ec, bytes_transferred);
        });
}

void WebSocketSession::onRead(beast::error_code ec, std::size_t bytes_transferred) {
    if (ec) {
        handleError("WebSocket read failed", ec);
        return;
    }

    // For now, we just acknowledge the message and continue reading
    // In a full implementation, we would process control messages here
    std::cout << "[" << COMPONENT_NAME << "] Received " << bytes_transferred
              << " bytes from " << client_endpoint_ << std::endl;

    // Clear buffer for next read
    buffer_.clear();

    // Continue reading if still alive
    if (isAlive()) {
        ws_.async_read(buffer_,
            [self = shared_from_this()](beast::error_code ec, std::size_t bytes_transferred) {
                self->onRead(ec, bytes_transferred);
            });
    }
}

void WebSocketSession::onWrite(beast::error_code ec, std::size_t bytes_transferred) {
    write_in_progress_.store(false);

    if (ec) {
        handleError("WebSocket write failed", ec);
        return;
    }

    std::cout << "[" << COMPONENT_NAME << "] Sent " << bytes_transferred
              << " bytes to " << client_endpoint_ << std::endl;

    // Process next message in queue
    processNextMessage();
}

void WebSocketSession::processNextMessage() {
    if (!isAlive() || write_in_progress_.load() || !queue_manager_) {
        return;
    }

    std::string message;

    // Get next message from queue manager - SRP compliance
    if (!queue_manager_->getNextMessage(message)) {
        return; // No messages to send
    }

    // Send the message
    if (!message.empty()) {
        write_in_progress_.store(true);

        ws_.async_write(boost::asio::buffer(message),
            [self = shared_from_this()](beast::error_code ec, std::size_t bytes_transferred) {
                self->onWrite(ec, bytes_transferred);
            });
    }
}

void WebSocketSession::enqueueMessage(const std::string& message) {
    if (!isAlive() || !queue_manager_) {
        return;
    }

    // Delegate to queue manager - SRP compliance
    bool message_queued = queue_manager_->enqueueMessage(message, write_in_progress_);
    
    // Start writing if message was queued and no write in progress
    if (message_queued && !write_in_progress_.load()) {
        processNextMessage();
    }
}

void WebSocketSession::handleError(const std::string& error_message, beast::error_code ec) {
    // Determine error severity
    data::ErrorSeverity severity = data::ErrorSeverity::ERROR;

    if (ec == websocket::error::closed) {
        severity = data::ErrorSeverity::INFO; // Normal closure
    } else if (ec == boost::asio::error::operation_aborted) {
        severity = data::ErrorSeverity::INFO; // Normal shutdown
    }

    if (ec) {
        utils::ErrorHandler::handleBoostError(COMPONENT_NAME,
                                              error_message + " for " + client_endpoint_,
                                              ec, severity);
    } else {
        utils::ErrorHandler::handleSystemError(COMPONENT_NAME,
                                              error_message + " for " + client_endpoint_,
                                              severity);
    }

    // Mark session as not alive and close
    is_alive_.store(false);

    if (!closing_.load()) {
        close();
    }
}

} // namespace siren::websocket