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

// macOS Traffic Light Dimensions (Exact Native Specifications)
constexpr std::int32_t BUTTON_WIDTH = 12;           // 12px diameter circles
constexpr std::int32_t BUTTON_HEIGHT = 12;          // 12px diameter circles
constexpr std::int32_t BUTTON_MIN_SIZE = 12;        // Minimum size for clickability
constexpr std::int32_t BUTTON_SPACING = 8;          // 8px spacing between buttons
constexpr std::int32_t BUTTONS_LEFT_MARGIN = 20;    // 20px from left edge

// Control Bar Dimensions (Adjusted for macOS styling)
constexpr std::int32_t CONTROL_BAR_HEIGHT = 32;     // Compact height for traffic lights
constexpr std::int32_t CONTROL_BAR_PADDING = 6;     // Vertical centering
constexpr std::int32_t CONTROL_BAR_MARGIN = 8;

// Button States and Visual Feedback (macOS-style transitions)
constexpr std::int32_t HOVER_ANIMATION_DURATION = 150;  // milliseconds (smooth transitions)
constexpr std::int32_t PRESS_ANIMATION_DURATION = 50;   // milliseconds
constexpr std::int32_t BORDER_WIDTH = 0;                // No border for circular buttons

// macOS Traffic Light Colors (Exact Native Specifications)
namespace Colors {
    // macOS Traffic Light Base Colors
    constexpr char MACOS_RED[] = "#FF5F57";               // Exact macOS red
    constexpr char MACOS_YELLOW[] = "#FFBD2E";            // Exact macOS yellow
    constexpr char MACOS_GREEN[] = "#28CA42";             // Exact macOS green

    // macOS Traffic Light Hover Colors (slightly darker)
    constexpr char MACOS_RED_HOVER[] = "#E04A42";         // Darker red on hover
    constexpr char MACOS_YELLOW_HOVER[] = "#E5A629";      // Darker yellow on hover
    constexpr char MACOS_GREEN_HOVER[] = "#23B13A";       // Darker green on hover

    // macOS Traffic Light Symbol Colors (dark symbols on hover)
    constexpr char MACOS_SYMBOL_COLOR[] = "#4D0000";      // Dark color for symbols
    constexpr char MACOS_SYMBOL_HOVER[] = "#1A0000";      // Darker for pressed state

    // Gradient Colors for 3D Effect
    constexpr char MACOS_GRADIENT_TOP[] = "#FFFFFF40";    // Light highlight
    constexpr char MACOS_GRADIENT_BOTTOM[] = "#00000020"; // Subtle shadow

    // Focus State (keyboard navigation - military compliance)
    constexpr char BUTTON_FOCUS_BORDER[] = "#00FF41";     // Military green
    constexpr char BUTTON_FOCUS_GLOW[] = "#00FF4140";     // Translucent green

    // Disabled State (system unavailable)
    constexpr char BUTTON_DISABLED_BACKGROUND[] = "#3A3A3A"; // Muted gray
    constexpr char BUTTON_DISABLED_BORDER[] = "#505050";     // Medium gray border
    constexpr char BUTTON_DISABLED_TEXT[] = "#808080";       // Dim text
}

// Accessibility and Navigation
constexpr std::int32_t TAB_ORDER_MINIMIZE = 1;
constexpr std::int32_t TAB_ORDER_MAXIMIZE = 2;
constexpr std::int32_t TAB_ORDER_CLOSE = 3;

// macOS Traffic Light Icon Specifications (Exact Native Symbols)
namespace Icons {
    constexpr char MINIMIZE[] = "−";     // Unicode minus (U+2212)
    constexpr char MAXIMIZE[] = "⤢";     // Unicode northeast arrow (U+2922)
    constexpr char RESTORE[] = "⤡";      // Unicode southwest arrow (U+2921)
    constexpr char CLOSE[] = "×";        // Unicode multiplication sign (U+00D7)
}

// macOS Traffic Light Font Specifications
namespace Fonts {
    constexpr char FONT_FAMILY[] = "SF Pro Display";     // macOS system font
    constexpr std::int32_t FONT_SIZE = 10;               // Smaller font for 12px buttons
    constexpr std::int32_t FONT_WEIGHT = 50;             // Normal weight
}

// Positioning Constants
constexpr std::int32_t RIGHT_MARGIN = 8;
constexpr std::int32_t TOP_MARGIN = 4;

} // namespace WindowControls
} // namespace Constants
} // namespace unoRadar

#endif // UNORADAR_CONSTANTS_WINDOWCONTROLS_H