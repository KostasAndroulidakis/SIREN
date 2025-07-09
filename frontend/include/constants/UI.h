/**
 * CLASSIFICATION: UNCLASSIFIED
 * EXPORT CONTROL: NOT SUBJECT TO EAR/ITAR
 * CONTRACT: UNORADAR-2025
 *
 * @file UI.h
 * @brief User Interface constants (MIL-STD-1472 compliant)
 * @author unoRadar Defense Systems
 * @date 2025
 *
 * MISRA C++ 2008 Compliant
 * DO-178C Level A Certifiable
 */

#ifndef UNORADAR_CONSTANTS_UI_H
#define UNORADAR_CONSTANTS_UI_H

#include <cstdint>

namespace unoRadar {
namespace Constants {
namespace UI {

// Window Dimensions
constexpr std::int32_t WINDOW_WIDTH = 800;
constexpr std::int32_t WINDOW_HEIGHT = 600;
constexpr std::int32_t MIN_WINDOW_WIDTH = 640;
constexpr std::int32_t MIN_WINDOW_HEIGHT = 480;

// Military-grade color scheme (MIL-STD-1472 contrast ratios)
namespace Colors {
    constexpr char BACKGROUND[] = "#000000";      // True black
    constexpr char PRIMARY_TEXT[] = "#00FF41";    // Military green
    constexpr char WARNING_TEXT[] = "#FFA500";    // Amber
    constexpr char ERROR_TEXT[] = "#FF0000";      // Red
    constexpr char GRID_LINES[] = "#1F1F1F";      // Dark gray
    constexpr char RADAR_SWEEP[] = "#00FF41";     // Military green
    constexpr char RADAR_TARGET[] = "#FFFFFF";    // White
}

// Font Configuration
namespace Fonts {
    constexpr char MONOSPACE[] = "Consolas";
    constexpr char FALLBACK[] = "Courier New";
    constexpr std::int32_t DEFAULT_SIZE = 10;
    constexpr std::int32_t MIN_SIZE = 8;
    constexpr std::int32_t MAX_SIZE = 24;
}

// Layout Constants
constexpr std::int32_t MARGIN = 10;
constexpr std::int32_t SPACING = 5;
constexpr std::int32_t STATUS_BAR_HEIGHT = 25;

// Title Bar Constants
constexpr std::int32_t TITLE_FONT_SIZE = 14;
constexpr std::int32_t TITLE_FONT_WEIGHT = 700;  // Bold for military styling

} // namespace UI
} // namespace Constants
} // namespace unoRadar

#endif // UNORADAR_CONSTANTS_UI_H