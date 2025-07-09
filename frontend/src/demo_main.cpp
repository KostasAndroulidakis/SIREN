/**
 * CLASSIFICATION: UNCLASSIFIED
 * EXPORT CONTROL: NOT SUBJECT TO EAR/ITAR
 * CONTRACT: UNORADAR-2025
 *
 * @file demo_main.cpp
 * @brief Military-grade window controls demonstration
 * @author unoRadar Defense Systems
 * @date 2025
 *
 * MISRA C++ 2008 Compliant
 * DO-178C Level A Certifiable
 * 
 * This demonstration shows the complete implementation of:
 * - Custom window control buttons (minimize, maximize, close)
 * - Military-standard styling and behavior
 * - Frameless window with custom title bar
 * - SRP and SSOT compliance
 * - Keyboard navigation and accessibility
 */

#include <QApplication>
#include <QStyleFactory>
#include <QDir>
#include <QStandardPaths>
#include "ui/MilitaryMainWindow.h"
#include "constants/Application.h"
#include "constants/UI.h"

/**
 * @brief Main entry point for military-grade window controls demo
 * @param argc Argument count
 * @param argv Argument vector
 * @return Application exit code
 */
int main(int argc, char *argv[])
{
    // Create Qt application
    QApplication app(argc, argv);
    
    // Set application metadata
    app.setApplicationName(unoRadar::Constants::Application::NAME);
    app.setApplicationDisplayName(QString("%1 - Military Controls Demo")
        .arg(unoRadar::Constants::Application::FULL_NAME));
    app.setApplicationVersion(unoRadar::Constants::Application::VERSION);
    app.setOrganizationName("unoRadar Defense Systems");
    app.setOrganizationDomain("unoradar.defense");
    
    // Set application icon (if available)
    // app.setWindowIcon(QIcon(":/icons/unoradar_icon.png"));
    
    // Create and show military-grade main window
    unoRadar::UI::MilitaryMainWindow window;
    window.show();
    
    // Center window on screen
    window.move(QGuiApplication::primaryScreen()->geometry().center() - window.rect().center());
    
    // Run event loop
    return app.exec();
}