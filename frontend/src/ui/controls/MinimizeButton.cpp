/**
 * CLASSIFICATION: UNCLASSIFIED
 * EXPORT CONTROL: NOT SUBJECT TO EAR/ITAR
 * CONTRACT: SIREN-2025
 *
 * @file MinimizeButton.cpp
 * @brief Implementation of minimize window control button
 * @author SIREN Defense Systems
 * @date 2025
 *
 * MISRA C++ 2008 Compliant
 * DO-178C Level A Certifiable
 */

#include "ui/controls/MinimizeButton.h"
#include "constants/WindowControls.h"
#include <QShortcut>
#include <QKeySequence>
#include <QColor>

namespace siren {
namespace UI {
namespace Controls {

MinimizeButton::MinimizeButton(QWidget* parent)
    : WindowControlButton(Constants::WindowControls::Icons::MINIMIZE,
                         "Minimize Window (Ctrl+M)",
                         parent)
{
    initializeMinimizeButton();
}

void MinimizeButton::initializeMinimizeButton()
{
    // Set tab order for keyboard navigation
    setTabOrder(this, this);

    // Connect button click to minimize action
    connect(this, &QPushButton::clicked, this, &MinimizeButton::onButtonClicked);

    // Set up keyboard shortcuts for accessibility
    setupKeyboardShortcuts();
}

void MinimizeButton::setupKeyboardShortcuts()
{
    // Create keyboard shortcut for minimize (Ctrl+M)
    QShortcut* minimizeShortcut = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_M), this);

    // Connect shortcut to minimize action
    connect(minimizeShortcut, &QShortcut::activated, this, &MinimizeButton::onButtonClicked);
}

QColor MinimizeButton::getBaseColor() const
{
    return QColor(Constants::WindowControls::Colors::MACOS_YELLOW);
}

QColor MinimizeButton::getHoverColor() const
{
    return QColor(Constants::WindowControls::Colors::MACOS_YELLOW_HOVER);
}

void MinimizeButton::onButtonClicked()
{
    // Emit signal to request window minimize
    emit minimizeRequested();
}

} // namespace Controls
} // namespace UI
} // namespace siren