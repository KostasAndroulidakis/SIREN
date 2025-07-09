/**
 * @file main.cpp
 * @brief Minimal Qt6 application for unoRadar frontend
 * @author unoRadar Project
 * @date 2025
 *
 * Minimal Qt6 test application - black window to verify Qt6 installation
 */

#include <QApplication>
#include <QMainWindow>
#include <QPalette>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // Create main window
    QMainWindow window;
    window.setWindowTitle("unoRadar - Military-Grade Radar System");

    // Set black background (military theme)
    QPalette palette = window.palette();
    palette.setColor(QPalette::Window, Qt::black);
    window.setPalette(palette);
    window.setAutoFillBackground(true);

    // Set window size (radar display dimensions)
    window.resize(800, 600);

    // Show window
    window.show();

    return app.exec();
}