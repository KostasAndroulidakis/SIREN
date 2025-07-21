#ifndef SIREN_SONAR_DATA_BUFFER_H
#define SIREN_SONAR_DATA_BUFFER_H

// SIREN Sonar System
// Sonar Data Buffer - Single Responsibility: Data Point Storage with Decay ONLY
// Compliant with MISRA C++ 2023, SRP, SSOT

#include <vector>
#include <cstdint>
#include "data/SonarDataParser.h"

namespace siren {
namespace visualization {

/**
 * @brief Sonar data buffer - Single Responsibility: Data Point Management
 *
 * This class has ONE job: Store and manage sonar data points with time-based decay.
 * It does NOT render, convert coordinates, or handle UI.
 *
 * Features:
 * - Fixed-size circular buffer for predictable memory usage
 * - Time-based data point expiration
 * - Decay factor calculation for fading effects
 * - Efficient point retrieval
 *
 * MISRA C++ Compliance:
 * - Rule 12.4.1: No dynamic allocation after initialization
 * - Rule 21.2.1: RAII principles
 * - Rule 5.0.1: No magic numbers
 */
class SonarDataBuffer final
{
public:
    /**
     * @brief Data point with timestamp for decay calculation
     */
    struct BufferedPoint {
        std::uint16_t angle{0};
        std::uint16_t distance{0};
        std::uint64_t timestamp{0};
        bool valid{false};

        /**
         * @brief Check if point has expired
         * @param currentTime Current time in milliseconds
         * @param lifetimeMs Point lifetime in milliseconds
         * @return True if expired
         */
        [[nodiscard]] bool isExpired(std::uint64_t currentTime, std::uint64_t lifetimeMs) const {
            return (currentTime > timestamp) && ((currentTime - timestamp) > lifetimeMs);
        }

        /**
         * @brief Calculate decay factor for visual fading
         * @param currentTime Current time in milliseconds
         * @param fadeStartMs Time before expiry to start fading
         * @param lifetimeMs Total lifetime in milliseconds
         * @return Decay factor (0.0 to 1.0)
         */
        [[nodiscard]] double getDecayFactor(std::uint64_t currentTime,
                                          std::uint64_t fadeStartMs,
                                          std::uint64_t lifetimeMs) const;
    };

    /**
     * @brief Construct buffer with fixed capacity
     * @param capacity Maximum number of points to store
     */
    explicit SonarDataBuffer(std::size_t capacity = DEFAULT_CAPACITY);

    /**
     * @brief Add new data point to buffer
     * @param dataPoint Sonar data point to add
     * @param timestamp Current timestamp in milliseconds
     */
    void addPoint(const data::SonarDataPoint& dataPoint, std::uint64_t timestamp);

    /**
     * @brief Remove expired points from buffer
     * @param currentTime Current time in milliseconds
     * @param lifetimeMs Point lifetime in milliseconds
     */
    void removeExpiredPoints(std::uint64_t currentTime, std::uint64_t lifetimeMs);

    /**
     * @brief Get all valid points
     * @return Vector of buffered points
     */
    [[nodiscard]] const std::vector<BufferedPoint>& getPoints() const { return m_points; }

    /**
     * @brief Get points within angle range
     * @param minAngle Minimum angle (inclusive)
     * @param maxAngle Maximum angle (inclusive)
     * @param output Output vector for filtered points
     */
    void getPointsInAngleRange(std::uint16_t minAngle,
                              std::uint16_t maxAngle,
                              std::vector<BufferedPoint>& output) const;

    /**
     * @brief Clear all buffered points
     */
    void clear();

    /**
     * @brief Get current number of points
     * @return Number of points in buffer
     */
    [[nodiscard]] std::size_t size() const { return m_points.size(); }

    /**
     * @brief Check if buffer is empty
     * @return True if no points stored
     */
    [[nodiscard]] bool empty() const { return m_points.empty(); }

    /**
     * @brief Get buffer capacity
     * @return Maximum number of points
     */
    [[nodiscard]] std::size_t capacity() const { return m_capacity; }

    /**
     * @brief Set point lifetime
     * @param lifetimeMs Lifetime in milliseconds
     */
    void setPointLifetime(std::uint64_t lifetimeMs);

    /**
     * @brief Set fade start time
     * @param fadeStartMs Time before expiry to start fading
     */
    void setFadeStartTime(std::uint64_t fadeStartMs);

private:
    // Buffer storage
    std::vector<BufferedPoint> m_points;
    std::size_t m_capacity;

    // Timing parameters
    std::uint64_t m_pointLifetimeMs;
    std::uint64_t m_fadeStartMs;

    // Buffer management constants (SSOT)
    static constexpr std::size_t DEFAULT_CAPACITY = 1000;
    static constexpr std::uint64_t DEFAULT_LIFETIME_MS = 5000;  // 5 seconds
    static constexpr std::uint64_t DEFAULT_FADE_START_MS = 3000;  // Start fading at 3 seconds
};

} // namespace visualization
} // namespace siren

#endif // SIREN_SONAR_DATA_BUFFER_H