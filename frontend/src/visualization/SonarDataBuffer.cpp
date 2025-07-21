// SIREN Military-Grade Sonar System
// Sonar Data Buffer Implementation
// Single Responsibility: Data Point Storage with Decay ONLY

#include "visualization/SonarDataBuffer.h"
#include <algorithm>

namespace siren {
namespace visualization {

double SonarDataBuffer::BufferedPoint::getDecayFactor(std::uint64_t currentTime,
                                                      std::uint64_t fadeStartMs,
                                                      std::uint64_t lifetimeMs) const
{
    if (!valid || currentTime < timestamp) {
        return 0.0;
    }

    const std::uint64_t age = currentTime - timestamp;

    // Point hasn't started fading yet
    if (age < fadeStartMs) {
        return 1.0;
    }

    // Point has expired
    if (age >= lifetimeMs) {
        return 0.0;
    }

    // Calculate linear fade
    const std::uint64_t fadeTime = age - fadeStartMs;
    const std::uint64_t fadeDuration = lifetimeMs - fadeStartMs;

    if (fadeDuration == 0) {
        return 0.0;  // Prevent division by zero
    }

    const double factor = 1.0 - (static_cast<double>(fadeTime) / static_cast<double>(fadeDuration));
    return std::max(0.0, std::min(1.0, factor));  // Clamp to [0, 1]
}

SonarDataBuffer::SonarDataBuffer(std::size_t capacity)
    : m_capacity(capacity)
    , m_pointLifetimeMs(DEFAULT_LIFETIME_MS)
    , m_fadeStartMs(DEFAULT_FADE_START_MS)
{
    m_points.reserve(capacity);
}

void SonarDataBuffer::addPoint(const data::SonarDataPoint& dataPoint, std::uint64_t timestamp)
{
    // Check if buffer is full
    if (m_points.size() >= m_capacity) {
        // Remove oldest point (FIFO)
        m_points.erase(m_points.begin());
    }

    // Create buffered point
    BufferedPoint point;
    point.angle = dataPoint.angle;
    point.distance = dataPoint.distance;
    point.timestamp = timestamp;
    point.valid = dataPoint.valid;

    // Add to buffer
    m_points.push_back(point);
}

void SonarDataBuffer::removeExpiredPoints(std::uint64_t currentTime, std::uint64_t lifetimeMs)
{
    // Remove all expired points
    m_points.erase(
        std::remove_if(m_points.begin(), m_points.end(),
            [currentTime, lifetimeMs](const BufferedPoint& point) {
                return point.isExpired(currentTime, lifetimeMs);
            }),
        m_points.end()
    );
}

void SonarDataBuffer::getPointsInAngleRange(std::uint16_t minAngle,
                                           std::uint16_t maxAngle,
                                           std::vector<BufferedPoint>& output) const
{
    output.clear();
    output.reserve(m_points.size());

    // Copy points within angle range
    std::copy_if(m_points.begin(), m_points.end(), std::back_inserter(output),
        [minAngle, maxAngle](const BufferedPoint& point) {
            return point.valid && point.angle >= minAngle && point.angle <= maxAngle;
        });
}

void SonarDataBuffer::clear()
{
    m_points.clear();
}

void SonarDataBuffer::setPointLifetime(std::uint64_t lifetimeMs)
{
    m_pointLifetimeMs = lifetimeMs;
}

void SonarDataBuffer::setFadeStartTime(std::uint64_t fadeStartMs)
{
    m_fadeStartMs = fadeStartMs;
}

} // namespace visualization
} // namespace siren