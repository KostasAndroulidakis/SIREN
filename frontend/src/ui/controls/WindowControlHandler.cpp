/**
 * CLASSIFICATION: UNCLASSIFIED
 * EXPORT CONTROL: NOT SUBJECT TO EAR/ITAR
 * CONTRACT: SIREN-2025
 *
 * @file WindowControlHandler.cpp
 * @brief Implementation of window control event handler
 * @author KostasAndroulidakis
 * @date 2025
 *
 * MISRA C++ 2008 Compliant
 * DO-178C Level A Certifiable
 */

#include "ui/controls/WindowControlHandler.h"
#include "ui/controls/WindowControlBar.h"
#include <QEvent>
#include <QWindowStateChangeEvent>
#include <QCloseEvent>
#include <QApplication>

namespace siren {
namespace UI {
namespace Controls {

WindowControlHandler::WindowControlHandler(WindowControlBar* controlBar,
                                           QWidget* targetWindow,
                                           QObject* parent)
    : QObject(parent)
    , m_controlBar(controlBar)
    , m_targetWindow(targetWindow)
    , m_isMaximized(false)
    , m_controlsEnabled(true)
    , m_isInitialized(false)
{
    // Validate input parameters
    if (m_controlBar == nullptr || m_targetWindow == nullptr) {
        // Handle error condition - in a real military system, this would be logged
        return;
    }
}

void WindowControlHandler::initialize()
{
    if (m_isInitialized) {
        return; // Already initialized
    }

    // Connect control bar signals
    connectControlBarSignals();

    // Set up window event monitoring
    setupWindowEventMonitoring();

    // Initialize window state
    validateAndUpdateWindowState();

    m_isInitialized = true;
}

void WindowControlHandler::connectControlBarSignals()
{
    if (m_controlBar == nullptr) {
        return;
    }

    // Connect minimize signal
    connect(m_controlBar, &WindowControlBar::minimizeRequested,
            this, &WindowControlHandler::handleMinimizeRequest);

    // Connect maximize signal
    connect(m_controlBar, &WindowControlBar::maximizeRequested,
            this, &WindowControlHandler::handleMaximizeRequest);

    // Connect restore signal
    connect(m_controlBar, &WindowControlBar::restoreRequested,
            this, &WindowControlHandler::handleRestoreRequest);

    // Connect close signal
    connect(m_controlBar, &WindowControlBar::closeRequested,
            this, &WindowControlHandler::handleCloseRequest);
}

void WindowControlHandler::setupWindowEventMonitoring()
{
    if (m_targetWindow == nullptr) {
        return;
    }

    // Install event filter to monitor window state changes
    m_targetWindow->installEventFilter(this);
}

void WindowControlHandler::validateAndUpdateWindowState()
{
    if (m_targetWindow == nullptr) {
        return;
    }

    // Check current window state
    bool isCurrentlyMaximized = m_targetWindow->isMaximized();

    if (m_isMaximized != isCurrentlyMaximized) {
        m_isMaximized = isCurrentlyMaximized;
        updateControlBarState();
        emit windowStateChanged(m_isMaximized);
    }
}

void WindowControlHandler::updateControlBarState()
{
    if (m_controlBar == nullptr) {
        return;
    }

    // Update control bar maximize button state
    m_controlBar->updateMaximizedState(m_isMaximized);
}

void WindowControlHandler::setControlsEnabled(bool enabled)
{
    m_controlsEnabled = enabled;

    if (m_controlBar != nullptr) {
        m_controlBar->setControlsEnabled(enabled);
    }
}

bool WindowControlHandler::isWindowMaximized() const
{
    return m_isMaximized;
}

void WindowControlHandler::handleMinimizeRequest()
{
    if (m_targetWindow == nullptr || !m_controlsEnabled) {
        return;
    }

    // Minimize the window
    m_targetWindow->showMinimized();
}

void WindowControlHandler::handleMaximizeRequest()
{
    if (m_targetWindow == nullptr || !m_controlsEnabled) {
        return;
    }

    // Maximize the window
    m_targetWindow->showMaximized();
}

void WindowControlHandler::handleRestoreRequest()
{
    if (m_targetWindow == nullptr || !m_controlsEnabled) {
        return;
    }

    // Restore the window to normal state
    m_targetWindow->showNormal();
}

void WindowControlHandler::handleCloseRequest()
{
    if (m_targetWindow == nullptr || !m_controlsEnabled) {
        return;
    }

    // Emit signal to allow application to handle pre-close logic
    emit windowClosing();

    // Close the window
    m_targetWindow->close();
}

bool WindowControlHandler::eventFilter(QObject* watched, QEvent* event)
{
    // Only process events for our target window
    if (watched != m_targetWindow) {
        return QObject::eventFilter(watched, event);
    }

    // Handle window state change events
    if (event->type() == QEvent::WindowStateChange) {
        handleWindowStateChange(event);
    }

    // Allow event to continue processing
    return QObject::eventFilter(watched, event);
}

void WindowControlHandler::handleWindowStateChange(QEvent* event)
{
    Q_UNUSED(event)

    // Validate and update window state
    validateAndUpdateWindowState();
}

} // namespace Controls
} // namespace UI
} // namespace siren