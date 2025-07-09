/**
 * CLASSIFICATION: UNCLASSIFIED
 * EXPORT CONTROL: NOT SUBJECT TO EAR/ITAR
 * CONTRACT: UNORADAR-2025
 *
 * @file main.cpp
 * @brief Main entry point for unoRadar frontend
 * @author unoRadar Defense Systems
 * @date 2025
 *
 * MISRA C++ 2008 Compliant
 * DO-178C Level A Certifiable
 */

#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QTextEdit>
#include <QtCore/QString>

// Include our constants
#include "constants/Application.h"
#include "constants/UI.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // Set application metadata
    app.setApplicationName(unoRadar::Constants::Application::NAME);
    app.setApplicationDisplayName(unoRadar::Constants::Application::FULL_NAME);
    app.setApplicationVersion(unoRadar::Constants::Application::VERSION);

    // Create basic window to test
    QMainWindow window;
    window.setWindowTitle(QString("%1 - %2")
        .arg(unoRadar::Constants::Application::FULL_NAME)
        .arg(unoRadar::Constants::Application::VERSION));

    window.resize(unoRadar::Constants::UI::WINDOW_WIDTH,
                  unoRadar::Constants::UI::WINDOW_HEIGHT);

    // Add simple text display
    QTextEdit* display = new QTextEdit(&window);
    display->setReadOnly(true);
    display->setText("unoRadar Frontend Starting...\nWaiting for WebSocket implementation...");
    window.setCentralWidget(display);

    window.show();

    return app.exec();
}