/**
 * @file server_lifecycle_manager.cpp
 * @brief Implementation of WebSocket server lifecycle manager - MISRA C++ compliant
 * @author KostasAndroulidakis
 * @date 2025
 *
 * Single Responsibility: Server component lifecycle management
 */

#include "websocket/server_lifecycle_manager.hpp"
#include "websocket/connection_acceptor.hpp"
#include "websocket/session_manager.hpp"
#include "websocket/message_broadcaster.hpp"
#include "websocket/statistics_collector.hpp"
#include "websocket/server_event_handler.hpp"
#include "websocket/server.hpp"
#include "constants/message.hpp"
#include "utils/error_handler.hpp"
#include <iostream>

namespace siren::websocket {

namespace cnst = siren::constants;

// SSOT for lifecycle manager constants (MISRA C++ Rule 5.0.1)
namespace {
    constexpr const char* COMPONENT_NAME = "ServerLifecycleManager";
}

ServerLifecycleManager::ServerLifecycleManager(
    std::unique_ptr<ConnectionAcceptor>& connection_acceptor,
    std::unique_ptr<SessionManager>& session_manager,
    std::unique_ptr<MessageBroadcaster>& message_broadcaster,
    std::unique_ptr<StatisticsCollector>& statistics_collector,
    std::unique_ptr<ServerEventHandler>& event_handler,
    uint16_t port)
    : connection_acceptor_(connection_acceptor)
    , session_manager_(session_manager)
    , message_broadcaster_(message_broadcaster)
    , statistics_collector_(statistics_collector)
    , event_handler_(event_handler)
    , port_(port)
{
    std::cout << "[" << COMPONENT_NAME << "] Initializing lifecycle manager for port " << port_ << std::endl;
}

bool ServerLifecycleManager::initialize(std::weak_ptr<WebSocketServer> server_weak_ptr) {
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
            [this, server_weak_ptr](tcp::socket socket) {
                event_handler_->onConnectionAccepted(std::move(socket), server_weak_ptr);
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

bool ServerLifecycleManager::start(std::atomic<bool>& running) {
    if (running.load()) {
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

        running.store(true);

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

void ServerLifecycleManager::stop(std::atomic<bool>& running, std::atomic<bool>& shutdown_requested) {
    if (!running.load()) {
        return;
    }

    std::cout << cnst::message::websocket_status::SERVER_PREFIX << " "
              << cnst::message::websocket_status::STOPPING_SERVER << std::endl;

    shutdown_requested.store(true);
    running.store(false);

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

} // namespace siren::websocket