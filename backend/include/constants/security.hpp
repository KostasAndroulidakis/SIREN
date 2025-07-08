/**
 * @file security.hpp
 * @brief Security and cryptographic constants for military-grade radar system
 * @author unoRadar Defense Systems
 * @date 2025
 * @classification UNCLASSIFIED
 *
 * TLS/SSL security configuration constants and cryptographic parameters.
 * Defines all security-related constants following MISRA C++ guidelines.
 *
 * MISRA C++ Compliance: All constants are constexpr and strongly typed
 * SRP: Single responsibility - security constants only
 * SSOT: Single source of truth for all security parameters
 */

#pragma once

#include <cstdint>
#include <chrono>

namespace unoradar { namespace constants { namespace security {

/// TLS/SSL protocol configuration
namespace tls {
    /// TLS protocol version enumeration for type safety
    enum class Version : uint8_t {
        TLS_1_2 = 12,
        TLS_1_3 = 13
    };

    /// Minimum TLS version (TLS 1.3 for military-grade security)
    constexpr Version MIN_VERSION = Version::TLS_1_3;

    /// Maximum TLS version supported
    constexpr Version MAX_VERSION = Version::TLS_1_3;

    /// SSL handshake timeout
    constexpr auto HANDSHAKE_TIMEOUT = std::chrono::seconds(30);

    /// Certificate validation timeout
    constexpr auto CERT_VALIDATION_TIMEOUT = std::chrono::seconds(10);

    /// SSL context verification mode flags
    constexpr uint32_t VERIFY_PEER = 0x01;
    constexpr uint32_t VERIFY_FAIL_IF_NO_PEER_CERT = 0x02;
    constexpr uint32_t VERIFY_CLIENT_ONCE = 0x04;
}

/// Cryptographic cipher suite configuration
namespace cipher {
    /// TLS 1.3 cipher suite identifiers (RFC 8446)
    constexpr uint16_t TLS_AES_128_GCM_SHA256 = 0x1301;
    constexpr uint16_t TLS_AES_256_GCM_SHA384 = 0x1302;
    constexpr uint16_t TLS_CHACHA20_POLY1305_SHA256 = 0x1303;

    /// Preferred cipher suite for military-grade security
    constexpr uint16_t PREFERRED_CIPHER = TLS_AES_256_GCM_SHA384;

    /// Fallback cipher suite
    constexpr uint16_t FALLBACK_CIPHER = TLS_CHACHA20_POLY1305_SHA256;
}

/// Certificate management configuration
namespace certificate {
    /// Certificate key size in bits (minimum for military-grade)
    constexpr uint32_t MIN_KEY_SIZE_BITS = 2048;

    /// Preferred certificate key size
    constexpr uint32_t PREFERRED_KEY_SIZE_BITS = 4096;

    /// Certificate validity period in days
    constexpr uint32_t VALIDITY_DAYS = 365;

    /// Certificate refresh threshold in days
    constexpr uint32_t REFRESH_THRESHOLD_DAYS = 30;

    /// Maximum certificate chain depth
    constexpr uint8_t MAX_CHAIN_DEPTH = 5;
}

/// Security validation thresholds
namespace validation {
    /// Maximum failed authentication attempts
    constexpr uint8_t MAX_AUTH_FAILURES = 3;

    /// Authentication lockout duration
    constexpr auto AUTH_LOCKOUT_DURATION = std::chrono::minutes(15);

    /// Session timeout duration
    constexpr auto SESSION_TIMEOUT = std::chrono::hours(8);

    /// Security audit log retention period in hours (90 days)
    constexpr auto AUDIT_RETENTION = std::chrono::hours(90 * 24);
}

} } } // namespace unoradar::constants::security