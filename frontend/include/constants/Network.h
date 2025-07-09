/**
 * CLASSIFICATION: UNCLASSIFIED
 * EXPORT CONTROL: NOT SUBJECT TO EAR/ITAR
 * CONTRACT: UNORADAR-2025
 *
 * @file Network.h
 * @brief Network-related constants for WebSocket communication
 * @author unoRadar Defense Systems
 * @date 2025
 *
 * MISRA C++ 2008 Compliant
 * DO-178C Level A Certifiable
 */

#ifndef UNORADAR_CONSTANTS_NETWORK_H
#define UNORADAR_CONSTANTS_NETWORK_H

#include <cstdint>
#include <cstddef>

namespace unoRadar {
namespace Constants {
namespace Network {

// WebSocket Configuration
constexpr char BACKEND_URL[] = "ws://localhost:8080";
constexpr char BACKEND_SECURE_URL[] = "wss://localhost:8443";

// Connection Parameters
constexpr std::int32_t RECONNECT_INTERVAL_MS = 5000;
constexpr std::int32_t CONNECTION_TIMEOUT_MS = 10000;
constexpr std::int32_t MAX_RECONNECT_ATTEMPTS = 10;
constexpr std::int32_t PING_INTERVAL_MS = 30000;
constexpr std::int32_t PONG_TIMEOUT_MS = 5000;

// Buffer Sizes
constexpr std::size_t MAX_MESSAGE_SIZE = 65536;  // 64KB
constexpr std::size_t RECEIVE_BUFFER_SIZE = 8192;  // 8KB

} // namespace Network
} // namespace Constants
} // namespace unoRadar

#endif // UNORADAR_CONSTANTS_NETWORK_H