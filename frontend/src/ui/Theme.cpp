// SIREN Theme Implementation
// Single Responsibility: Apply Styling ONLY

#include "ui/Theme.h"

namespace siren {
namespace ui {

void Theme::applyToApplication(QWidget* app)
{
    if (app != nullptr) {
        app->setStyleSheet(getStyleSheet());
    }
}

void Theme::applyStatusPanelStyle(QWidget* panel)
{
    if (panel != nullptr) {
        const QString style = QString(R"(
            #statusPanel {
                background-color: %1;
                border-bottom: 2px solid %2;
                color: %3;
                font-family: %4;
            }
        )").arg(SECONDARY_BACKGROUND, BORDER_ACTIVE, PRIMARY_TEXT, MONOSPACE_FONT);

        panel->setStyleSheet(style);
    }
}

void Theme::applyControlPanelStyle(QWidget* panel)
{
    if (panel != nullptr) {
        const QString style = QString(R"(
            #controlPanel {
                background-color: %1;
                border-right: 2px solid %2;
                color: %3;
                font-family: %4;
            }
        )").arg(PANEL_BACKGROUND, BORDER_INACTIVE, PRIMARY_TEXT, MONOSPACE_FONT);

        panel->setStyleSheet(style);
    }
}

void Theme::applySonarPanelStyle(QWidget* panel)
{
    if (panel != nullptr) {
        const QString style = QString(R"(
            #sonarPanel {
                background-color: %1;
                border: 2px solid %2;
                color: %3;
            }
        )").arg(PRIMARY_BACKGROUND, BORDER_ACTIVE, PRIMARY_TEXT);

        panel->setStyleSheet(style);
    }
}

void Theme::applyDataPanelStyle(QWidget* panel)
{
    if (panel != nullptr) {
        const QString style = QString(R"(
            #dataPanel {
                background-color: %1;
                border-left: 2px solid %2;
                color: %3;
                font-family: %4;
            }
        )").arg(PANEL_BACKGROUND, BORDER_INACTIVE, PRIMARY_TEXT, MONOSPACE_FONT);

        panel->setStyleSheet(style);
    }
}

void Theme::applyPerformancePanelStyle(QWidget* panel)
{
    if (panel != nullptr) {
        const QString style = QString(R"(
            #performancePanel {
                background-color: %1;
                border-top: 2px solid %2;
                color: %3;
                font-family: %4;
            }
        )").arg(SECONDARY_BACKGROUND, BORDER_ACTIVE, PRIMARY_TEXT, MONOSPACE_FONT);

        panel->setStyleSheet(style);
    }
}

void Theme::applyConnectionIndicatorStyle(QWidget* widget)
{
    if (widget != nullptr) {
        const QString style = QString(R"(
            .connectionIndicator {
                background-color: %1;
                border: 2px solid %2;
                border-radius: 12px;
                color: %3;
                font-family: %4;
                font-size: 14px;
                padding: 5px 10px;
            }
            .connectionIndicator[connected="true"] {
                background-color: %5;
                border-color: %5;
                color: %1;
            }
            .connectionIndicator[connected="false"] {
                background-color: %6;
                border-color: %6;
                color: %3;
            }
        )").arg(SECONDARY_BACKGROUND, BORDER_INACTIVE, PRIMARY_TEXT, MONOSPACE_FONT,
                SUCCESS_COLOR, DANGER_COLOR);

        widget->setStyleSheet(style);
    }
}

void Theme::applyPrimaryButtonStyle(QWidget* button)
{
    if (button != nullptr) {
        const QString style = QString(R"(
            QPushButton {
                background-color: %1;
                border: 2px solid %2;
                color: %1;
                font-family: %4;
                font-weight: bold;
                font-size: 14px;
                padding: 8px 16px;
                min-height: 24px;
            }
            QPushButton:hover {
                background-color: %3;
                border-color: %3;
            }
            QPushButton:pressed {
                background-color: %2;
            }
            QPushButton:disabled {
                background-color: %5;
                border-color: %6;
                color: %6;
            }
        )").arg(PRIMARY_BACKGROUND, PRIMARY_ACCENT, BORDER_HOVER, MONOSPACE_FONT,
                SECONDARY_BACKGROUND, DISABLED_TEXT);

        button->setStyleSheet(style);
    }
}

