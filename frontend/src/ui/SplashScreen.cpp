// SIREN Military-Grade Radar System
// Splash Screen Implementation
// Compliant with MISRA C++ 2023, Qt6 Best Practices, MIL-STD-1472

#include "ui/SplashScreen.h"
#include <QApplication>
#include <QScreen>
#include <QFont>
#include <QFontMetrics>
#include <QPainter>
#include <QStyleOption>
#include <QDebug>

namespace siren {
namespace ui {

SplashScreen::SplashScreen(QWidget* parent)
    : QSplashScreen(QPixmap(), Qt::WindowStaysOnTopHint)
    , m_progressBar(nullptr)
    , m_logoLabel(nullptr)
    , m_versionLabel(nullptr)
    , m_statusLabel(nullptr)
    , m_layout(nullptr)
    , m_centralWidget(nullptr)
    , m_progressTimer(nullptr)
    , m_finishTimer(nullptr)
    , m_currentProgress(0)
    , m_logoLoaded(false)
    , m_initializationComplete(false)
{
    // MISRA C++ Rule 21.2.1: RAII initialization
    // Initialize all components in constructor
    m_initializationComplete = initializeUI();
    
    if (m_initializationComplete) {
        m_logoLoaded = loadLogo();
        applyMilitaryTheme();
        
        if (!m_logoLoaded) {
            createFallbackDisplay();
        }
    }
}

bool SplashScreen::initializeUI()
{
    try {
        // Set fixed size for deterministic display (MISRA C++ Rule 5.0.1)
        setFixedSize(SPLASH_WIDTH, SPLASH_HEIGHT);
        
        // Create central widget for layout management
        m_centralWidget = new QWidget(this);
        m_centralWidget->setGeometry(0, 0, SPLASH_WIDTH, SPLASH_HEIGHT);
        
        // Create main layout
        m_layout = new QVBoxLayout(m_centralWidget);
        m_layout->setContentsMargins(40, 40, 40, 40);
        m_layout->setSpacing(20);
        
        // Title label removed - logo speaks for itself
        
        // Create version label
        m_versionLabel = new QLabel(APPLICATION_VERSION, m_centralWidget);
        m_versionLabel->setAlignment(Qt::AlignCenter);
        m_versionLabel->setObjectName("versionLabel");
        
        // Create status label
        m_statusLabel = new QLabel("Initializing System...", m_centralWidget);
        m_statusLabel->setAlignment(Qt::AlignCenter);
        m_statusLabel->setObjectName("statusLabel");
        
        // Create progress bar
        m_progressBar = new QProgressBar(m_centralWidget);
        m_progressBar->setRange(0, 100);
        m_progressBar->setValue(0);
        m_progressBar->setTextVisible(true);
        m_progressBar->setObjectName("progressBar");
        
        // Create logo label (will be populated in loadLogo)
        m_logoLabel = new QLabel(m_centralWidget);
        m_logoLabel->setAlignment(Qt::AlignCenter);
        m_logoLabel->setObjectName("logoLabel");
        
        // Add widgets to layout with proper spacing
        m_layout->addStretch(2);  // Top spacing
        m_layout->addWidget(m_logoLabel);  // Logo centered
        m_layout->addStretch(1);  // Spacing after logo
        m_layout->addWidget(m_versionLabel);  // Keep version info
        m_layout->addStretch(1);  // Middle spacing
        m_layout->addWidget(m_statusLabel);
        m_layout->addWidget(m_progressBar);
        m_layout->addStretch(2);  // Bottom spacing
        
        // Create timers with proper parent for automatic cleanup
        m_progressTimer = new QTimer(this);
        m_finishTimer = new QTimer(this);
        
        // Connect signals (Qt6 new syntax for type safety)
        connect(m_progressTimer, &QTimer::timeout, this, &SplashScreen::updateProgress);
        connect(m_finishTimer, &QTimer::timeout, this, &SplashScreen::finishLoading);
        
        // Configure timers
        m_progressTimer->setInterval(PROGRESS_UPDATE_INTERVAL_MS);
        m_finishTimer->setSingleShot(true);
        m_finishTimer->setInterval(DISPLAY_DURATION_MS);
        
        return true;
        
    } catch (...) {
        // MISRA C++ compliant error handling without exceptions
        qDebug() << "SplashScreen: Failed to initialize UI components";
        return false;
    }
}

bool SplashScreen::loadLogo()
{
    // Load logo from embedded resources (deterministic access)
    QPixmap logoPixmap(LOGO_RESOURCE_PATH);
    
    if (logoPixmap.isNull()) {
        qDebug() << "SplashScreen: Failed to load logo from resources:" << LOGO_RESOURCE_PATH;
        return false;
    }
    
    // Scale logo to appropriate size while maintaining aspect ratio
    constexpr int LOGO_MAX_HEIGHT = 120;
    const QPixmap scaledLogo = logoPixmap.scaledToHeight(LOGO_MAX_HEIGHT, Qt::SmoothTransformation);
    
    // Set logo in the label widget (centered in layout)
    if (m_logoLabel != nullptr) {
        m_logoLabel->setPixmap(scaledLogo);
        m_logoLabel->setAlignment(Qt::AlignCenter);
    }
    
    qDebug() << "SplashScreen: Logo loaded successfully";
    return true;
}

void SplashScreen::applyMilitaryTheme()
{
    // Apply MIL-STD-1472 compliant styling
    const QString militaryStyleSheet = R"(
        SplashScreen {
            background-color: #1a1a1a;
            border: 2px solid #00ff00;
        }
        
        #versionLabel {
            color: #FF4500;
            font-family: 'Courier New', monospace;
            font-size: 14px;
        }
        
