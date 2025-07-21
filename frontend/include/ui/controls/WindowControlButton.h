/**
 * CLASSIFICATION: UNCLASSIFIED
 * EXPORT CONTROL: NOT SUBJECT TO EAR/ITAR
 * CONTRACT: SIREN-2025
 *
 * @file WindowControlButton.h
 * @brief Base class for window control buttons (MIL-STD-1472 compliant)
 * @author KostasAndroulidakis
 * @date 2025
 *
 * MISRA C++ 2008 Compliant
 * DO-178C Level A Certifiable
 * Single Responsibility: Base functionality for window control buttons
 */

#ifndef SIREN_UI_CONTROLS_WINDOWCONTROLBUTTON_H
#define SIREN_UI_CONTROLS_WINDOWCONTROLBUTTON_H

#include <QPushButton>
#include <QWidget>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QFocusEvent>
#include <QPaintEvent>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include <QPainter>
#include <QRadialGradient>
#include <QLinearGradient>
#include <memory>

namespace siren {
namespace UI {
namespace Controls {

/**
 * @class WindowControlButton
 * @brief Base class providing common functionality for window control buttons
 *
 * Responsibilities:
 * - Provides military-standard visual states (normal, hover, pressed, focus)
 * - Implements MIL-STD-1472 compliant sizing and accessibility
 * - Handles keyboard navigation and focus management
 * - Provides smooth animations for state transitions
 *
 * MISRA C++ Compliance:
 * - Virtual destructor for proper inheritance
 * - Protected inheritance interface
 * - No dynamic memory allocation after initialization
 * - Explicit constructors
 */
class WindowControlButton : public QPushButton
{
    Q_OBJECT

public:
    /**
     * @brief Explicit constructor
     * @param icon The button icon text (Unicode character)
     * @param tooltip The accessibility tooltip text
     * @param parent Parent widget (may be nullptr)
     */
    explicit WindowControlButton(const QString& icon,
                                const QString& tooltip,
                                QWidget* parent = nullptr);

    /**
     * @brief Virtual destructor for proper inheritance
     */
    virtual ~WindowControlButton() = default;

    /**
     * @brief Copy constructor - deleted for MISRA compliance
     */
    WindowControlButton(const WindowControlButton&) = delete;

    /**
     * @brief Copy assignment operator - deleted for MISRA compliance
     */
    WindowControlButton& operator=(const WindowControlButton&) = delete;

    /**
     * @brief Move constructor - deleted for MISRA compliance
     */
    WindowControlButton(WindowControlButton&&) = delete;

    /**
     * @brief Move assignment operator - deleted for MISRA compliance
     */
    WindowControlButton& operator=(WindowControlButton&&) = delete;

    /**
     * @brief Enable/disable the button
     * @param enabled Button state
     */
    void setEnabled(bool enabled);

    /**
     * @brief Get current button state
     * @return true if button is enabled
     */
    bool isEnabled() const;

protected:
    /**
     * @brief Handle mouse enter events for hover state
     * @param event Mouse event details
     */
    void enterEvent(QEnterEvent* event) override;

    /**
     * @brief Handle mouse leave events to exit hover state
     * @param event Mouse event details
     */
    void leaveEvent(QEvent* event) override;

    /**
     * @brief Handle mouse press events for pressed state
     * @param event Mouse event details
     */
    void mousePressEvent(QMouseEvent* event) override;

    /**
     * @brief Handle mouse release events to exit pressed state
     * @param event Mouse event details
     */
    void mouseReleaseEvent(QMouseEvent* event) override;

    /**
     * @brief Handle focus in events for keyboard navigation
     * @param event Focus event details
     */
    void focusInEvent(QFocusEvent* event) override;

    /**
     * @brief Handle focus out events to clear focus state
     * @param event Focus event details
     */
    void focusOutEvent(QFocusEvent* event) override;

    /**
     * @brief Handle key press events for keyboard activation
     * @param event Key event details
     */
    void keyPressEvent(QKeyEvent* event) override;

    /**
     * @brief Handle key release events for keyboard activation
     * @param event Key event details
     */
    void keyReleaseEvent(QKeyEvent* event) override;

    /**
     * @brief Custom paint event for military-standard styling
     * @param event Paint event details
     */
    void paintEvent(QPaintEvent* event) override;

private:
    /**
     * @brief Initialize button appearance and behavior
     */
    void initializeButton();

    /**
     * @brief Apply military-standard styling
     */
    void applyMilitaryStyle();

    /**
     * @brief Update button visual state
     * @param state New visual state
     */
    void updateButtonState(const QString& state);

    /**
     * @brief Create smooth state transition animation
     * @param targetOpacity Target opacity for transition
     */
    void createStateTransition(double targetOpacity);

    /**
     * @brief Get the base color for the button (implemented by subclasses)
     * @return Base color for the button
     */
    virtual QColor getBaseColor() const = 0;

    /**
     * @brief Get the hover color for the button (implemented by subclasses)
     * @return Hover color for the button
     */
    virtual QColor getHoverColor() const = 0;

    /**
     * @brief Draw circular button background with gradient
     * @param painter QPainter instance
     * @param rect Button rectangle
     */
    void drawCircularButton(QPainter& painter, const QRect& rect);

    /**
     * @brief Draw button symbol when hovered
     * @param painter QPainter instance
     * @param rect Button rectangle
     */
    void drawButtonSymbol(QPainter& painter, const QRect& rect);

    /**
     * @brief Draw focus indicator for keyboard navigation
     * @param painter QPainter instance
     * @param rect Button rectangle
     */
    void drawFocusIndicator(QPainter& painter, const QRect& rect);

    // Member variables
    QString m_icon;                                           ///< Button icon text
    QString m_tooltip;                                        ///< Accessibility tooltip
    QString m_currentState;                                   ///< Current visual state
    bool m_isHovered;                                         ///< Hover state flag
    bool m_isPressed;                                         ///< Pressed state flag
    bool m_isFocused;                                         ///< Focus state flag
    std::unique_ptr<QPropertyAnimation> m_stateAnimation;     ///< State transition animation
    std::unique_ptr<QGraphicsOpacityEffect> m_opacityEffect; ///< Opacity effect for animations
};

} // namespace Controls
} // namespace UI
} // namespace siren

#endif // SIREN_UI_CONTROLS_WINDOWCONTROLBUTTON_H