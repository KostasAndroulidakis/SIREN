#ifndef SIREN_SONAR_ANIMATION_CONTROLLER_H
#define SIREN_SONAR_ANIMATION_CONTROLLER_H

// SIREN Sonar System
// Sonar Animation Controller - Single Responsibility: Sweep Animation Timing ONLY
// Compliant with MISRA C++ 2023, SRP, SSOT

#include <QObject>
#include <QTimer>
#include <cstdint>

namespace siren {
namespace visualization {

/**
 * @brief Sonar animation controller - Single Responsibility: Animation Timing
 *
 * This class has ONE job: Control the sweep animation timing and angle updates.
 * It does NOT render, store data, or handle UI events.
 *
 * Features:
 * - Bidirectional sweep animation (0° → 180° → 0°)
 * - Configurable sweep speed
 * - Frame rate independent animation
 * - Pause/resume functionality
 *
 * MISRA C++ Compliance:
 * - Rule 12.4.1: No dynamic allocation
 * - Rule 21.2.1: RAII for timer resource
 * - Rule 5.0.1: No magic numbers
 */
class SonarAnimationController final : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Sweep direction enumeration
     */
    enum class SweepDirection {
        FORWARD,   // 0° to 180°
        BACKWARD   // 180° to 0°
    };

    /**
     * @brief Construct animation controller
     * @param parent Parent object (Qt memory management)
     */
    explicit SonarAnimationController(QObject* parent = nullptr);

    /**
     * @brief Destructor - RAII cleanup
     */
    ~SonarAnimationController() override = default;

    // MISRA C++ Rule 12.1.1: Disable copy/move
    SonarAnimationController(const SonarAnimationController&) = delete;
    SonarAnimationController& operator=(const SonarAnimationController&) = delete;
    SonarAnimationController(SonarAnimationController&&) = delete;
    SonarAnimationController& operator=(SonarAnimationController&&) = delete;

    /**
     * @brief Start animation
     */
    void start();

    /**
     * @brief Stop animation
     */
    void stop();

    /**
     * @brief Pause animation
     */
    void pause();

    /**
     * @brief Resume animation
     */
    void resume();

    /**
     * @brief Reset to starting position
     */
    void reset();

    /**
     * @brief Set sweep speed
     * @param degreesPerSecond Sweep speed in degrees per second
     */
    void setSweepSpeed(double degreesPerSecond);

    /**
     * @brief Get current sweep angle
     * @return Current angle in degrees (0-180)
     */
    [[nodiscard]] std::uint16_t getCurrentAngle() const { return m_currentAngle; }

    /**
     * @brief Get current sweep direction
     * @return Current direction
     */
    [[nodiscard]] SweepDirection getCurrentDirection() const { return m_currentDirection; }

    /**
     * @brief Check if animation is running
     * @return True if animating
     */
    [[nodiscard]] bool isAnimating() const { return m_isAnimating; }

    /**
     * @brief Get sweep speed
     * @return Speed in degrees per second
     */
    [[nodiscard]] double getSweepSpeed() const { return m_sweepSpeed; }

    /**
     * @brief Synchronize cursor with actual servo position
     * @param servoAngle Current servo angle in degrees from backend
     * 
     * This method synchronizes the display cursor with the real servo motor position.
     * It overrides any simulated animation and tracks the actual hardware position.
     */
    void syncWithServoPosition(std::uint16_t servoAngle);

signals:
    /**
     * @brief Emitted when sweep angle changes
     * @param angle New angle in degrees
     */
    void angleChanged(std::uint16_t angle);

    /**
     * @brief Emitted when sweep direction changes
     * @param direction New direction
     */
    void directionChanged(SweepDirection direction);

    /**
     * @brief Emitted when sweep completes a full cycle
     */
    void sweepCycleCompleted();

private slots:
    /**
     * @brief Update animation frame
     */
    void updateAnimation();

private:
    // Animation state
    std::uint16_t m_currentAngle{5};      // Start at servo minimum
    std::uint16_t m_targetAngle{5};       // Target angle for smooth interpolation
    SweepDirection m_currentDirection{SweepDirection::FORWARD};
    bool m_isAnimating{false};
    bool m_isInterpolating{false};        // Smooth movement to target

    // Timing
    QTimer* m_animationTimer{nullptr};
    std::uint64_t m_lastUpdateTime{0};
    double m_sweepSpeed{DEFAULT_SWEEP_SPEED};

    // Animation constants (SSOT)
    static constexpr int ANIMATION_FPS = 60;
    static constexpr int ANIMATION_INTERVAL_MS = 1000 / ANIMATION_FPS;
    static constexpr double DEFAULT_SWEEP_SPEED = 45.0;  // degrees per second
    // Display range (full semicircle shown)
    static constexpr std::uint16_t DISPLAY_MIN_ANGLE = 0;
    static constexpr std::uint16_t DISPLAY_MAX_ANGLE = 180;
    
    // Smooth interpolation
    static constexpr double INTERPOLATION_SPEED = 120.0;  // degrees per second for smooth tracking
};

} // namespace visualization
} // namespace siren

#endif // SIREN_SONAR_ANIMATION_CONTROLLER_H