/**
 * CLASSIFICATION: UNCLASSIFIED
 * EXPORT CONTROL: NOT SUBJECT TO EAR/ITAR
 * CONTRACT: SIREN-2025
 *
 * @file MaximizeButton.cpp
 * @brief Implementation of maximize/restore window control button
 * @author KostasAndroulidakis
 * @date 2025
 *
 * MISRA C++ 2008 Compliant
 * DO-178C Level A Certifiable
 */

#include "ui/controls/MaximizeButton.h"
#include "constants/WindowControls.h"
#include <QShortcut>
#include <QKeySequence>
#include <QColor>

namespace siren {
namespace UI {
namespace Controls {

MaximizeButton::MaximizeButton(QWidget* parent)
    : WindowControlButton(Constants::WindowControls::Icons::MAXIMIZE,
                         "Maximize Window (Ctrl+Shift+M)",
                         parent)
    , m_isMaximized(false)
{
    initializeMaximizeButton();
}

void MaximizeButton::initializeMaximizeButton()
{
    // Set tab order for keyboard navigation
    setTabOrder(this, this);

    // Connect button click to maximize/restore action
    connect(this, &QPushButton::clicked, this, &MaximizeButton::onButtonClicked);

    // Set up keyboard shortcuts for accessibility
    setupKeyboardShortcuts();

    // Initialize button appearance
    updateButtonAppearance();
}

void MaximizeButton::setupKeyboardShortcuts()
{
    // Create keyboard shortcut for maximize/restore (Ctrl+Shift+M)
    QShortcut* maximizeShortcut = new QShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_M), this);

    // Connect shortcut to maximize/restore action
    connect(maximizeShortcut, &QShortcut::activated, this, &MaximizeButton::onButtonClicked);
}

void MaximizeButton::updateMaximizedState(bool isMaximized)
{
    if (m_isMaximized != isMaximized) {
        m_isMaximized = isMaximized;
        updateButtonAppearance();
    }
}

void MaximizeButton::updateButtonAppearance()
{
    if (m_isMaximized) {
        // Window is maximized, show restore icon
        setText(Constants::WindowControls::Icons::RESTORE);
    } else {
        // Window is normal, show maximize icon
        setText(Constants::WindowControls::Icons::MAXIMIZE);
    }
}

QColor MaximizeButton::getBaseColor() const
{
    return QColor(Constants::WindowControls::Colors::MACOS_GREEN);
}

QColor MaximizeButton::getHoverColor() const
{
    return QColor(Constants::WindowControls::Colors::MACOS_GREEN_HOVER);
}

void MaximizeButton::onButtonClicked()
{
    if (m_isMaximized) {
        // Request restore action
        emit restoreRequested();
    } else {
        // Request maximize action
        emit maximizeRequested();
    }
}

} // namespace Controls
} // namespace UI
} // namespace siren