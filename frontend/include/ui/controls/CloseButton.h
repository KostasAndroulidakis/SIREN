/**
 * CLASSIFICATION: UNCLASSIFIED
 * EXPORT CONTROL: NOT SUBJECT TO EAR/ITAR
 * CONTRACT: UNORADAR-2025
 *
 * @file CloseButton.h
 * @brief Close window control button (MIL-STD-1472 compliant)
 * @author unoRadar Defense Systems
 * @date 2025
 *
 * MISRA C++ 2008 Compliant
 * DO-178C Level A Certifiable
 * Single Responsibility: Window close functionality
 */

#ifndef UNORADAR_UI_CONTROLS_CLOSEBUTTON_H
#define UNORADAR_UI_CONTROLS_CLOSEBUTTON_H

#include "ui/controls/WindowControlButton.h"
#include <QWidget>
#include <QColor>

namespace unoRadar {
namespace UI {
namespace Controls {

/**
 * @class CloseButton
 * @brief Specialized window control button for close functionality
 *
 * Single Responsibility:
 * - Provides close window functionality
 * - Implements critical action styling (red hover state)
 * - Inherits all military-standard styling and behavior from base class
 * - Implements keyboard shortcut (Ctrl+Q) for accessibility
 *
 * MISRA C++ Compliance:
 * - Explicit constructor
 * - Virtual destructor inherited from base
 * - No additional dynamic memory allocation
 */
class CloseButton final : public WindowControlButton
{
    Q_OBJECT

public:
    /**
     * @brief Explicit constructor
     * @param parent Parent widget (may be nullptr)
     */
    explicit CloseButton(QWidget* parent = nullptr);

    /**
     * @brief Destructor - default (inherited virtual destructor)
     */
    ~CloseButton() = default;

    /**
     * @brief Copy constructor - deleted for MISRA compliance
     */
    CloseButton(const CloseButton&) = delete;

    /**
     * @brief Copy assignment operator - deleted for MISRA compliance
     */
    CloseButton& operator=(const CloseButton&) = delete;

    /**
     * @brief Move constructor - deleted for MISRA compliance
     */
    CloseButton(CloseButton&&) = delete;

    /**
     * @brief Move assignment operator - deleted for MISRA compliance
     */
    CloseButton& operator=(CloseButton&&) = delete;

signals:
    /**
     * @brief Signal emitted when close action is requested
     */
    void closeRequested();

protected:
    /**
     * @brief Get the base color for the button (macOS red)
     * @return Base color for the button
     */
    QColor getBaseColor() const override;

    /**
     * @brief Get the hover color for the button (macOS red hover)
     * @return Hover color for the button
     */
    QColor getHoverColor() const override;

private slots:
    /**
     * @brief Handle button click to close window
     */
    void onButtonClicked();

private:
    /**
     * @brief Initialize close-specific functionality
     */
    void initializeCloseButton();

    /**
     * @brief Set up keyboard shortcuts for accessibility
     */
    void setupKeyboardShortcuts();

};

} // namespace Controls
} // namespace UI
} // namespace unoRadar

#endif // UNORADAR_UI_CONTROLS_CLOSEBUTTON_H