/**
 * CLASSIFICATION: UNCLASSIFIED
 * EXPORT CONTROL: NOT SUBJECT TO EAR/ITAR
 * CONTRACT: SIREN-2025
 *
 * @file Data.h
 * @brief Data processing and validation constants
 * @author SIREN Defense Systems
 * @date 2025
 *
 * MISRA C++ 2008 Compliant
 * DO-178C Level A Certifiable
 */

#ifndef SIREN_CONSTANTS_DATA_H
#define SIREN_CONSTANTS_DATA_H

#include <cstdint>

namespace siren {
namespace Constants {
namespace Data {

// Display Limits
constexpr std::int32_t MAX_DISPLAY_ENTRIES = 100;
constexpr std::int32_t DATA_BUFFER_SIZE = 1000;
constexpr std::int32_t MIN_DATA_POINTS = 10;

// Sonar Data Constraints (from hardware specifications)
constexpr double MIN_ANGLE_DEGREES = 0.0;
constexpr double MAX_ANGLE_DEGREES = 180.0;
constexpr double ANGLE_RESOLUTION = 1.0;

constexpr double MIN_DISTANCE_CM = 2.0;
constexpr double MAX_DISTANCE_CM = 450.0;
constexpr double DISTANCE_ACCURACY_CM = 0.3;

// Data Validation Thresholds
constexpr double OUTLIER_THRESHOLD = 3.0;  // Standard deviations
constexpr std::int32_t SMOOTHING_WINDOW_SIZE = 5;

// JSON Field Names
namespace Fields {
    constexpr char ANGLE[] = "angle";
    constexpr char DISTANCE[] = "distance";
    constexpr char TIMESTAMP[] = "timestamp";
    constexpr char QUALITY[] = "quality";
}

} // namespace Data
} // namespace Constants
} // namespace siren

#endif // SIREN_CONSTANTS_DATA_H