#ifndef SIREN_POLAR_COORDINATE_CONVERTER_H
#define SIREN_POLAR_COORDINATE_CONVERTER_H

// SIREN Sonar System
// Polar Coordinate Converter - Single Responsibility: Coordinate Transformation ONLY
// Compliant with MISRA C++ 2023, SRP, SSOT

#include <QPoint>
#include <cstdint>
#include <cmath>

namespace siren {
namespace visualization {

/**
 * @brief Polar coordinate converter - Single Responsibility: Coordinate Transformation
 *
 * This class has ONE job: Convert between polar coordinates (angle/distance)
 * and Cartesian screen coordinates for sonar display.
 * It does NOT store data, handle UI, or manage state.
 *
 * Features:
 * - Polar to screen coordinate conversion
 * - Screen to polar coordinate conversion
 * - Origin-based transformations
 * - Scale factor support
 *
 * MISRA C++ Compliance:
 * - Rule 21.2.1: No dynamic allocation
 * - Rule 5.0.1: No magic numbers
 * - Rule 8.18.2: No assignment in conditions
 */
class PolarCoordinateConverter final
{
public:
    /**
     * @brief Set display center point
     * @param center Center point for polar origin
     */
    void setCenterPoint(const QPoint& center);

    /**
     * @brief Set display radius (maximum distance scale)
     * @param radius Display radius in pixels
     */
    void setDisplayRadius(int radius);

    /**
     * @brief Set maximum distance for scaling
     * @param maxDistance Maximum distance in centimeters
     */
    void setMaxDistance(std::uint16_t maxDistance);

    /**
     * @brief Convert polar to screen coordinates
     * @param angle Angle in degrees (0-180, 0=right, 180=left)
     * @param distance Distance in centimeters
     * @return Screen coordinates
     */
    [[nodiscard]] QPoint polarToScreen(std::uint16_t angle, std::uint16_t distance) const;

    /**
     * @brief Convert screen to polar coordinates
     * @param point Screen coordinates
     * @param angle Output angle in degrees
     * @param distance Output distance in centimeters
     * @return True if conversion successful (point within display)
     */
    [[nodiscard]] bool screenToPolar(const QPoint& point,
                                    std::uint16_t& angle,
                                    std::uint16_t& distance) const;

    /**
     * @brief Check if point is within display area
     * @param point Screen coordinates
     * @return True if point is within display radius
     */
    [[nodiscard]] bool isPointInDisplay(const QPoint& point) const;

    /**
     * @brief Get current center point
     * @return Center point
     */
    [[nodiscard]] QPoint getCenterPoint() const { return m_center; }

    /**
     * @brief Get current display radius
     * @return Display radius in pixels
     */
    [[nodiscard]] int getDisplayRadius() const { return m_displayRadius; }

    /**
     * @brief Get maximum distance setting
     * @return Maximum distance in centimeters
     */
    [[nodiscard]] std::uint16_t getMaxDistance() const { return m_maxDistance; }

private:
    /**
     * @brief Convert degrees to radians
     * @param degrees Angle in degrees
     * @return Angle in radians
     */
    [[nodiscard]] static double degreesToRadians(std::uint16_t degrees);

    /**
     * @brief Convert radians to degrees
     * @param radians Angle in radians
     * @return Angle in degrees
     */
    [[nodiscard]] static std::uint16_t radiansToDegrees(double radians);

    // Display parameters
    QPoint m_center{0, 0};
    int m_displayRadius{300};
    std::uint16_t m_maxDistance{450};

    // Math constants (SSOT)
    static constexpr double PI = 3.14159265358979323846;
    static constexpr double DEGREES_TO_RADIANS = PI / 180.0;
    static constexpr double RADIANS_TO_DEGREES = 180.0 / PI;

    // Sonar display convention: 0° is right, 90° is up, 180° is left
    // This matches the servo sweep pattern (right to left)
    static constexpr double ANGLE_OFFSET = 0.0;
};

} // namespace visualization
} // namespace siren

#endif // SIREN_POLAR_COORDINATE_CONVERTER_H