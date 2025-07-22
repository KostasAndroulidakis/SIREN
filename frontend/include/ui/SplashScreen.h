#ifndef SIREN_SPLASH_SCREEN_H
#define SIREN_SPLASH_SCREEN_H

// SIREN Sonar System
// Splash Screen Component
// Compliant with MISRA C++ 2023, Qt6 Best Practices, MIL-STD-1472

#include <QSplashScreen>
#include <QProgressBar>
#include <QLabel>
#include <QVBoxLayout>
#include <QTimer>
#include <QPixmap>
#include <QWidget>
#include <QMovie>

namespace siren {
namespace ui {

/**
 * @brief Military-grade splash screen for SIREN sonar system startup
 *
 * Features:
 * - Embedded logo display with fallback handling
 * - Animated progress bar with realistic loading simulation
 * - Military-standard styling and typography
 * - Deterministic timing (1-3 second display)
 * - RAII resource management
 * - Error handling without exceptions
 *
 * MISRA C++ Compliance:
 * - Rule 12.4.1: No dynamic allocation after initialization
 * - Rule 21.2.1: RAII for all resources
 * - Rule 5.0.1: No magic numbers (all constants named)
 * - Rule 8.18.2: No assignment operators in conditions
 */
class SplashScreen final : public QSplashScreen
{
    Q_OBJECT

public:
    // MISRA C++ Rule 5.0.1: Named constants instead of magic numbers
    static constexpr int SPLASH_WIDTH = 600;
    static constexpr int SPLASH_HEIGHT = 400;
    static constexpr int DISPLAY_DURATION_MS = 2000;  // 2 seconds
    static constexpr int PROGRESS_UPDATE_INTERVAL_MS = 50;  // 20 FPS
    static constexpr int PROGRESS_STEPS = DISPLAY_DURATION_MS / PROGRESS_UPDATE_INTERVAL_MS;

    /**
     * @brief Construct splash screen with military-grade initialization
     * @param parent Parent widget (Qt memory management)
     */
    explicit SplashScreen(QWidget* parent = nullptr);

    /**
     * @brief Destructor - RAII cleanup
     */
    ~SplashScreen() override = default;

    // MISRA C++ Rule 12.1.1: Disable copy/move for Qt widgets
    SplashScreen(const SplashScreen&) = delete;
    SplashScreen& operator=(const SplashScreen&) = delete;
    SplashScreen(SplashScreen&&) = delete;
    SplashScreen& operator=(SplashScreen&&) = delete;

    /**
     * @brief Show splash screen with progress animation
     * @return true if successfully displayed, false on error
     */
    [[nodiscard]] bool showSplash();

signals:
    /**
     * @brief Emitted when splash screen completes loading sequence
     */
    void loadingComplete();

    /**
     * @brief Emitted when splash screen encounters an error
     * @param errorMessage Description of the error
     */
    void errorOccurred(const QString& errorMessage);

private slots:
    /**
     * @brief Update progress bar during loading simulation
     */
    void updateProgress();

    /**
     * @brief Complete loading sequence and hide splash
     */
    void finishLoading();

private:
    /**
     * @brief Initialize UI components with military styling
     * @return true if successful, false on error
     */
    [[nodiscard]] bool initializeUI();

    /**
     * @brief Load and validate SIREN logo from resources
     * @return true if logo loaded successfully, false on error
     */
    [[nodiscard]] bool loadLogo();

    /**
     * @brief Apply military-standard styling (MIL-STD-1472)
     */
    void applyTheme();

    /**
     * @brief Create fallback display if logo fails to load
     */
    void createFallbackDisplay();

    // UI Components (managed by Qt parent-child hierarchy)
    QProgressBar* m_progressBar{nullptr};
    QLabel* m_logoLabel{nullptr};
    QLabel* m_versionLabel{nullptr};
    QLabel* m_statusLabel{nullptr};
    QVBoxLayout* m_layout{nullptr};
    QWidget* m_centralWidget{nullptr};

    // Animation and timing
    QTimer* m_progressTimer{nullptr};
    QTimer* m_finishTimer{nullptr};

    // State management
    int m_currentProgress{0};
    bool m_logoLoaded{false};
    bool m_initializationComplete{false};

    // Constants for military-grade display
    static constexpr const char* APPLICATION_NAME = "SIREN";
    static constexpr const char* APPLICATION_VERSION = "v1.0.0";
    static constexpr const char* CLASSIFICATION_LEVEL = "UNCLASSIFIED";
    static constexpr const char* LOGO_RESOURCE_PATH = ":/images/images/siren-logo.png";
};

} // namespace ui
} // namespace siren

#endif // SIREN_SPLASH_SCREEN_H