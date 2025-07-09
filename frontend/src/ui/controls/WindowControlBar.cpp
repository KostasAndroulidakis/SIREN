/**
 * CLASSIFICATION: UNCLASSIFIED
 * EXPORT CONTROL: NOT SUBJECT TO EAR/ITAR
 * CONTRACT: UNORADAR-2025
 *
 * @file WindowControlBar.cpp
 * @brief Implementation of window control bar container widget
 * @author unoRadar Defense Systems
 * @date 2025
 *
 * MISRA C++ 2008 Compliant
 * DO-178C Level A Certifiable
 */

#include "ui/controls/WindowControlBar.h"
#include "ui/controls/MinimizeButton.h"
#include "ui/controls/MaximizeButton.h"
#include "ui/controls/CloseButton.h"
#include "constants/WindowControls.h"
#include <QPainter>
#include <QResizeEvent>
#include <QSpacerItem>

namespace unoRadar {
namespace UI {
namespace Controls {

WindowControlBar::WindowControlBar(QWidget* parent)
    : QWidget(parent)
    , m_layout(std::make_unique<QHBoxLayout>(this))
    , m_minimizeButton(std::make_unique<MinimizeButton>(this))
    , m_maximizeButton(std::make_unique<MaximizeButton>(this))
    , m_closeButton(std::make_unique<CloseButton>(this))
    , m_controlsEnabled(true)
{
    initializeControlBar();
}

WindowControlBar::~WindowControlBar()
{
    // Destructor implementation - unique_ptr destructors are called automatically
    // This definition is needed here because the complete types are available
}

void WindowControlBar::initializeControlBar()
{
    // Set fixed height for the control bar
    setFixedHeight(Constants::WindowControls::CONTROL_BAR_HEIGHT);
    
    // Set minimum width to accommodate all buttons
    const std::int32_t minWidth = (Constants::WindowControls::BUTTON_WIDTH * 3) + 
                                  (Constants::WindowControls::BUTTON_SPACING * 2) + 
                                  (Constants::WindowControls::CONTROL_BAR_PADDING * 2);
    setMinimumWidth(minWidth);

    // Create and configure control buttons
    createControlButtons();
    
    // Set up layout
    setupLayout();
    
    // Connect button signals
    connectButtonSignals();
    
    // Apply military styling
    applyMilitaryStyle();
    
    // Set up tab order for keyboard navigation
    setupTabOrder();
}

void WindowControlBar::createControlButtons()
{
    // Buttons are already created in constructor initializer list
    // Configure their properties here
    
    // Set focus policies for keyboard navigation
    m_minimizeButton->setFocusPolicy(Qt::StrongFocus);
    m_maximizeButton->setFocusPolicy(Qt::StrongFocus);
    m_closeButton->setFocusPolicy(Qt::StrongFocus);
}

void WindowControlBar::setupLayout()
{
    // Configure layout properties
    m_layout->setContentsMargins(Constants::WindowControls::CONTROL_BAR_PADDING,
                                Constants::WindowControls::CONTROL_BAR_PADDING,
                                Constants::WindowControls::CONTROL_BAR_PADDING,
                                Constants::WindowControls::CONTROL_BAR_PADDING);
    
    m_layout->setSpacing(Constants::WindowControls::BUTTON_SPACING);
    m_layout->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    // Add spacer to push buttons to the right
    m_layout->addStretch();
    
    // Add buttons in standard order (minimize, maximize, close)
    m_layout->addWidget(m_minimizeButton.get());
    m_layout->addWidget(m_maximizeButton.get());
    m_layout->addWidget(m_closeButton.get());

    // Set the layout
    setLayout(m_layout.get());
}

void WindowControlBar::connectButtonSignals()
{
    // Connect minimize button
    connect(m_minimizeButton.get(), &MinimizeButton::minimizeRequested,
            this, &WindowControlBar::minimizeRequested);

    // Connect maximize button
    connect(m_maximizeButton.get(), &MaximizeButton::maximizeRequested,
            this, &WindowControlBar::maximizeRequested);
    
    connect(m_maximizeButton.get(), &MaximizeButton::restoreRequested,
            this, &WindowControlBar::restoreRequested);

    // Connect close button
    connect(m_closeButton.get(), &CloseButton::closeRequested,
            this, &WindowControlBar::closeRequested);
}

void WindowControlBar::applyMilitaryStyle()
{
    // Apply transparent background to allow parent styling
    setStyleSheet("WindowControlBar { background-color: transparent; }");
    
    // Set auto-fill background to false to maintain transparency
    setAutoFillBackground(false);
}

void WindowControlBar::setupTabOrder()
{
    // Set tab order according to MIL-STD-1472 standards
    setTabOrder(m_minimizeButton.get(), m_maximizeButton.get());
    setTabOrder(m_maximizeButton.get(), m_closeButton.get());
}

void WindowControlBar::updateMaximizedState(bool isMaximized)
{
    m_maximizeButton->updateMaximizedState(isMaximized);
}

void WindowControlBar::setControlsEnabled(bool enabled)
{
    m_controlsEnabled = enabled;
    
    m_minimizeButton->setEnabled(enabled);
    m_maximizeButton->setEnabled(enabled);
    m_closeButton->setEnabled(enabled);
}

bool WindowControlBar::areControlsEnabled() const
{
    return m_controlsEnabled;
}

void WindowControlBar::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event)
    
    // Custom paint implementation for military styling
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    // Draw transparent background (allows parent styling to show through)
    painter.fillRect(rect(), Qt::transparent);
}

void WindowControlBar::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
    
    // Update layout if needed
    if (m_layout) {
        m_layout->update();
    }
}

} // namespace Controls
} // namespace UI
} // namespace unoRadar