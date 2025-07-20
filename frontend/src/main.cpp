/**
 * CLASSIFICATION: UNCLASSIFIED
 * EXPORT CONTROL: NOT SUBJECT TO EAR/ITAR
 * CONTRACT: SIREN-2025
 *
 * @file main.cpp
 * @brief Main entry point for SIREN frontend
 * @author SIREN Defense Systems
 * @date 2025
 *
 * MISRA C++ 2008 Compliant
 * DO-178C Level A Certifiable
 */

#include <QtWidgets/QApplication>
#include <QtCore/QString>
#include <QtGui/QPalette>
#include <QtCore/QTimer>
#include <QtCore/QEventLoop>

// Include our constants
#include "constants/Application.h"

// Include military main window
#include "ui/MilitaryMainWindow.h"

// Include splash screen
#include "ui/SplashScreen.h"

#ifdef Q_OS_MACOS
#include <QWindow>
#endif

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // Force Qt to ignore system theme and use our custom colors
    app.setAttribute(Qt::AA_UseStyleSheetPropagationInWidgetStyles, true);

    // Force Fusion style for consistent cross-platform behavior
    app.setStyle("Fusion");

    // Military-grade true black QPalette (Complete override)
    QPalette palette;
    palette.setColor(QPalette::Window, QColor(0, 0, 0));           // True black background
    palette.setColor(QPalette::WindowText, QColor(0, 255, 65));   // Military green text
    palette.setColor(QPalette::Base, QColor(0, 0, 0));            // Input field background
    palette.setColor(QPalette::AlternateBase, QColor(31, 31, 31)); // Alternate row color
    palette.setColor(QPalette::Text, QColor(0, 255, 65));         // Input field text
    palette.setColor(QPalette::Button, QColor(0, 0, 0));          // Button background
    palette.setColor(QPalette::ButtonText, QColor(0, 255, 65));   // Button text
    palette.setColor(QPalette::BrightText, QColor(255, 255, 255)); // Bright text
    palette.setColor(QPalette::Link, QColor(0, 255, 65));         // Links
    palette.setColor(QPalette::Highlight, QColor(0, 255, 65));    // Selection highlight
    palette.setColor(QPalette::HighlightedText, QColor(0, 0, 0)); // Selected text
    app.setPalette(palette);

    // Set application metadata
    app.setApplicationName(siren::Constants::Application::NAME);
    app.setApplicationDisplayName(siren::Constants::Application::FULL_NAME);
    app.setApplicationVersion(siren::Constants::Application::VERSION);

    // Create splash screen for military-grade startup sequence
    siren::ui::SplashScreen splash;
    
    // Create main window but don't show it yet
    siren::ui::MilitaryMainWindow window;
    
    // Connect splash screen completion to main window display
    QObject::connect(&splash, &siren::ui::SplashScreen::loadingComplete, [&window]() {
        window.showMaximized();  // Open in full-size window
        window.raise();
        window.activateWindow();
    });
    
    // Handle splash screen errors gracefully
    QObject::connect(&splash, &siren::ui::SplashScreen::errorOccurred, [&window](const QString& error) {
        qDebug() << "Splash screen error:" << error;
        // Show main window anyway if splash fails
        window.showMaximized();  // Open in full-size window
    });
    
    // Show splash screen and start application
    if (!splash.showSplash()) {
        // If splash fails, show main window directly
        qDebug() << "Failed to show splash screen, showing main window directly";
        window.showMaximized();  // Open in full-size window
    }

    return app.exec();
}