/**
 * CLASSIFICATION: UNCLASSIFIED
 * EXPORT CONTROL: NOT SUBJECT TO EAR/ITAR
 * CONTRACT: UNORADAR-2025
 *
 * @file WindowControls.h
 * @brief Window control constants (MIL-STD-1472 compliant)
 * @author unoRadar Defense Systems
 * @date 2025
 *
 * MISRA C++ 2008 Compliant
 * DO-178C Level A Certifiable
 * Single Source of Truth (SSOT) for window control specifications
 */

#ifndef UNORADAR_CONSTANTS_WINDOWCONTROLS_H
#define UNORADAR_CONSTANTS_WINDOWCONTROLS_H

#include <cstdint>

namespace unoRadar {
namespace Constants {
namespace WindowControls {

// Button Dimensions (MIL-STD-1472 minimum target size compliance)
constexpr std::int32_t BUTTON_WIDTH = 32;
constexpr std::int32_t BUTTON_HEIGHT = 32;
constexpr std::int32_t BUTTON_MIN_SIZE = 16;  // MIL-STD-1472 minimum
constexpr std::int32_t BUTTON_SPACING = 4;

// Control Bar Dimensions
constexpr std::int32_t CONTROL_BAR_HEIGHT = 40;
constexpr std::int32_t CONTROL_BAR_PADDING = 4;
constexpr std::int32_t CONTROL_BAR_MARGIN = 8;

// Button States and Visual Feedback
constexpr std::int32_t HOVER_ANIMATION_DURATION = 100;  // milliseconds
constexpr std::int32_t PRESS_ANIMATION_DURATION = 50;   // milliseconds
constexpr std::int32_t BORDER_WIDTH = 1;

// MIL-STD-1472 Color Specifications
namespace Colors {
    // Normal State (high contrast for military environments)
    constexpr char BUTTON_BACKGROUND[] = "#1A1A1A";      // Dark gray
    constexpr char BUTTON_BORDER[] = "#404040";          // Medium gray
    constexpr char BUTTON_TEXT[] = "#CCCCCC";            // Light gray

    // Hover State (enhanced visibility)
    constexpr char BUTTON_HOVER_BACKGROUND[] = "#2D2D2D"; // Lighter gray
    constexpr char BUTTON_HOVER_BORDER[] = "#606060";     // Brighter border
    constexpr char BUTTON_HOVER_TEXT[] = "#FFFFFF";       // White text

    // Pressed State (immediate visual feedback)
    constexpr char BUTTON_PRESSED_BACKGROUND[] = "#404040"; // Medium gray
    constexpr char BUTTON_PRESSED_BORDER[] = "#808080";     // Bright border
    constexpr char BUTTON_PRESSED_TEXT[] = "#FFFFFF";       // White text

    // Focus State (keyboard navigation)
    constexpr char BUTTON_FOCUS_BORDER[] = "#00FF41";     // Military green
    constexpr char BUTTON_FOCUS_GLOW[] = "#00FF4140";     // Translucent green

    // Disabled State (system unavailable)
    constexpr char BUTTON_DISABLED_BACKGROUND[] = "#0F0F0F"; // Very dark
    constexpr char BUTTON_DISABLED_BORDER[] = "#202020";     // Dark border
    constexpr char BUTTON_DISABLED_TEXT[] = "#404040";       // Dim text

    // Close Button Special Colors (critical action)
    constexpr char CLOSE_BUTTON_HOVER[] = "#FF4444";      // Red hover
    constexpr char CLOSE_BUTTON_PRESSED[] = "#FF6666";    // Light red pressed
}

// Accessibility and Navigation
constexpr std::int32_t TAB_ORDER_MINIMIZE = 1;
constexpr std::int32_t TAB_ORDER_MAXIMIZE = 2;
constexpr std::int32_t TAB_ORDER_CLOSE = 3;

// Icon Specifications (Unicode symbols for consistency)
namespace Icons {
    constexpr char MINIMIZE[] = "−";     // Unicode minus
    constexpr char MAXIMIZE[] = "□";     // Unicode square
    constexpr char RESTORE[] = "❐";      // Unicode overlapping squares
    constexpr char CLOSE[] = "×";        // Unicode multiplication sign
}

// Font Specifications
namespace Fonts {
    constexpr char FONT_FAMILY[] = "Arial";
    constexpr std::int32_t FONT_SIZE = 14;
    constexpr std::int32_t FONT_WEIGHT = 75;  // Bold
}

// Positioning Constants
constexpr std::int32_t RIGHT_MARGIN = 8;
constexpr std::int32_t TOP_MARGIN = 4;

} // namespace WindowControls
} // namespace Constants
} // namespace unoRadar

#endif // UNORADAR_CONSTANTS_WINDOWCONTROLS_H