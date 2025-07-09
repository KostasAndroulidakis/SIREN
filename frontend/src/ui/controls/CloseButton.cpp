/**
 * CLASSIFICATION: UNCLASSIFIED
 * EXPORT CONTROL: NOT SUBJECT TO EAR/ITAR
 * CONTRACT: UNORADAR-2025
 *
 * @file CloseButton.cpp
 * @brief Implementation of close window control button
 * @author unoRadar Defense Systems
 * @date 2025
 *
 * MISRA C++ 2008 Compliant
 * DO-178C Level A Certifiable
 */

#include "ui/controls/CloseButton.h"
#include "constants/WindowControls.h"
#include <QShortcut>
#include <QKeySequence>
#include <QMouseEvent>

namespace unoRadar {
namespace UI {
namespace Controls {

CloseButton::CloseButton(QWidget* parent)
    : WindowControlButton(Constants::WindowControls::Icons::CLOSE, 
                         "Close Window (Ctrl+Q)", 
                         parent)
{
    initializeCloseButton();
}

void CloseButton::initializeCloseButton()
{
    // Set tab order for keyboard navigation (close button is last)
    setTabOrder(this, this);
    
    // Connect button click to close action
    connect(this, &QPushButton::clicked, this, &CloseButton::onButtonClicked);
    
    // Set up keyboard shortcuts for accessibility
    setupKeyboardShortcuts();
}

void CloseButton::setupKeyboardShortcuts()
{
    // Create keyboard shortcut for close (Ctrl+Q)
    QShortcut* closeShortcut = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_Q), this);
    
    // Connect shortcut to close action
    connect(closeShortcut, &QShortcut::activated, this, &CloseButton::onButtonClicked);
}

void CloseButton::applyCriticalActionStyle(const QString& state)
{
    QString styleSheet;
    
    if (state == "hover") {
        styleSheet = QString(
            "QPushButton {"
            "    background-color: %1;"
            "    border: %2px solid %1;"
            "    color: %3;"
            "    border-radius: 2px;"
            "}"
        ).arg(Constants::WindowControls::Colors::CLOSE_BUTTON_HOVER)
         .arg(Constants::WindowControls::BORDER_WIDTH)
         .arg(Constants::WindowControls::Colors::BUTTON_HOVER_TEXT);
    }
    else if (state == "pressed") {
        styleSheet = QString(
            "QPushButton {"
            "    background-color: %1;"
            "    border: %2px solid %1;"
            "    color: %3;"
            "    border-radius: 2px;"
            "}"
        ).arg(Constants::WindowControls::Colors::CLOSE_BUTTON_PRESSED)
         .arg(Constants::WindowControls::BORDER_WIDTH)
         .arg(Constants::WindowControls::Colors::BUTTON_PRESSED_TEXT);
    }
    else { // normal state - use base class styling
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

void CloseButton::enterEvent(QEnterEvent* event)
{
    if (isEnabled()) {
        applyCriticalActionStyle("hover");
    }
    
    // Call base class implementation for state management
    WindowControlButton::enterEvent(event);
}

void CloseButton::leaveEvent(QEvent* event)
{
    if (isEnabled()) {
        applyCriticalActionStyle("normal");
    }
    
    // Call base class implementation for state management
    WindowControlButton::leaveEvent(event);
}

void CloseButton::mousePressEvent(QMouseEvent* event)
{
    if (isEnabled() && event->button() == Qt::LeftButton) {
        applyCriticalActionStyle("pressed");
    }
    
    // Call base class implementation for state management
    WindowControlButton::mousePressEvent(event);
}

void CloseButton::onButtonClicked()
{
    // Emit signal to request window close
    emit closeRequested();
}

} // namespace Controls
} // namespace UI
} // namespace unoRadar