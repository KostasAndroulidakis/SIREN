#ifndef SIREN_VISUALIZATION_CONSTANTS_H
#define SIREN_VISUALIZATION_CONSTANTS_H

// SIREN Military-Grade Sonar System
// Visualization Constants - Single Source of Truth (SSOT)
// All sonar display related constants in one place

#include <cstdint>

namespace siren {
namespace constants {
namespace visualization {

// Display geometry constants
constexpr int MIN_WIDGET_SIZE = 400;
constexpr int DISPLAY_MARGIN = 40;
constexpr int DISPLAY_PADDING = 20;

// Grid and line widths
constexpr int GRID_LINE_WIDTH = 1;
constexpr int GRID_MAJOR_LINE_WIDTH = 2;
constexpr int SWEEP_LINE_WIDTH = 3;
constexpr int DATA_POINT_SIZE = 4;

// Text sizes
constexpr int SCALE_FONT_SIZE = 10;
constexpr int TITLE_FONT_SIZE = 14;
constexpr int LABEL_FONT_SIZE = 12;

// Hardware specifications (from datasheets)
constexpr std::uint16_t SERVO_MIN_ANGLE = 0;      // SG90 servo minimum
constexpr std::uint16_t SERVO_MAX_ANGLE = 180;    // SG90 servo maximum
constexpr std::uint16_t SENSOR_MIN_DISTANCE = 2;   // HC-SR04 minimum (cm)
constexpr std::uint16_t SENSOR_MAX_DISTANCE = 400; // HC-SR04 maximum (cm)

// Display ranges
constexpr std::uint16_t DISPLAY_MAX_DISTANCE = 450;  // Slightly larger than sensor max
constexpr std::uint16_t DISTANCE_RING_INTERVAL = 50; // Distance rings every 50cm
constexpr std::uint16_t ANGLE_MARKER_INTERVAL = 30;  // Angle markers every 30°

// Animation timing
constexpr int ANIMATION_FPS = 60;
constexpr int ANIMATION_INTERVAL_MS = 1000 / ANIMATION_FPS;
constexpr double DEFAULT_SWEEP_SPEED = 45.0;  // degrees per second

// Data point lifetime
constexpr std::uint64_t POINT_LIFETIME_MS = 5000;    // 5 seconds
constexpr std::uint64_t POINT_FADE_START_MS = 3000;  // Start fading at 3 seconds

// Colors (military green theme)
namespace colors {
    constexpr const char* BACKGROUND = "#0A0A0A";           // Very dark gray
    constexpr const char* GRID_MINOR = "#003300";           // Very dark green
    constexpr const char* GRID_MAJOR = "#006600";           // Dark green
    constexpr const char* GRID_PRIMARY = "#00AA00";         // Medium green
    constexpr const char* SWEEP_LINE = "#00FF00";           // Bright green
    constexpr const char* SWEEP_TRAIL = "#004400";          // Dark green trail
    constexpr const char* DATA_POINT = "#FFFF00";           // Yellow
    constexpr const char* DATA_POINT_RECENT = "#FF0000";    // Red for fresh data
    constexpr const char* DATA_POINT_FADING = "#888800";    // Darker yellow
    constexpr const char* TEXT_PRIMARY = "#00FF00";         // Bright green
    constexpr const char* TEXT_SECONDARY = "#00AA00";       // Medium green
    constexpr const char* SCALE_TEXT = "#AAFFAA";           // Light green
    constexpr const char* WARNING = "#FF4500";              // Orange
    constexpr const char* ERROR = "#FF0000";                // Red
}

// Font families
constexpr const char* MONOSPACE_FONT = "Courier New";
constexpr const char* SYSTEM_FONT = "Arial";

} // namespace visualization
} // namespace constants
} // namespace siren

#endif // SIREN_VISUALIZATION_CONSTANTS_H