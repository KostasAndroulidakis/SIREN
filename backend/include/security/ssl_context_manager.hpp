/**
 * @file ssl_context_manager.hpp
 * @brief Military-grade SSL/TLS context manager for secure WebSocket communications
 * @author KostasAndroulidakis
 * @date 2025
 * @classification UNCLASSIFIED
 *
 * Provides secure SSL/TLS context configuration for WebSocket Secure (WSS) connections
 * with military-grade cryptographic standards and certificate management.
 *
 * MISRA C++ Compliance: RAII pattern, exception safety, no raw pointers
 * Security: TLS 1.3 only, FIPS 140-2 compatible cipher suites
 * SRP: Single responsibility - SSL context management only
 */

#pragma once

#include <memory>
#include <string>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/system/error_code.hpp>
#include "constants/security.hpp"

namespace siren::security {

namespace ssl = boost::asio::ssl;
namespace cnst = siren::constants::security;

/**
 * @brief Military-grade SSL context configuration and management
 *
 * Provides secure SSL/TLS context with:
 * - TLS 1.3 minimum version
 * - Military-grade cipher suites
 * - Certificate validation
 * - FIPS 140-2 compatibility
 */
class SSLContextManager {
public:
    /**
     * @brief Constructor
     * @param io_context Boost.Asio I/O context
     */
    explicit SSLContextManager(boost::asio::io_context& io_context);

    /**
     * @brief Destructor - ensures secure cleanup
     */
    ~SSLContextManager() = default;

    // Non-copyable, non-movable for security (MISRA C++)
    SSLContextManager(const SSLContextManager&) = delete;
    SSLContextManager& operator=(const SSLContextManager&) = delete;
    SSLContextManager(SSLContextManager&&) = delete;
    SSLContextManager& operator=(SSLContextManager&&) = delete;

    /**
     * @brief Initialize SSL context with military-grade security
     * @param cert_file Path to certificate file
     * @param key_file Path to private key file
     * @param ca_file Path to CA certificate file (optional)
     * @return true if initialization successful
     */
    bool initialize(const std::string& cert_file,
                   const std::string& key_file,
                   const std::string& ca_file = "");

    /**
     * @brief Get SSL context for secure connections
     * @return SSL context reference
     * @throws std::runtime_error if not initialized
     */
    ssl::context& getContext();

    /**
     * @brief Check if SSL context is initialized
     * @return true if initialized
     */
    bool isInitialized() const noexcept;

    /**
     * @brief Get TLS version being used
     * @return TLS version enumeration
     */
    cnst::tls::Version getTLSVersion() const noexcept;

    /**
     * @brief Validate certificate file
     * @param cert_file Certificate file to validate
     * @return true if valid
     */
    bool validateCertificateFile(const std::string& cert_file) const;

    /**
     * @brief Validate private key file
     * @param key_file Private key file to validate
     * @return true if valid
     */
    bool validatePrivateKeyFile(const std::string& key_file) const;

private:
    boost::asio::io_context& io_context_;
    std::unique_ptr<ssl::context> ssl_context_;
    bool initialized_;
    cnst::tls::Version tls_version_;

    /**
     * @brief Configure SSL context with military-grade security
     * @return true if configuration successful
     */
    bool configureSecurityOptions();

    /**
     * @brief Set cipher suites for TLS 1.3
     * @return true if configuration successful
     */
    bool configureCipherSuites();

    /**
     * @brief Configure certificate verification
     * @return true if configuration successful
     */
    bool configureCertificateVerification();

    /**
     * @brief Certificate verification callback
     * @param preverified Pre-verification result
     * @param ctx Verification context
     * @return true if certificate is valid
     */
    static bool verifyCertificate(bool preverified, ssl::verify_context& ctx);

    /**
     * @brief Load certificate file
     * @param cert_file Certificate file path
     * @return true if loaded successfully
     */
    bool loadCertificate(const std::string& cert_file);

    /**
     * @brief Load private key file
     * @param key_file Private key file path
     * @return true if loaded successfully
     */
    bool loadPrivateKey(const std::string& key_file);

    /**
     * @brief Load CA certificate file
     * @param ca_file CA certificate file path
     * @return true if loaded successfully
     */
    bool loadCACertificate(const std::string& ca_file);
};

} // namespace siren::security