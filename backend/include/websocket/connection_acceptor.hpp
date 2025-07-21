/**
 * @file connection_acceptor.hpp
 * @brief TCP connection acceptor with single responsibility - MISRA C++ compliant
 * @author SIREN Project
 * @date 2025
 *
 * Single Responsibility: Accept incoming TCP connections ONLY
 * MISRA C++ 2023 compliant - Rule 5.0.1 (no magic numbers), Rule 21.2.1 (RAII)
 */

#pragma once

#include <memory>
#include <functional>
#include <atomic>
#include <boost/asio.hpp>
#include <boost/beast.hpp>

namespace siren::websocket {

namespace beast = boost::beast;
using tcp = boost::asio::ip::tcp;

/**
 * @brief TCP connection acceptor with single responsibility
 *
 * RESPONSIBILITIES:
 * - Accept incoming TCP connections ONLY
 * - Notify parent when connection is accepted
 * - Handle acceptor-level errors ONLY
 *
 * NOT RESPONSIBLE FOR:
 * - WebSocket handshakes (handled by sessions)
 * - Connection management (handled by session manager)
 * - Message processing (handled by message broadcaster)
 * - Statistics (handled by statistics collector)
 *
 * MISRA C++ Compliance:
 * - Rule 5.0.1: All constants defined, no magic numbers
 * - Rule 21.2.1: RAII for all resources
 * - Rule 8.4.1: Single responsibility per class
 */
class ConnectionAcceptor {
public:
    /// Callback type for accepted connections (SSOT)
    using AcceptCallback = std::function<void(tcp::socket)>;
    
    /// Callback type for acceptor errors (SSOT)
    using ErrorCallback = std::function<void(const std::string&, beast::error_code)>;

    /**
     * @brief Constructor - RAII initialization
     * @param io_context Boost.Asio I/O context
     * @param port TCP port for accepting connections
     */
    explicit ConnectionAcceptor(boost::asio::io_context& io_context, uint16_t port);

    /**
     * @brief Destructor - RAII cleanup
     */
    ~ConnectionAcceptor();

    // MISRA C++ Rule 12.1.1: Disable copy/move for resource management
    ConnectionAcceptor(const ConnectionAcceptor&) = delete;
    ConnectionAcceptor& operator=(const ConnectionAcceptor&) = delete;
    ConnectionAcceptor(ConnectionAcceptor&&) = delete;
    ConnectionAcceptor& operator=(ConnectionAcceptor&&) = delete;

    /**
     * @brief Initialize the TCP acceptor
     * @return true if initialization successful
     */
    bool initialize();

    /**
     * @brief Start accepting connections
     * @return true if started successfully
     */
    bool start();

    /**
     * @brief Stop accepting connections
     */
    void stop();

    /**
     * @brief Check if acceptor is running
     */
    bool isRunning() const noexcept;

    /**
     * @brief Set callback for accepted connections (SSOT for callback setting)
     * @param callback Function to call when connection is accepted
     */
    void setAcceptCallback(AcceptCallback callback);

    /**
     * @brief Set callback for acceptor errors (SSOT for error handling)
     * @param callback Function to call when acceptor error occurs
     */
    void setErrorCallback(ErrorCallback callback);

private:
    // Core components - RAII managed
    boost::asio::io_context& io_context_;
    uint16_t port_;
    std::unique_ptr<tcp::acceptor> acceptor_;

    // State management - atomic for thread safety
    std::atomic<bool> running_;
    std::atomic<bool> shutdown_requested_;

    // Callbacks - SSOT for notification
    AcceptCallback accept_callback_;
    ErrorCallback error_callback_;

    /**
     * @brief Handle accepted connection (SSOT for accept logic)
     * @param ec Error code from accept operation
     * @param socket Accepted TCP socket
     */
    void onAccept(beast::error_code ec, tcp::socket socket);

    /**
     * @brief Start next accept operation (SSOT for async accept)
     */
    void startAccept();

    /**
     * @brief Handle acceptor errors (SSOT for error handling)
     * @param operation_name Name of failed operation
     * @param ec Error code
     */
    void handleAcceptorError(const std::string& operation_name, beast::error_code ec);
};

} // namespace siren::websocket