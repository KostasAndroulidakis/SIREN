/**
 * @file session_manager.cpp
 * @brief Implementation of WebSocket session lifecycle manager - MISRA C++ compliant
 * @author SIREN Project
 * @date 2025
 *
 * Single Responsibility: Manage WebSocket session lifecycle ONLY
 */

#include "websocket/session_manager.hpp"
#include "websocket/session.hpp" // For WebSocketSession definition
#include "utils/error_handler.hpp"
#include <iostream>
#include <algorithm>

namespace siren::websocket {

SessionManager::SessionManager()
    : sessions_mutex_()
    , active_sessions_()
    , session_callback_(nullptr)
    , cleanup_counter_(0)
{
    std::cout << "[" << COMPONENT_NAME << "] Initializing session manager" << std::endl;
    
    // Reserve reasonable capacity to avoid frequent reallocations
    active_sessions_.reserve(32); // SSOT for initial capacity
}

SessionManager::~SessionManager() {
    closeAllSessions();
}

std::shared_ptr<WebSocketSession> SessionManager::createSession(
    tcp::socket&& socket, 
    std::weak_ptr<WebSocketServer> server_weak_ptr) {
    
    try {
        // Create new session (RAII managed)
        auto session = std::make_shared<WebSocketSession>(std::move(socket), server_weak_ptr);

        // Get client endpoint for logging
        const std::string endpoint = session->getClientEndpoint();

        // Add to active sessions (thread-safe)
        {
            std::lock_guard<std::mutex> lock(sessions_mutex_);
            active_sessions_.push_back(session);
        }

        // Notify session creation
        notifySessionEvent(endpoint, true);

        // Check if cleanup is needed
        checkPeriodicCleanup();

        std::cout << "[" << COMPONENT_NAME << "] Created session for " << endpoint 
                  << " (total: " << getActiveSessionCount() << ")" << std::endl;

        return session;

    } catch (const std::exception& e) {
        utils::ErrorHandler::handleException(COMPONENT_NAME, "session creation", e,
                                           data::ErrorSeverity::ERROR);
        return nullptr;
    }
}

void SessionManager::removeSession(std::shared_ptr<WebSocketSession> session) {
    if (!session) {
        return; // Invalid session
    }

    const std::string endpoint = session->getClientEndpoint();
    bool removed = false;

    // Remove from active sessions (thread-safe)
    {
        std::lock_guard<std::mutex> lock(sessions_mutex_);
        
        auto it = std::find(active_sessions_.begin(), active_sessions_.end(), session);
        if (it != active_sessions_.end()) {
            active_sessions_.erase(it);
            removed = true;
        }
    }

    if (removed) {
        // Notify session removal
        notifySessionEvent(endpoint, false);

        // Check if cleanup is needed
        checkPeriodicCleanup();

        std::cout << "[" << COMPONENT_NAME << "] Removed session for " << endpoint 
                  << " (total: " << getActiveSessionCount() << ")" << std::endl;
    }
}

SessionManager::SessionContainer SessionManager::getActiveSessions() const {
    std::lock_guard<std::mutex> lock(sessions_mutex_);
    return active_sessions_; // Copy for thread safety
}

size_t SessionManager::getActiveSessionCount() const noexcept {
    std::lock_guard<std::mutex> lock(sessions_mutex_);
    return active_sessions_.size();
}

void SessionManager::closeAllSessions() {
    std::cout << "[" << COMPONENT_NAME << "] Closing all sessions..." << std::endl;

    SessionContainer sessions_to_close;

    // Get copy of all sessions (thread-safe)
    {
        std::lock_guard<std::mutex> lock(sessions_mutex_);
        sessions_to_close = active_sessions_;
    }

    // Close each session
    for (auto& session : sessions_to_close) {
        if (session && session->isAlive()) {
            try {
                session->close();
            } catch (const std::exception& e) {
                utils::ErrorHandler::handleException(COMPONENT_NAME, "session closure", e,
                                                   data::ErrorSeverity::WARNING);
            }
        }
    }

    // Clear the active sessions container
    {
        std::lock_guard<std::mutex> lock(sessions_mutex_);
        active_sessions_.clear();
    }

    std::cout << "[" << COMPONENT_NAME << "] All sessions closed" << std::endl;
}

void SessionManager::cleanupClosedSessions() {
    size_t initial_count = 0;
    size_t cleaned_count = 0;

    // Remove dead sessions (thread-safe)
    {
        std::lock_guard<std::mutex> lock(sessions_mutex_);
        initial_count = active_sessions_.size();

        // Remove sessions that are no longer alive
        active_sessions_.erase(
            std::remove_if(active_sessions_.begin(), active_sessions_.end(),
                          [](const std::shared_ptr<WebSocketSession>& session) {
                              return !session || !session->isAlive();
                          }),
            active_sessions_.end());

        cleaned_count = initial_count - active_sessions_.size();
    }

    if (cleaned_count > 0) {
        std::cout << "[" << COMPONENT_NAME << "] Cleaned up " << cleaned_count 
                  << " closed sessions (remaining: " << getActiveSessionCount() << ")" << std::endl;
    }
}

void SessionManager::setSessionCallback(SessionEventCallback callback) {
    session_callback_ = std::move(callback);
}

void SessionManager::notifySessionEvent(const std::string& endpoint, bool connected) {
    if (session_callback_) {
        try {
            session_callback_(endpoint, connected);
        } catch (const std::exception& e) {
            utils::ErrorHandler::handleException(COMPONENT_NAME, "session event callback", e,
                                               data::ErrorSeverity::WARNING);
        }
    }
}

void SessionManager::checkPeriodicCleanup() {
    // Increment cleanup counter atomically
    size_t count = cleanup_counter_.fetch_add(1);

    // Perform cleanup every N session changes (SSOT for cleanup frequency)
    if (count % CLEANUP_THRESHOLD == 0) {
        cleanupClosedSessions();
    }
}

} // namespace siren::websocket