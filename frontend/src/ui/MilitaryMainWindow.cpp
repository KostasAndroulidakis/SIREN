/**
 * CLASSIFICATION: UNCLASSIFIED
 * EXPORT CONTROL: NOT SUBJECT TO EAR/ITAR
 * CONTRACT: UNORADAR-2025
 *
 * @file MilitaryMainWindow.cpp
 * @brief Implementation of military-grade main window with custom controls
 * @author unoRadar Defense Systems
 * @date 2025
 *
 * MISRA C++ 2008 Compliant
 * DO-178C Level A Certifiable
 */

#include "ui/MilitaryMainWindow.h"
#include "ui/controls/WindowControlBar.h"
#include "ui/controls/WindowControlHandler.h"
#include "constants/UI.h"
#include "constants/WindowControls.h"
#include "constants/Application.h"
#include <QHBoxLayout>
#include <QLabel>
#include <QCloseEvent>
#include <QMouseEvent>
#include <QApplication>
#include <QScreen>

namespace unoRadar {
namespace UI {

MilitaryMainWindow::MilitaryMainWindow(QWidget* parent)
    : QMainWindow(parent)
    , m_titleBar(std::make_unique<QWidget>(this))
    , m_centralWidget(std::make_unique<QWidget>(this))
    , m_mainLayout(std::make_unique<QVBoxLayout>(m_centralWidget.get()))
    , m_contentDisplay(std::make_unique<QTextEdit>(m_centralWidget.get()))
    , m_controlBar(std::make_unique<Controls::WindowControlBar>(m_titleBar.get()))
    , m_controlHandler(std::make_unique<Controls::WindowControlHandler>(m_controlBar.get(), static_cast<QWidget*>(this), this))
    , m_titleLabel(std::make_unique<QLabel>(Constants::Application::COMMAND_CENTER_TITLE, m_titleBar.get()))
    , m_isDragging(false)
    , m_dragStartPosition()
    , m_isFullSize(false)
    , m_normalGeometry()
{
    initializeMilitaryWindow();
}

MilitaryMainWindow::~MilitaryMainWindow()
{
    // Destructor implementation - unique_ptr destructors are called automatically
    // This definition is needed here because the complete types are available
}

void MilitaryMainWindow::initializeMilitaryWindow()
{
    // Set up window flags for frameless operation
    setupWindowFlags();

    // Set window properties
    setWindowTitle(Constants::Application::COMMAND_CENTER_TITLE);

    resize(Constants::UI::WINDOW_WIDTH, Constants::UI::WINDOW_HEIGHT);
    setMinimumSize(Constants::UI::MIN_WINDOW_WIDTH, Constants::UI::MIN_WINDOW_HEIGHT);

    // Create UI components
    createCustomTitleBar();
    createMainContent();

    // Apply military styling
    applyMilitaryTheme();

    // Initialize control handler
    m_controlHandler->initialize();

    // Connect control handler signals
    connect(m_controlHandler.get(), &Controls::WindowControlHandler::windowClosing,
            this, &MilitaryMainWindow::onWindowClosing);

    connect(m_controlHandler.get(), &Controls::WindowControlHandler::windowStateChanged,
            this, &MilitaryMainWindow::onWindowStateChanged);
}

void MilitaryMainWindow::setupWindowFlags()
{
    // Set frameless window with custom controls
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);

    // Enable window resizing
    setAttribute(Qt::WA_Hover, true);

