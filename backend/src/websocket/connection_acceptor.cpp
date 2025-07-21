/**
 * @file connection_acceptor.cpp
 * @brief Implementation of TCP connection acceptor - MISRA C++ compliant
 * @author KostasAndroulidakis
 * @date 2025
 *
 * Single Responsibility: Accept incoming TCP connections ONLY
 */

#include "websocket/connection_acceptor.hpp"
#include "constants/communication.hpp"
#include "constants/message.hpp"
#include "utils/error_handler.hpp"
#include <iostream>

namespace siren::websocket {

namespace cnst = siren::constants;

// SSOT for connection acceptor constants (MISRA C++ Rule 5.0.1)
namespace {
    constexpr int ACCEPTOR_BACKLOG = 128;  // TCP listen backlog
    constexpr const char* COMPONENT_NAME = "ConnectionAcceptor";
}

ConnectionAcceptor::ConnectionAcceptor(boost::asio::io_context& io_context, uint16_t port)
    : io_context_(io_context)
    , port_(port)
    , acceptor_(nullptr)
    , running_(false)
    , shutdown_requested_(false)
    , accept_callback_(nullptr)
    , error_callback_(nullptr)
{
    std::cout << "[" << COMPONENT_NAME << "] Initializing TCP acceptor on port " << port_ << std::endl;
}

ConnectionAcceptor::~ConnectionAcceptor() {
    if (running_.load()) {
        stop();
    }
}

bool ConnectionAcceptor::initialize() {
    try {
        // Create TCP acceptor (RAII managed)
        acceptor_ = std::make_unique<tcp::acceptor>(io_context_);

        // Configure acceptor endpoint
        tcp::endpoint endpoint(tcp::v4(), port_);

        // Open acceptor
        acceptor_->open(endpoint.protocol());

        // Set socket options for better performance
        acceptor_->set_option(tcp::acceptor::reuse_address(true));

        // Bind to endpoint
        acceptor_->bind(endpoint);

        // Start listening
        acceptor_->listen(ACCEPTOR_BACKLOG);

        std::cout << "[" << COMPONENT_NAME << "] Initialized on "
                  << endpoint.address().to_string() << ":" << endpoint.port() << std::endl;
        return true;

    } catch (const std::exception& e) {
        utils::ErrorHandler::handleException(COMPONENT_NAME, "initialization", e,
                                           data::ErrorSeverity::FATAL);
        return false;
    }
}

bool ConnectionAcceptor::start() {
    if (running_.load()) {
        utils::ErrorHandler::handleSystemError(COMPONENT_NAME,
                                             "Already running",
                                             data::ErrorSeverity::WARNING);
        return true;
    }

    if (!acceptor_) {
        utils::ErrorHandler::handleSystemError(COMPONENT_NAME,
                                             "Not initialized - cannot start",
                                             data::ErrorSeverity::ERROR);
        return false;
    }

    try {
        running_.store(true);
        shutdown_requested_.store(false);

        // Start accepting connections
        startAccept();

        std::cout << "[" << COMPONENT_NAME << "] Started accepting connections on port "
                  << port_ << std::endl;
        return true;

    } catch (const std::exception& e) {
        utils::ErrorHandler::handleException(COMPONENT_NAME, "start operation", e,
                                           data::ErrorSeverity::ERROR);
        running_.store(false);
        return false;
    }
}

void ConnectionAcceptor::stop() {
    if (!running_.load()) {
        return;
    }

    std::cout << "[" << COMPONENT_NAME << "] Stopping connection acceptor..." << std::endl;

    shutdown_requested_.store(true);
    running_.store(false);

    // Close acceptor to stop accepting new connections
    if (acceptor_) {
        try {
            acceptor_->close();
        } catch (const std::exception& e) {
            utils::ErrorHandler::handleException(COMPONENT_NAME, "shutdown", e,
                                               data::ErrorSeverity::WARNING);
        }
    }

    std::cout << "[" << COMPONENT_NAME << "] Stopped" << std::endl;
}

bool ConnectionAcceptor::isRunning() const noexcept {
    return running_.load();
}

void ConnectionAcceptor::setAcceptCallback(AcceptCallback callback) {
    accept_callback_ = std::move(callback);
}

void ConnectionAcceptor::setErrorCallback(ErrorCallback callback) {
    error_callback_ = std::move(callback);
}

void ConnectionAcceptor::startAccept() {
    if (!running_.load() || shutdown_requested_.load() || !acceptor_) {
        return;
    }

    // Start async accept operation
    acceptor_->async_accept(
        [this](beast::error_code ec, tcp::socket socket) {
            onAccept(ec, std::move(socket));
        });
}

void ConnectionAcceptor::onAccept(beast::error_code ec, tcp::socket socket) {
    if (ec) {
        // Handle accept error
        handleAcceptorError("async_accept", ec);
        return;
    }

    if (shutdown_requested_.load()) {
        return; // Shutting down, ignore new connections
    }

    // Successfully accepted connection
    try {
        auto endpoint = socket.remote_endpoint();
        std::cout << "[" << COMPONENT_NAME << "] Accepted connection from "
                  << endpoint.address().to_string() << ":" << endpoint.port() << std::endl;

        // Notify parent via callback (SSOT for notification)
        if (accept_callback_) {
            accept_callback_(std::move(socket));
        }

    } catch (const std::exception& e) {
        utils::ErrorHandler::handleException(COMPONENT_NAME, "connection handling", e,
                                           data::ErrorSeverity::ERROR);
    }

    // Continue accepting more connections
    startAccept();
}

void ConnectionAcceptor::handleAcceptorError(const std::string& operation_name,
                                           beast::error_code ec) {
    const std::string error_message = operation_name + " failed: " + ec.message();

    // Determine error severity
    data::ErrorSeverity severity = data::ErrorSeverity::ERROR;
    if (ec == boost::asio::error::operation_aborted) {
        // Normal shutdown - lower severity
        severity = data::ErrorSeverity::INFO;
    }

    utils::ErrorHandler::handleBoostError(COMPONENT_NAME, error_message, ec, severity);

    // Notify parent via callback (SSOT for error notification)
    if (error_callback_) {
        error_callback_(error_message, ec);
    }

    // Continue accepting if not a fatal error and still running
    if (running_.load() && !shutdown_requested_.load() &&
        ec != boost::asio::error::operation_aborted) {
        startAccept();
    }
}

} // namespace siren::websocket