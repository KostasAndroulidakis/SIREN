/**
 * @file session_manager.cpp
 * @brief Implementation of military-grade WebSocket session manager
 * @author unoRadar Project
 * @date 2025
 *
 * Specialized manager for WebSocket session lifecycle management.
 * Follows SRP by focusing exclusively on session creation, tracking,
 * health monitoring, and cleanup.
 */

#include "websocket/session_manager.hpp"
#include "websocket/server.hpp"
#include <iostream>

namespace siren::websocket {

WebSocketSessionManager::WebSocketSessionManager()
    : total_sessions_created_(0)
    , total_sessions_closed_(0)
{
    std::cout << "[WebSocketSessionManager] Initializing session manager" << std::endl;
}

WebSocketSessionManager::~WebSocketSessionManager() {
    closeAllSessions();
    std::cout << "[WebSocketSessionManager] Session manager destroyed" << std::endl;
}

std::shared_ptr<WebSocketSession> WebSocketSessionManager::createSession(tcp::socket&& socket,
                                                                        std::weak_ptr<WebSocketServer> server_weak_ptr) {
    // Create new session
    auto session = std::make_shared<WebSocketSession>(std::move(socket), server_weak_ptr);

    // Add to active sessions
    addSession(session);

    // Track creation
    total_sessions_created_.fetch_add(1);

    std::cout << "[WebSocketSessionManager] ✅ Session created: " << session->getClientEndpoint() << std::endl;

    return session;
}

void WebSocketSessionManager::addSession(std::shared_ptr<WebSocketSession> session) {
    {
        std::lock_guard<std::mutex> lock(sessions_mutex_);
        active_sessions_.insert(session);
    }

    // Notify callback about new connection
    notifySessionCallback(session->getClientEndpoint(), true);

    std::cout << "[WebSocketSessionManager] 🔌 Session added: " << session->getClientEndpoint()
              << " (total: " << getActiveSessionCount() << ")" << std::endl;
}

void WebSocketSessionManager::removeSession(std::shared_ptr<WebSocketSession> session) {
    std::string endpoint = session->getClientEndpoint();

    {
        std::lock_guard<std::mutex> lock(sessions_mutex_);
        active_sessions_.erase(session);
    }

    // Track closure
    total_sessions_closed_.fetch_add(1);

    // Notify callback about disconnection
    notifySessionCallback(endpoint, false);

    std::cout << "[WebSocketSessionManager] 🔌 Session removed: " << endpoint
              << " (remaining: " << getActiveSessionCount() << ")" << std::endl;
}

std::unordered_set<std::shared_ptr<WebSocketSession>> WebSocketSessionManager::getActiveSessions() const {
    std::lock_guard<std::mutex> lock(sessions_mutex_);
    return active_sessions_;
}

size_t WebSocketSessionManager::getActiveSessionCount() const noexcept {
    std::lock_guard<std::mutex> lock(sessions_mutex_);
    return active_sessions_.size();
}

void WebSocketSessionManager::cleanupClosedSessions() {
    std::lock_guard<std::mutex> lock(sessions_mutex_);

    size_t initial_count = active_sessions_.size();

    auto it = active_sessions_.begin();
    while (it != active_sessions_.end()) {
        if (!(*it)->isAlive()) {
            it = active_sessions_.erase(it);
            total_sessions_closed_.fetch_add(1);
        } else {
            ++it;
        }
    }

    size_t cleaned_count = initial_count - active_sessions_.size();
    if (cleaned_count > 0) {
        std::cout << "[WebSocketSessionManager] 🧹 Cleaned up " << cleaned_count
                  << " closed sessions" << std::endl;
    }
}

void WebSocketSessionManager::closeAllSessions() {
    std::lock_guard<std::mutex> lock(sessions_mutex_);

    size_t session_count = active_sessions_.size();
    if (session_count > 0) {
        std::cout << "[WebSocketSessionManager] 🛑 Closing " << session_count << " active sessions..." << std::endl;

        for (auto& session : active_sessions_) {
            session->close();
        }

        active_sessions_.clear();
        total_sessions_closed_.fetch_add(session_count);
    }
}

bool WebSocketSessionManager::isSessionActive(std::shared_ptr<WebSocketSession> session) const {
    std::lock_guard<std::mutex> lock(sessions_mutex_);
    return active_sessions_.find(session) != active_sessions_.end();
}

void WebSocketSessionManager::setSessionCallback(SessionCallback callback) {
    session_callback_ = std::move(callback);
}

WebSocketSessionManager::SessionStats WebSocketSessionManager::getSessionStats() const {
    std::lock_guard<std::mutex> lock(sessions_mutex_);

    return SessionStats{
        .total_sessions = total_sessions_created_.load(),
        .active_sessions = active_sessions_.size(),
        .closed_sessions = total_sessions_closed_.load()
    };
}

void WebSocketSessionManager::notifySessionCallback(const std::string& endpoint, bool connected) {
    if (session_callback_) {
        session_callback_(endpoint, connected);
    }
}

} // namespace siren::websocket