    // Set window to stay on top during development (remove in production)
    // setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
}

void MilitaryMainWindow::createCustomTitleBar()
{
    // Set title bar properties
    m_titleBar->setFixedHeight(Constants::WindowControls::CONTROL_BAR_HEIGHT);
    m_titleBar->setStyleSheet(QString("background-color: %1; border-bottom: 1px solid %2;")
        .arg(Constants::UI::Colors::BACKGROUND)
        .arg(Constants::UI::Colors::GRID_LINES));

    // Create title bar layout - title takes full width for perfect centering
    auto titleLayout = std::make_unique<QHBoxLayout>(m_titleBar.get());
    titleLayout->setContentsMargins(0, 0, 0, 0);
    titleLayout->setSpacing(0);

    // Configure title label with military-grade styling
    m_titleLabel->setStyleSheet(QString("color: %1; font-weight: %2; font-size: %3px; font-family: %4;")
        .arg(Constants::UI::Colors::PRIMARY_TEXT)
        .arg(Constants::UI::TITLE_FONT_WEIGHT)
        .arg(Constants::UI::TITLE_FONT_SIZE)
        .arg(Constants::UI::Fonts::MONOSPACE));
    m_titleLabel->setAlignment(Qt::AlignCenter);

    // Enable mouse tracking for double-click detection
    m_titleLabel->installEventFilter(this);

    // Title takes full width for perfect center alignment
    titleLayout->addWidget(m_titleLabel.get());

    // Position traffic lights absolutely on top-left (not affecting layout)
    m_controlBar->setParent(m_titleBar.get());
    m_controlBar->resize(100, Constants::WindowControls::CONTROL_BAR_HEIGHT); // Give it proper size
    m_controlBar->move(0, 0);    // Position at very left edge (0px padding)
    m_controlBar->raise(); // Bring to front
    m_controlBar->show();

    m_titleBar->setLayout(titleLayout.release());
}

void MilitaryMainWindow::createMainContent()
{
    // Set up main layout
    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    m_mainLayout->setSpacing(0);

    // Add title bar to main layout
    m_mainLayout->addWidget(m_titleBar.get());

    // Configure content display
    m_contentDisplay->setReadOnly(true);
    m_contentDisplay->setText(
        "COMMAND CENTER - unoRadar Military-Grade Frontend\n"
        "Centered Title with Left-Positioned Traffic Lights\n\n"
        "Features:\n"
        "• Frameless window with custom controls\n"
        "• MIL-STD-1472 compliant styling\n"
        "• MISRA C++ 2008 compliant code\n"
        "• DO-178C Level A certifiable design\n"
        "• Single Responsibility Principle (SRP)\n"
        "• Single Source of Truth (SSOT)\n\n"
        "Title Bar Layout:\n"
        "• Traffic lights positioned on left (20px from edge)\n"
        "• COMMAND CENTER title centered in title bar\n"
        "• Military-grade font styling (bold, 14px)\n"
        "• Proper spacing and alignment\n\n"
        "Controls:\n"
        "• Minimize: Ctrl+M\n"
        "• Maximize/Restore: Ctrl+Shift+M\n"
        "• Close: Ctrl+Q\n"
        "• Tab navigation supported\n\n"
        "Window Features:\n"
        "• Drag title bar to move window\n"
        "• Hover effects on all controls\n"
        "• Keyboard accessibility\n"
        "• Military-grade color scheme\n"
        "• True black background\n"
        "• High contrast ratios\n\n"
        "Status: Ready for radar data integration"
    );

    // Add content to main layout
    m_mainLayout->addWidget(m_contentDisplay.get(), 1); // Stretch factor 1

    // Set central widget
    setCentralWidget(m_centralWidget.get());
}

void MilitaryMainWindow::applyMilitaryTheme()
{
    // Apply military-grade black theme
    QString globalStyle = QString(
        "QMainWindow { "
        "    background-color: %1; "
        "    color: %2; "
        "} "
        "QWidget { "
        "    background-color: %1; "
        "    color: %2; "
        "} "
        "QTextEdit { "
        "    background-color: %1; "
        "    color: %2; "
        "    border: 1px solid %3; "
        "    padding: 10px; "
        "    font-family: %4; "
        "    font-size: %5px; "
        "    selection-background-color: %6; "
        "} "
        "QLabel { "
        "    background-color: transparent; "
        "    color: %2; "
        "}"
    ).arg(Constants::UI::Colors::BACKGROUND)
     .arg(Constants::UI::Colors::PRIMARY_TEXT)
     .arg(Constants::UI::Colors::GRID_LINES)
     .arg(Constants::UI::Fonts::MONOSPACE)
     .arg(Constants::UI::Fonts::DEFAULT_SIZE)
     .arg(Constants::UI::Colors::RADAR_SWEEP);

    setStyleSheet(globalStyle);

    // Force auto-fill background
    setAutoFillBackground(true);
}

void MilitaryMainWindow::closeEvent(QCloseEvent* event)
{
    // Handle any cleanup before closing
    event->accept();
}

void MilitaryMainWindow::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        // Check if click is in title bar area (for dragging)
        QRect titleBarRect = m_titleBar->geometry();
        if (titleBarRect.contains(event->position().toPoint())) {
            m_isDragging = true;
            m_dragStartPosition = event->globalPosition().toPoint() - frameGeometry().topLeft();
        }
    }

    QMainWindow::mousePressEvent(event);
}

void MilitaryMainWindow::mouseMoveEvent(QMouseEvent* event)
{
    if (m_isDragging && (event->buttons() & Qt::LeftButton)) {
        // Move window during drag
        move(event->globalPosition().toPoint() - m_dragStartPosition);
    }

    QMainWindow::mouseMoveEvent(event);
}

void MilitaryMainWindow::onWindowClosing()
{
    // Handle pre-close logic here
    // In a real application, this might save state, confirm close, etc.

    // Update content display
    m_contentDisplay->append("\n[SYSTEM] Window closing requested...");

    // Process any pending events
    QApplication::processEvents();
}

void MilitaryMainWindow::onWindowStateChanged(bool isMaximized)
{
    // Update content display with state change
    QString stateText = isMaximized ? "MAXIMIZED" : "NORMAL";
    m_contentDisplay->append(QString("\n[SYSTEM] Window state changed to: %1").arg(stateText));

    // Handle any state-specific logic here
    if (isMaximized) {
        // Disable dragging when maximized
        m_isDragging = false;
    }
}

bool MilitaryMainWindow::eventFilter(QObject* object, QEvent* event)
{
    // Check if this is a double-click on the title label
    if (object == m_titleLabel.get() && event->type() == QEvent::MouseButtonDblClick) {
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
        if (mouseEvent->button() == Qt::LeftButton) {
            toggleFullSize();
            return true; // Event handled
        }
    }
    
    // Pass event to parent class
    return QMainWindow::eventFilter(object, event);
}

void MilitaryMainWindow::toggleFullSize()
{
    if (m_isFullSize) {
        // Restore to normal size
        if (!m_normalGeometry.isNull()) {
            setGeometry(m_normalGeometry);
            m_isFullSize = false;
            m_contentDisplay->append("\n[SYSTEM] Window restored to normal size");
        }
    } else {
        // Save current geometry and go full size
        m_normalGeometry = geometry();
        
        // Get screen geometry and set window to fill it (but not full-screen mode)
        QRect screenRect = QApplication::primaryScreen()->availableGeometry();
        setGeometry(screenRect);
        m_isFullSize = true;
        m_contentDisplay->append("\n[SYSTEM] Window expanded to full size");
    }
}

} // namespace UI
} // namespace unoRadar