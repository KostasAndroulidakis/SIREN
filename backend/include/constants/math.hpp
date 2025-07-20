/**
 * @file math.hpp
 * @brief Mathematical constants and formulas for military-grade radar calculations
 * @author SIREN Defense Systems
 * @date 2025
 * @classification UNCLASSIFIED
 *
 * Mathematical constants, coordinate transformations, and precision specifications.
 * Supports radar coordinate system calculations and unit conversions.
 *
 * MISRA C++ Compliance: All constants are constexpr and strongly typed
 * SRP: Single responsibility - mathematical constants and formulas only
 */

#pragma once

#include <cstdint>
#ifdef __cpp_lib_math_constants
#include <numbers>
#endif

namespace siren { namespace constants { namespace math {

/// Fundamental mathematical constants
namespace fundamental {
    // Use C++20 std::numbers::pi or fallback to high-precision value
    #ifdef __cpp_lib_math_constants
        constexpr double PI = std::numbers::pi;
    #else
        constexpr double PI = 3.14159265358979323846;
    #endif

    /// Degrees to radians conversion factor
    constexpr double DEG_TO_RAD = PI / 180.0;

    /// Radians to degrees conversion factor
    constexpr double RAD_TO_DEG = 180.0 / PI;
}

/// Radar coordinate system configuration
namespace coordinates {
    /// Radar coordinate system origin (center point)
    constexpr double RADAR_CENTER_X = 0.0;
    constexpr double RADAR_CENTER_Y = 0.0;

    /// Distance measurement precision (1mm resolution)
    constexpr double DISTANCE_PRECISION = 0.1;
}

/// Test and demonstration values
namespace test {
    /// Test radar data point values for system validation
    constexpr int16_t TEST_ANGLE_DEGREES = 90;
    constexpr int16_t TEST_DISTANCE_CM = 150;
}

} } } // namespace siren::constants::math