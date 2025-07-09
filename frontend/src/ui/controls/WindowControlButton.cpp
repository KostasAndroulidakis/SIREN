/**
 * CLASSIFICATION: UNCLASSIFIED
 * EXPORT CONTROL: NOT SUBJECT TO EAR/ITAR
 * CONTRACT: UNORADAR-2025
 *
 * @file WindowControlButton.cpp
 * @brief Implementation of base window control button class
 * @author unoRadar Defense Systems
 * @date 2025
 *
 * MISRA C++ 2008 Compliant
 * DO-178C Level A Certifiable
 */

#include "ui/controls/WindowControlButton.h"
#include "constants/WindowControls.h"
#include <QPainter>
#include <QFont>
#include <QFontMetrics>
#include <QPalette>
#include <QApplication>
#include <QKeySequence>

namespace unoRadar {
namespace UI {
namespace Controls {

WindowControlButton::WindowControlButton(const QString& icon,
                                         const QString& tooltip,
                                         QWidget* parent)
    : QPushButton(parent)
    , m_icon(icon)
    , m_tooltip(tooltip)
    , m_isHovered(false)
    , m_isPressed(false)
    , m_isFocused(false)
    , m_stateAnimation(std::make_unique<QPropertyAnimation>(this))
    , m_opacityEffect(std::make_unique<QGraphicsOpacityEffect>(this))
{
    initializeButton();
}

void WindowControlButton::initializeButton()
{
    // Set fixed size for macOS traffic lights (12px diameter)
    setFixedSize(Constants::WindowControls::BUTTON_WIDTH,
                 Constants::WindowControls::BUTTON_HEIGHT);

    // Set minimum size for accessibility
    setMinimumSize(Constants::WindowControls::BUTTON_MIN_SIZE,
                   Constants::WindowControls::BUTTON_MIN_SIZE);

    // Configure text (no tooltip for clean military UI)
    setText(m_icon);

    // Enable focus for keyboard navigation
    setFocusPolicy(Qt::StrongFocus);

    // Set up opacity effect for animations
    setGraphicsEffect(m_opacityEffect.get());
    m_opacityEffect->setOpacity(1.0);

    // Configure state animation
    m_stateAnimation->setTargetObject(m_opacityEffect.get());
    m_stateAnimation->setPropertyName("opacity");
    m_stateAnimation->setDuration(Constants::WindowControls::HOVER_ANIMATION_DURATION);

    // Remove default button styling for custom circular rendering
    setFlat(true);
    setStyleSheet("QPushButton { border: none; background: transparent; }");

    // Apply macOS traffic light styling
    applyMilitaryStyle();
}

void WindowControlButton::applyMilitaryStyle()
{
    // Create macOS-style font for traffic light symbols
    QFont buttonFont(Constants::WindowControls::Fonts::FONT_FAMILY,
                     Constants::WindowControls::Fonts::FONT_SIZE,
                     Constants::WindowControls::Fonts::FONT_WEIGHT);
    setFont(buttonFont);

    // Initialize button state
    updateButtonState("normal");
}

void WindowControlButton::updateButtonState(const QString& state)
{
    // Store current state for custom painting
    m_currentState = state;

    // Force repaint to reflect state changes
    update();
}

void WindowControlButton::drawCircularButton(QPainter& painter, const QRect& rect)
{
    painter.save();

    // Choose color based on current state
    QColor baseColor = getBaseColor();
    QColor currentColor = baseColor;

    if (m_isPressed) {
        currentColor = currentColor.darker(110);
    } else if (m_isHovered) {
        currentColor = getHoverColor();
    }

    // Create radial gradient for 3D effect
    QRadialGradient gradient(rect.center(), rect.width() / 2.0);
    gradient.setColorAt(0.0, currentColor.lighter(115));
    gradient.setColorAt(0.7, currentColor);
    gradient.setColorAt(1.0, currentColor.darker(115));

    // Draw circular button
    painter.setBrush(QBrush(gradient));
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(rect);

    // Add subtle highlight for glass effect
    QRadialGradient highlight(rect.center(), rect.width() / 3.0);
    highlight.setColorAt(0.0, QColor(255, 255, 255, 30));
    highlight.setColorAt(1.0, QColor(255, 255, 255, 0));

    painter.setBrush(QBrush(highlight));
    painter.drawEllipse(rect.adjusted(1, 1, -1, -1));

    painter.restore();
}

void WindowControlButton::drawButtonSymbol(QPainter& painter, const QRect& rect)
{
    painter.save();

    // Set up font for symbol
    QFont symbolFont(Constants::WindowControls::Fonts::FONT_FAMILY,
                     Constants::WindowControls::Fonts::FONT_SIZE,
                     Constants::WindowControls::Fonts::FONT_WEIGHT);
    painter.setFont(symbolFont);

    // Choose symbol color
    QColor symbolColor(Constants::WindowControls::Colors::MACOS_SYMBOL_COLOR);
    if (m_isPressed) {
        symbolColor = QColor(Constants::WindowControls::Colors::MACOS_SYMBOL_HOVER);
    }

    painter.setPen(symbolColor);
    
    // Calculate precise center position using font metrics
    QFontMetrics fontMetrics(symbolFont);
    QRect textRect = fontMetrics.boundingRect(m_icon);
    
    // Center the symbol precisely in the button
    int x = rect.center().x() - textRect.width() / 2;
    int y = rect.center().y() + fontMetrics.ascent() / 2 - fontMetrics.descent() / 2;
    
    painter.drawText(x, y, m_icon);

    painter.restore();
}

void WindowControlButton::drawFocusIndicator(QPainter& painter, const QRect& rect)
{
    painter.save();

    // Create focus ring
    QPen focusPen;
    focusPen.setColor(QColor(Constants::WindowControls::Colors::BUTTON_FOCUS_BORDER));
    focusPen.setWidth(2);
    focusPen.setStyle(Qt::SolidLine);

    painter.setPen(focusPen);
    painter.setBrush(Qt::NoBrush);

    // Draw focus ring slightly larger than button
    QRect focusRect = rect.adjusted(-2, -2, 2, 2);
    painter.drawEllipse(focusRect);

    painter.restore();
}

void WindowControlButton::createStateTransition(double targetOpacity)
{
    if (m_stateAnimation->state() == QPropertyAnimation::Running) {
        m_stateAnimation->stop();
    }

    m_stateAnimation->setStartValue(m_opacityEffect->opacity());
    m_stateAnimation->setEndValue(targetOpacity);
    m_stateAnimation->start();
}

void WindowControlButton::setEnabled(bool enabled)
{
    QPushButton::setEnabled(enabled);

    if (enabled) {
        updateButtonState("normal");
    } else {
        updateButtonState("disabled");
    }
}

bool WindowControlButton::isEnabled() const
{
    return QPushButton::isEnabled();
}

void WindowControlButton::enterEvent(QEnterEvent* event)
{
    if (isEnabled()) {
        m_isHovered = true;
        updateButtonState("hover");
        createStateTransition(1.0);
    }
    QPushButton::enterEvent(event);
}

void WindowControlButton::leaveEvent(QEvent* event)
{
    if (isEnabled()) {
        m_isHovered = false;
        if (m_isFocused) {
            updateButtonState("focus");
        } else {
            updateButtonState("normal");
        }
        createStateTransition(1.0);
    }
    QPushButton::leaveEvent(event);
}

void WindowControlButton::mousePressEvent(QMouseEvent* event)
{
    if (isEnabled() && event->button() == Qt::LeftButton) {
        m_isPressed = true;
        updateButtonState("pressed");
        createStateTransition(0.8);
    }
    QPushButton::mousePressEvent(event);
}

void WindowControlButton::mouseReleaseEvent(QMouseEvent* event)
{
    if (isEnabled() && event->button() == Qt::LeftButton) {
        m_isPressed = false;
        if (m_isHovered) {
            updateButtonState("hover");
        } else if (m_isFocused) {
            updateButtonState("focus");
        } else {
            updateButtonState("normal");
        }
        createStateTransition(1.0);
    }
    QPushButton::mouseReleaseEvent(event);
}

void WindowControlButton::focusInEvent(QFocusEvent* event)
{
    if (isEnabled()) {
        m_isFocused = true;
        updateButtonState("focus");
        createStateTransition(1.0);
    }
    QPushButton::focusInEvent(event);
}

void WindowControlButton::focusOutEvent(QFocusEvent* event)
{
    if (isEnabled()) {
        m_isFocused = false;
        if (m_isHovered) {
            updateButtonState("hover");
        } else {
            updateButtonState("normal");
        }
        createStateTransition(1.0);
    }
    QPushButton::focusOutEvent(event);
}

void WindowControlButton::keyPressEvent(QKeyEvent* event)
{
    if (isEnabled() && (event->key() == Qt::Key_Return || event->key() == Qt::Key_Space)) {
        m_isPressed = true;
        updateButtonState("pressed");
        createStateTransition(0.8);
    }
    QPushButton::keyPressEvent(event);
}

void WindowControlButton::keyReleaseEvent(QKeyEvent* event)
{
    if (isEnabled() && (event->key() == Qt::Key_Return || event->key() == Qt::Key_Space)) {
        m_isPressed = false;
        updateButtonState("focus");
        createStateTransition(1.0);

        // Emit clicked signal for keyboard activation
        emit clicked();
    }
    QPushButton::keyReleaseEvent(event);
}

void WindowControlButton::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    // Get button dimensions
    const int diameter = qMin(width(), height());
    const QRect buttonRect((width() - diameter) / 2, (height() - diameter) / 2, diameter, diameter);

    // Draw circular button background with gradient
    drawCircularButton(painter, buttonRect);

    // Draw symbol if hovered
    if (m_isHovered || m_isPressed) {
        drawButtonSymbol(painter, buttonRect);
    }

    // Draw focus indicator if focused
    if (m_isFocused) {
        drawFocusIndicator(painter, buttonRect);
    }
}

} // namespace Controls
} // namespace UI
} // namespace unoRadar