void Theme::applySecondaryButtonStyle(QWidget* button)
{
    if (button != nullptr) {
        const QString style = QString(R"(
            QPushButton {
                background-color: %1;
                border: 2px solid %2;
                color: %2;
                font-family: %4;
                font-weight: bold;
                font-size: 14px;
                padding: 8px 16px;
                min-height: 24px;
            }
            QPushButton:hover {
                background-color: %2;
                color: %1;
            }
            QPushButton:pressed {
                background-color: %3;
            }
        )").arg(PRIMARY_BACKGROUND, SECONDARY_ACCENT, SECONDARY_PRESSED, MONOSPACE_FONT);

        button->setStyleSheet(style);
    }
}

void Theme::applyDangerButtonStyle(QWidget* button)
{
    if (button != nullptr) {
        const QString style = QString(R"(
            QPushButton {
                background-color: %1;
                border: 2px solid %2;
                color: %2;
                font-family: %4;
                font-weight: bold;
                font-size: 14px;
                padding: 8px 16px;
                min-height: 24px;
            }
            QPushButton:hover {
                background-color: %2;
                color: %1;
            }
            QPushButton:pressed {
                background-color: %3;
            }
        )").arg(PRIMARY_BACKGROUND, DANGER_COLOR, DANGER_PRESSED, MONOSPACE_FONT);

        button->setStyleSheet(style);
    }
}

QString Theme::getStyleSheet()
{
    return QString(R"(
        /* Global Application Style */
        QWidget {
            background-color: %1;
            color: %2;
            font-family: %3;
            font-size: 13px;
        }

        /* Placeholder Labels */
        #placeholderLabel {
            color: %4;
            font-size: 18px;
            font-weight: bold;
            border: 2px dashed %5;
            padding: 20px;
            background-color: %6;
        }

        /* Frame Styles */
        QFrame {
            border: 1px solid %5;
        }

        /* Label Styles */
        QLabel {
            color: %2;
            background-color: transparent;
        }

        /* Input Styles */
        QLineEdit, QTextEdit, QSpinBox, QDoubleSpinBox {
            background-color: %6;
            border: 2px solid %5;
            color: %2;
            padding: 5px;
            font-family: %3;
        }

        QLineEdit:focus, QTextEdit:focus, QSpinBox:focus, QDoubleSpinBox:focus {
            border-color: %4;
        }

        /* Slider Styles */
        QSlider::groove:horizontal {
            background: %5;
            height: 8px;
            border-radius: 4px;
        }

        QSlider::handle:horizontal {
            background: %4;
            border: 2px solid %4;
            width: 18px;
            margin: -5px 0;
            border-radius: 9px;
        }

        QSlider::sub-page:horizontal {
            background: %4;
            border-radius: 4px;
        }

        /* Progress Bar Styles */
        QProgressBar {
            background-color: %5;
            border: 2px solid %4;
            border-radius: 4px;
            text-align: center;
            color: %2;
            font-family: %3;
        }

        QProgressBar::chunk {
            background-color: %4;
            border-radius: 2px;
        }

        /* Scrollbar Styles */
        QScrollBar:vertical {
            background: %6;
            width: 16px;
            border: 1px solid %5;
        }

        QScrollBar::handle:vertical {
            background: %4;
            border: 1px solid %4;
            border-radius: 3px;
        }

        QScrollBar::handle:vertical:hover {
            background: %7;
        }

    )").arg(PRIMARY_BACKGROUND,     // %1 - Main background
            PRIMARY_TEXT,           // %2 - Main text
            MONOSPACE_FONT,          // %3 - Font family
            PRIMARY_ACCENT,         // %4 - Cyan accent
            BORDER_INACTIVE,        // %5 - Dark borders
            SECONDARY_BACKGROUND,   // %6 - Panel background
            BORDER_HOVER);          // %7 - Hover color
}

// Color accessors for QPalette setup (SSOT compliance)
QColor Theme::getPrimaryBackground()
{
    return QColor(PRIMARY_BACKGROUND);
}

QColor Theme::getMilitaryGreen()
{
    return QColor(MILITARY_GREEN);
}

QColor Theme::getAlternateRow()
{
    return QColor(ALTERNATE_ROW);
}

QColor Theme::getPrimaryText()
{
    return QColor(PRIMARY_TEXT);
}

} // namespace ui
} // namespace siren