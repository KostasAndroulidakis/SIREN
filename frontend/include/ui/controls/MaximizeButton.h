/**
 * CLASSIFICATION: UNCLASSIFIED
 * EXPORT CONTROL: NOT SUBJECT TO EAR/ITAR
 * CONTRACT: UNORADAR-2025
 *
 * @file MaximizeButton.h
 * @brief Maximize/restore window control button (MIL-STD-1472 compliant)
 * @author unoRadar Defense Systems
 * @date 2025
 *
 * MISRA C++ 2008 Compliant
 * DO-178C Level A Certifiable
 * Single Responsibility: Window maximize/restore functionality
 */

#ifndef UNORADAR_UI_CONTROLS_MAXIMIZEBUTTON_H
#define UNORADAR_UI_CONTROLS_MAXIMIZEBUTTON_H

#include "ui/controls/WindowControlButton.h"
#include <QWidget>
#include <QColor>

namespace unoRadar {
namespace UI {
namespace Controls {

/**
 * @class MaximizeButton
 * @brief Specialized window control button for maximize/restore functionality
 *
 * Single Responsibility:
 * - Provides maximize/restore window functionality
 * - Manages dual-state icon (maximize/restore) based on window state
 * - Inherits all military-standard styling and behavior from base class
 * - Implements keyboard shortcut (Ctrl+Shift+M) for accessibility
 *
 * MISRA C++ Compliance:
 * - Explicit constructor
 * - Virtual destructor inherited from base
 * - No additional dynamic memory allocation
 */
class MaximizeButton final : public WindowControlButton
{
    Q_OBJECT

public:
    /**
     * @brief Explicit constructor
     * @param parent Parent widget (may be nullptr)
     */
    explicit MaximizeButton(QWidget* parent = nullptr);

    /**
     * @brief Destructor - default (inherited virtual destructor)
     */
    ~MaximizeButton() = default;

    /**
     * @brief Copy constructor - deleted for MISRA compliance
     */
    MaximizeButton(const MaximizeButton&) = delete;

    /**
     * @brief Copy assignment operator - deleted for MISRA compliance
     */
    MaximizeButton& operator=(const MaximizeButton&) = delete;

    /**
     * @brief Move constructor - deleted for MISRA compliance
     */
    MaximizeButton(MaximizeButton&&) = delete;

    /**
     * @brief Move assignment operator - deleted for MISRA compliance
     */
    MaximizeButton& operator=(MaximizeButton&&) = delete;

    /**
     * @brief Update button state based on window maximized state
     * @param isMaximized Current window maximized state
     */
    void updateMaximizedState(bool isMaximized);

signals:
    /**
     * @brief Signal emitted when maximize action is requested
     */
    void maximizeRequested();

    /**
     * @brief Signal emitted when restore action is requested
     */
    void restoreRequested();

protected:
    /**
     * @brief Get the base color for the button (macOS green)
     * @return Base color for the button
     */
    QColor getBaseColor() const override;

    /**
     * @brief Get the hover color for the button (macOS green hover)
     * @return Hover color for the button
     */
    QColor getHoverColor() const override;

private slots:
    /**
     * @brief Handle button click to toggle maximize/restore
     */
    void onButtonClicked();

private:
    /**
     * @brief Initialize maximize-specific functionality
     */
    void initializeMaximizeButton();

    /**
     * @brief Set up keyboard shortcuts for accessibility
     */
    void setupKeyboardShortcuts();

    /**
     * @brief Update button icon and tooltip based on state
     */
    void updateButtonAppearance();

    // Member variables
    bool m_isMaximized; ///< Current window maximized state
};

} // namespace Controls
} // namespace UI
} // namespace unoRadar

#endif // UNORADAR_UI_CONTROLS_MAXIMIZEBUTTON_H