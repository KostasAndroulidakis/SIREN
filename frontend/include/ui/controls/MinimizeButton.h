/**
 * CLASSIFICATION: UNCLASSIFIED
 * EXPORT CONTROL: NOT SUBJECT TO EAR/ITAR
 * CONTRACT: SIREN-2025
 *
 * @file MinimizeButton.h
 * @brief Minimize window control button (MIL-STD-1472 compliant)
 * @author SIREN Defense Systems
 * @date 2025
 *
 * MISRA C++ 2008 Compliant
 * DO-178C Level A Certifiable
 * Single Responsibility: Window minimize functionality
 */

#ifndef SIREN_UI_CONTROLS_MINIMIZEBUTTON_H
#define SIREN_UI_CONTROLS_MINIMIZEBUTTON_H

#include "ui/controls/WindowControlButton.h"
#include <QWidget>
#include <QColor>

namespace siren {
namespace UI {
namespace Controls {

/**
 * @class MinimizeButton
 * @brief Specialized window control button for minimize functionality
 *
 * Single Responsibility:
 * - Provides minimize window functionality
 * - Inherits all military-standard styling and behavior from base class
 * - Implements keyboard shortcut (Ctrl+M) for accessibility
 *
 * MISRA C++ Compliance:
 * - Explicit constructor
 * - Virtual destructor inherited from base
 * - No additional dynamic memory allocation
 */
class MinimizeButton final : public WindowControlButton
{
    Q_OBJECT

public:
    /**
     * @brief Explicit constructor
     * @param parent Parent widget (may be nullptr)
     */
    explicit MinimizeButton(QWidget* parent = nullptr);

    /**
     * @brief Destructor - default (inherited virtual destructor)
     */
    ~MinimizeButton() = default;

    /**
     * @brief Copy constructor - deleted for MISRA compliance
     */
    MinimizeButton(const MinimizeButton&) = delete;

    /**
     * @brief Copy assignment operator - deleted for MISRA compliance
     */
    MinimizeButton& operator=(const MinimizeButton&) = delete;

    /**
     * @brief Move constructor - deleted for MISRA compliance
     */
    MinimizeButton(MinimizeButton&&) = delete;

    /**
     * @brief Move assignment operator - deleted for MISRA compliance
     */
    MinimizeButton& operator=(MinimizeButton&&) = delete;

signals:
    /**
     * @brief Signal emitted when minimize action is requested
     */
    void minimizeRequested();

protected:
    /**
     * @brief Get the base color for the button (macOS yellow)
     * @return Base color for the button
     */
    QColor getBaseColor() const override;

    /**
     * @brief Get the hover color for the button (macOS yellow hover)
     * @return Hover color for the button
     */
    QColor getHoverColor() const override;

private slots:
    /**
     * @brief Handle button click to minimize window
     */
    void onButtonClicked();

private:
    /**
     * @brief Initialize minimize-specific functionality
     */
    void initializeMinimizeButton();

    /**
     * @brief Set up keyboard shortcuts for accessibility
     */
    void setupKeyboardShortcuts();
};

} // namespace Controls
} // namespace UI
} // namespace siren

#endif // SIREN_UI_CONTROLS_MINIMIZEBUTTON_H