#ifndef SIREN_LAYOUT_CONSTANTS_H
#define SIREN_LAYOUT_CONSTANTS_H

// SIREN Military-Grade Radar System
// Layout Constants - Single Source of Truth for 1920x1080 Desktop
// Compliant with MISRA C++ 2023, SSOT, MIL-STD-1472

#include <cstdint>

namespace siren {
namespace constants {
namespace layout {

/**
 * @brief Single Source of Truth for layout dimensions (1920x1080 desktop)
 * 
 * All dimensions optimized for 1920x1080 resolution desktop displays.
 * No magic numbers should appear in UI code - all dimensions defined here.
 * 
 * MISRA C++ Compliance:
 * - Rule 5.0.1: No magic numbers (all dimensions defined here)
 * - Rule 2.10.1: All constants shall be used
 */

// Target Resolution
constexpr std::uint16_t SCREEN_WIDTH = 1920;
constexpr std::uint16_t SCREEN_HEIGHT = 1080;

// Panel Dimensions (optimized for 1920x1080)
constexpr std::uint16_t STATUS_PANEL_HEIGHT = 60;       // Top status bar
constexpr std::uint16_t CONTROL_PANEL_WIDTH = 320;      // Left control panel (16.7% of width)
constexpr std::uint16_t DATA_PANEL_WIDTH = 360;         // Right data panel (18.75% of width)
constexpr std::uint16_t PERFORMANCE_PANEL_HEIGHT = 140; // Bottom performance panel

// Center radar display area (calculated)
// Width: 1920 - 320 - 360 = 1240px
// Height: 1080 - 60 - 140 = 880px

// Spacing and Margins
constexpr std::uint8_t PANEL_SPACING = 0;               // Clean military look
constexpr std::uint8_t PANEL_MARGIN = 12;               // Internal panel margins
constexpr std::uint8_t WIDGET_SPACING = 10;             // Space between widgets
constexpr std::uint8_t GROUP_SPACING = 20;              // Space between widget groups

// Border Dimensions
constexpr std::uint8_t PANEL_BORDER_WIDTH = 2;          // Visible panel borders
constexpr std::uint8_t FOCUS_BORDER_WIDTH = 3;          // Focused element border

// Control Dimensions (desktop optimized)
constexpr std::uint8_t BUTTON_HEIGHT = 40;              // Comfortable click target
constexpr std::uint8_t BUTTON_MIN_WIDTH = 100;          // Minimum button width
constexpr std::uint8_t INDICATOR_SIZE = 32;             // Status indicator size
constexpr std::uint8_t SLIDER_HEIGHT = 30;              // Slider control height

// Text Dimensions (readable on 1920x1080)
constexpr std::uint8_t LABEL_HEIGHT = 24;               // Standard label height
constexpr std::uint8_t TITLE_HEIGHT = 32;               // Section title height
constexpr std::uint8_t STATUS_TEXT_SIZE = 18;           // Status text size
constexpr std::uint8_t DATA_TEXT_SIZE = 20;             // Data display text size

// Connection Status Indicator
constexpr std::uint8_t CONNECTION_INDICATOR_SIZE = 24;   // Connection LED size
constexpr std::uint16_t CONNECTION_LABEL_WIDTH = 150;    // "Backend Connected" width

// Calibration Controls
constexpr std::uint16_t CALIBRATION_SLIDER_WIDTH = 240; // Angle adjustment slider
constexpr std::uint8_t CALIBRATION_BUTTON_SIZE = 60;     // Quick position buttons

// Data Display Fields
constexpr std::uint8_t DATA_FIELD_HEIGHT = 36;          // Data field row height
constexpr std::uint8_t DATA_LABEL_WIDTH = 120;          // "Distance:" label width
constexpr std::uint8_t DATA_VALUE_WIDTH = 100;          // "123.4 cm" value width

// Performance Metrics
constexpr std::uint8_t METRIC_VALUE_WIDTH = 80;         // "60 FPS" width
constexpr std::uint8_t METRIC_LABEL_WIDTH = 100;        // "Frame Rate:" width
constexpr std::uint8_t METRIC_ROW_HEIGHT = 32;          // Metric display row

} // namespace layout
} // namespace constants
} // namespace siren

#endif // SIREN_LAYOUT_CONSTANTS_H