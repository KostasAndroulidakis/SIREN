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
#include "websocket/server_lifecycle_manager.hpp"
#include "websocket/data_broadcast_coordinator.hpp"
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
    session_manager_ = std::make_shared<SessionManager>();
    message_broadcaster_ = std::make_unique<MessageBroadcaster>();
    statistics_collector_ = std::make_shared<StatisticsCollector>();
    event_handler_ = std::make_unique<ServerEventHandler>(session_manager_, statistics_collector_);
    lifecycle_manager_ = std::make_unique<ServerLifecycleManager>(
        connection_acceptor_, session_manager_, message_broadcaster_, 
        statistics_collector_, event_handler_, port_);
    broadcast_coordinator_ = std::make_unique<DataBroadcastCoordinator>(
        session_manager_, message_broadcaster_);
}

WebSocketServer::~WebSocketServer() {
    if (running_.load()) {
        stop();
    }
}

bool WebSocketServer::initialize() {
    return lifecycle_manager_->initialize(weak_from_this());
}

bool WebSocketServer::start() {
    return lifecycle_manager_->start(running_);
}

void WebSocketServer::stop() {
    lifecycle_manager_->stop(running_, shutdown_requested_);
}

bool WebSocketServer::isRunning() const noexcept {
    return running_.load();
}

void WebSocketServer::broadcastSonarData(const data::SonarDataPoint& data) {
    broadcast_coordinator_->broadcastSonarData(data, running_);
}

void WebSocketServer::broadcastPerformanceMetrics(const data::PerformanceMetrics& metrics) {
    broadcast_coordinator_->broadcastPerformanceMetrics(metrics, running_);
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