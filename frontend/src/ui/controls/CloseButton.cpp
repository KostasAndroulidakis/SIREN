/**
 * CLASSIFICATION: UNCLASSIFIED
 * EXPORT CONTROL: NOT SUBJECT TO EAR/ITAR
 * CONTRACT: SIREN-2025
 *
 * @file CloseButton.cpp
 * @brief Implementation of close window control button
 * @author KostasAndroulidakis
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
#include <QColor>

namespace siren {
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

QColor CloseButton::getBaseColor() const
{
    return QColor(Constants::WindowControls::Colors::MACOS_RED);
}

QColor CloseButton::getHoverColor() const
{
    return QColor(Constants::WindowControls::Colors::MACOS_RED_HOVER);
}


void CloseButton::onButtonClicked()
{
    // Emit signal to request window close
    emit closeRequested();
}

} // namespace Controls
} // namespace UI
} // namespace siren