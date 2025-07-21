// SIREN Sonar System
// Polar Coordinate Converter Implementation
// Single Responsibility: Coordinate Transformation ONLY

#include "visualization/PolarCoordinateConverter.h"
#include <algorithm>

namespace siren {
namespace visualization {

void PolarCoordinateConverter::setCenterPoint(const QPoint& center)
{
    m_center = center;
}

void PolarCoordinateConverter::setDisplayRadius(int radius)
{
    m_displayRadius = std::max(1, radius);  // Ensure positive radius
}

void PolarCoordinateConverter::setMaxDistance(std::uint16_t maxDistance)
{
    m_maxDistance = std::max(std::uint16_t(1), maxDistance);  // Ensure positive distance
}

QPoint PolarCoordinateConverter::polarToScreen(std::uint16_t angle, std::uint16_t distance) const
{
    // Scale distance to display radius
    const double scaleFactor = static_cast<double>(m_displayRadius) / static_cast<double>(m_maxDistance);
    const double scaledDistance = static_cast<double>(distance) * scaleFactor;

    // Convert angle to radians
    // Sonar display convention: 0° is right, angles increase counter-clockwise
    const double radians = degreesToRadians(angle);

    // Calculate Cartesian coordinates
    // Note: Y-axis is inverted in screen coordinates (positive Y goes down)
    const int x = m_center.x() + static_cast<int>(scaledDistance * std::cos(radians));
    const int y = m_center.y() - static_cast<int>(scaledDistance * std::sin(radians));

    return QPoint(x, y);
}

bool PolarCoordinateConverter::screenToPolar(const QPoint& point,
                                            std::uint16_t& angle,
                                            std::uint16_t& distance) const
{
    // Calculate offset from center
    const int dx = point.x() - m_center.x();
    const int dy = m_center.y() - point.y();  // Invert Y for screen coordinates

    // Calculate distance from center
    const double pixelDistance = std::sqrt(static_cast<double>(dx * dx + dy * dy));

    // Check if point is within display
    if (pixelDistance > static_cast<double>(m_displayRadius)) {
        return false;
    }

    // Calculate angle using atan2 (handles all quadrants correctly)
    double radians = std::atan2(static_cast<double>(dy), static_cast<double>(dx));

    // Ensure positive angle
    if (radians < 0.0) {
        radians += 2.0 * PI;
    }

    // Convert to degrees
    angle = radiansToDegrees(radians);

    // Limit angle to sonar range (0-180 degrees)
    if (angle > 180) {
        return false;  // Outside sonar sweep range
    }

    // Scale pixel distance back to centimeters
    const double scaleFactor = static_cast<double>(m_maxDistance) / static_cast<double>(m_displayRadius);
    distance = static_cast<std::uint16_t>(pixelDistance * scaleFactor);

    return true;
}

bool PolarCoordinateConverter::isPointInDisplay(const QPoint& point) const
{
    const int dx = point.x() - m_center.x();
    const int dy = point.y() - m_center.y();
    const double distanceSquared = static_cast<double>(dx * dx + dy * dy);
    const double radiusSquared = static_cast<double>(m_displayRadius * m_displayRadius);

    return distanceSquared <= radiusSquared;
}

double PolarCoordinateConverter::degreesToRadians(std::uint16_t degrees)
{
    return static_cast<double>(degrees) * DEGREES_TO_RADIANS;
}

std::uint16_t PolarCoordinateConverter::radiansToDegrees(double radians)
{
    return static_cast<std::uint16_t>(radians * RADIANS_TO_DEGREES + 0.5);  // Round to nearest degree
}

} // namespace visualization
} // namespace siren