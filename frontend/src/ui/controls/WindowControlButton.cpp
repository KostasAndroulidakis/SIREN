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
    // Set fixed size according to MIL-STD-1472
    setFixedSize(Constants::WindowControls::BUTTON_WIDTH, 
                 Constants::WindowControls::BUTTON_HEIGHT);

    // Set minimum size for accessibility
    setMinimumSize(Constants::WindowControls::BUTTON_MIN_SIZE, 
                   Constants::WindowControls::BUTTON_MIN_SIZE);

    // Configure text and tooltip
    setText(m_icon);
    setToolTip(m_tooltip);

    // Enable focus for keyboard navigation
    setFocusPolicy(Qt::StrongFocus);

    // Set up opacity effect for animations
    setGraphicsEffect(m_opacityEffect.get());
    m_opacityEffect->setOpacity(1.0);

    // Configure state animation
    m_stateAnimation->setTargetObject(m_opacityEffect.get());
    m_stateAnimation->setPropertyName("opacity");
    m_stateAnimation->setDuration(Constants::WindowControls::HOVER_ANIMATION_DURATION);

    // Apply military-standard styling
    applyMilitaryStyle();
}

void WindowControlButton::applyMilitaryStyle()
{
    // Create military-compliant font
    QFont buttonFont(Constants::WindowControls::Fonts::FONT_FAMILY, 
                     Constants::WindowControls::Fonts::FONT_SIZE, 
                     Constants::WindowControls::Fonts::FONT_WEIGHT);
    setFont(buttonFont);

    // Apply base styling
    updateButtonState("normal");
}

void WindowControlButton::updateButtonState(const QString& state)
{
    QString styleSheet;
    
    if (state == "hover") {
        styleSheet = QString(
            "QPushButton {"
            "    background-color: %1;"
            "    border: %2px solid %3;"
            "    color: %4;"
            "    border-radius: 2px;"
            "}"
        ).arg(Constants::WindowControls::Colors::BUTTON_HOVER_BACKGROUND)
         .arg(Constants::WindowControls::BORDER_WIDTH)
         .arg(Constants::WindowControls::Colors::BUTTON_HOVER_BORDER)
         .arg(Constants::WindowControls::Colors::BUTTON_HOVER_TEXT);
    }
    else if (state == "pressed") {
        styleSheet = QString(
            "QPushButton {"
            "    background-color: %1;"
            "    border: %2px solid %3;"
            "    color: %4;"
            "    border-radius: 2px;"
            "}"
        ).arg(Constants::WindowControls::Colors::BUTTON_PRESSED_BACKGROUND)
         .arg(Constants::WindowControls::BORDER_WIDTH)
         .arg(Constants::WindowControls::Colors::BUTTON_PRESSED_BORDER)
         .arg(Constants::WindowControls::Colors::BUTTON_PRESSED_TEXT);
    }
    else if (state == "focus") {
        styleSheet = QString(
            "QPushButton {"
            "    background-color: %1;"
            "    border: %2px solid %3;"
            "    color: %4;"
            "    border-radius: 2px;"
            "}"
        ).arg(Constants::WindowControls::Colors::BUTTON_BACKGROUND)
         .arg(Constants::WindowControls::BORDER_WIDTH)
         .arg(Constants::WindowControls::Colors::BUTTON_FOCUS_BORDER)
         .arg(Constants::WindowControls::Colors::BUTTON_TEXT);
    }
    else if (state == "disabled") {
        styleSheet = QString(
            "QPushButton {"
            "    background-color: %1;"
            "    border: %2px solid %3;"
            "    color: %4;"
            "    border-radius: 2px;"
            "}"
        ).arg(Constants::WindowControls::Colors::BUTTON_DISABLED_BACKGROUND)
         .arg(Constants::WindowControls::BORDER_WIDTH)
         .arg(Constants::WindowControls::Colors::BUTTON_DISABLED_BORDER)
         .arg(Constants::WindowControls::Colors::BUTTON_DISABLED_TEXT);
    }
    else { // normal state
        styleSheet = QString(
            "QPushButton {"
            "    background-color: %1;"
            "    border: %2px solid %3;"
            "    color: %4;"
            "    border-radius: 2px;"
            "}"
        ).arg(Constants::WindowControls::Colors::BUTTON_BACKGROUND)
         .arg(Constants::WindowControls::BORDER_WIDTH)
         .arg(Constants::WindowControls::Colors::BUTTON_BORDER)
         .arg(Constants::WindowControls::Colors::BUTTON_TEXT);
    }

    setStyleSheet(styleSheet);
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
    // Use default Qt painting with our custom styles
    QPushButton::paintEvent(event);
}

} // namespace Controls
} // namespace UI
} // namespace unoRadar