        #statusLabel {
            color: #ffffff;
            font-family: 'Courier New', monospace;
            font-size: 12px;
        }
        
        #progressBar {
            background-color: #333333;
            border: 1px solid #00ff00;
            border-radius: 3px;
            text-align: center;
            color: #ffffff;
            font-family: 'Courier New', monospace;
            font-size: 10px;
        }
        
        #progressBar::chunk {
            background-color: #00FFFF;
            border-radius: 2px;
        }
    )";
    
    setStyleSheet(militaryStyleSheet);
}

void SplashScreen::createFallbackDisplay()
{
    // Create fallback text-based logo if image fails to load
    if (m_logoLabel != nullptr) {
        m_logoLabel->setText("SIREN\nLOGO");
        m_logoLabel->setAlignment(Qt::AlignCenter);
        m_logoLabel->setStyleSheet(R"(
            QLabel {
                color: #00ff00;
                font-family: 'Courier New', monospace;
                font-size: 18px;
                font-weight: bold;
                border: 2px solid #00ff00;
                border-radius: 5px;
                padding: 10px;
                background-color: rgba(0, 0, 0, 0.8);
            }
        )");
    }
    qDebug() << "SplashScreen: Using fallback text logo";
}

bool SplashScreen::showSplash()
{
    if (!m_initializationComplete) {
        qDebug() << "SplashScreen: Cannot show - initialization failed";
        emit errorOccurred("Failed to initialize splash screen");
        return false;
    }
    
    // Show splash screen
    show();
    
    // Center splash screen on primary screen (after show() to ensure proper geometry)
    if (QScreen* screen = QApplication::primaryScreen()) {
        const QRect screenGeometry = screen->availableGeometry();
        const int x = screenGeometry.x() + (screenGeometry.width() - width()) / 2;
        const int y = screenGeometry.y() + (screenGeometry.height() - height()) / 2;
        move(x, y);
    }
    
    // Start progress animation and finish timer
    m_progressTimer->start();
    m_finishTimer->start();
    
    // Ensure splash is raised to top and centered
    raise();
    activateWindow();
    
    qDebug() << "SplashScreen: Displayed successfully";
    return true;
}

void SplashScreen::updateProgress()
{
    // Calculate progress based on elapsed time
    m_currentProgress = static_cast<int>((static_cast<double>(m_finishTimer->remainingTime()) / DISPLAY_DURATION_MS) * 100.0);
    m_currentProgress = 100 - m_currentProgress;  // Invert for forward progress
    
    // Clamp to valid range
    if (m_currentProgress < 0) {
        m_currentProgress = 0;
    } else if (m_currentProgress > 100) {
        m_currentProgress = 100;
    }
    
    // Update progress bar
    if (m_progressBar != nullptr) {
        m_progressBar->setValue(m_currentProgress);
    }
    
    // Update status message based on progress
    if (m_statusLabel != nullptr) {
        QString statusMessage;
        if (m_currentProgress < 25) {
            statusMessage = "Initializing Hardware...";
        } else if (m_currentProgress < 50) {
            statusMessage = "Loading Radar Systems...";
        } else if (m_currentProgress < 75) {
            statusMessage = "Establishing Communications...";
        } else if (m_currentProgress < 100) {
            statusMessage = "Finalizing Startup...";
        } else {
            statusMessage = "System Ready";
        }
        m_statusLabel->setText(statusMessage);
    }
}

void SplashScreen::finishLoading()
{
    // Stop progress timer
    if (m_progressTimer != nullptr) {
        m_progressTimer->stop();
    }
    
    // Set final progress
    m_currentProgress = 100;
    if (m_progressBar != nullptr) {
        m_progressBar->setValue(100);
    }
    
    if (m_statusLabel != nullptr) {
        m_statusLabel->setText("System Ready");
    }
    
    // Brief pause to show completion
    QTimer::singleShot(500, this, [this]() {
        hide();
        emit loadingComplete();
        qDebug() << "SplashScreen: Loading sequence completed";
    });
}

} // namespace ui
} // namespace siren