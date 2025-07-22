// SIREN Sonar System
// Sonar Animation Controller Implementation
// Single Responsibility: Sweep Animation Timing ONLY

#include "visualization/SonarAnimationController.h"
#include <QDateTime>
#include <cmath>

namespace siren {
namespace visualization {

SonarAnimationController::SonarAnimationController(QObject* parent)
    : QObject(parent)
    , m_animationTimer(new QTimer(this))
{
    // Configure animation timer
    m_animationTimer->setInterval(ANIMATION_INTERVAL_MS);
    m_animationTimer->setTimerType(Qt::PreciseTimer);  // For smooth animation

    // Connect timer to update slot
    connect(m_animationTimer, &QTimer::timeout, this, &SonarAnimationController::updateAnimation);
}

void SonarAnimationController::start()
{
    if (!m_isAnimating) {
        m_isAnimating = true;
        m_lastUpdateTime = QDateTime::currentMSecsSinceEpoch();
        m_animationTimer->start();
    }
}

void SonarAnimationController::stop()
{
    m_isAnimating = false;
    m_animationTimer->stop();
    reset();
}

void SonarAnimationController::pause()
{
    m_isAnimating = false;
    m_animationTimer->stop();
}

void SonarAnimationController::resume()
{
    if (!m_isAnimating) {
        m_isAnimating = true;
        m_lastUpdateTime = QDateTime::currentMSecsSinceEpoch();
        m_animationTimer->start();
    }
}

void SonarAnimationController::reset()
{
    m_currentAngle = DISPLAY_MIN_ANGLE;  // Start at display minimum
    m_targetAngle = DISPLAY_MIN_ANGLE;   // Reset target too
    m_isInterpolating = false;           // Stop any interpolation
    m_currentDirection = SweepDirection::FORWARD;
    emit angleChanged(m_currentAngle);
    emit directionChanged(m_currentDirection);
}

void SonarAnimationController::setSweepSpeed(double degreesPerSecond)
{
    // Ensure positive speed
    m_sweepSpeed = std::max(1.0, degreesPerSecond);
}

void SonarAnimationController::updateAnimation()
{
    if (!m_isAnimating) {
        return;
    }

    // Calculate time delta
    const std::uint64_t currentTime = QDateTime::currentMSecsSinceEpoch();
    const std::uint64_t deltaTime = currentTime - m_lastUpdateTime;
    m_lastUpdateTime = currentTime;

    // Calculate time delta
    const double deltaSeconds = static_cast<double>(deltaTime) / 1000.0;
    double newAngle = static_cast<double>(m_currentAngle);

    if (m_isInterpolating) {
        // Smooth interpolation to real servo position
        const double targetAngle = static_cast<double>(m_targetAngle);
        const double angleDifference = targetAngle - newAngle;
        
        if (std::abs(angleDifference) > 0.5) {  // Not close enough yet
            const double moveDistance = INTERPOLATION_SPEED * deltaSeconds;
            if (angleDifference > 0) {
                newAngle += std::min(moveDistance, angleDifference);
            } else {
                newAngle += std::max(-moveDistance, angleDifference);
            }
        } else {
            // Close enough, snap to target
            newAngle = targetAngle;
            m_isInterpolating = false;
        }
    } else {
        // Simulated sweep animation (when not synced to real servo)
        const double deltaAngle = m_sweepSpeed * deltaSeconds;
        
        if (m_currentDirection == SweepDirection::FORWARD) {
            newAngle += deltaAngle;
            
            // Check if we've reached the display maximum
            if (newAngle >= static_cast<double>(DISPLAY_MAX_ANGLE)) {
                newAngle = static_cast<double>(DISPLAY_MAX_ANGLE);
                m_currentDirection = SweepDirection::BACKWARD;
                emit directionChanged(m_currentDirection);
            }
        } else {
            newAngle -= deltaAngle;
            
            // Check if we've reached the display minimum
            if (newAngle <= static_cast<double>(DISPLAY_MIN_ANGLE)) {
                newAngle = static_cast<double>(DISPLAY_MIN_ANGLE);
                m_currentDirection = SweepDirection::FORWARD;
                emit directionChanged(m_currentDirection);
                emit sweepCycleCompleted();
            }
        }
    }

    // Update current angle
    const std::uint16_t roundedAngle = static_cast<std::uint16_t>(std::round(newAngle));
    if (roundedAngle != m_currentAngle) {
        m_currentAngle = roundedAngle;
        emit angleChanged(m_currentAngle);
    }

    // Animation timer keeps running as long as we're animating or interpolating
    // It will be stopped explicitly when stop() is called
}

void SonarAnimationController::syncWithServoPosition(std::uint16_t servoAngle)
{
    // Accept actual servo angle from backend without clamping
    // Backend/firmware handles its own safety limits
    
    // Update direction based on angle change
    if (servoAngle != m_currentAngle) {
        if (servoAngle > m_currentAngle) {
            if (m_currentDirection != SweepDirection::FORWARD) {
                m_currentDirection = SweepDirection::FORWARD;
                emit directionChanged(m_currentDirection);
            }
        } else if (servoAngle < m_currentAngle) {
            if (m_currentDirection != SweepDirection::BACKWARD) {
                m_currentDirection = SweepDirection::BACKWARD;
                emit directionChanged(m_currentDirection);
            }
        }

        // Immediately snap to actual servo position
        m_currentAngle = servoAngle;
        m_targetAngle = servoAngle;
        m_isInterpolating = false;
        
        // Emit immediate change
        emit angleChanged(m_currentAngle);
    }
}

} // namespace visualization
} // namespace siren