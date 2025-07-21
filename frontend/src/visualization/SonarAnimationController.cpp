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
    m_currentAngle = MIN_ANGLE;
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

    // Calculate angle change based on time and speed
    const double deltaSeconds = static_cast<double>(deltaTime) / 1000.0;
    const double deltaAngle = m_sweepSpeed * deltaSeconds;

    // Update angle based on direction
    double newAngle = static_cast<double>(m_currentAngle);

    if (m_currentDirection == SweepDirection::FORWARD) {
        newAngle += deltaAngle;

        // Check if we've reached the end
        if (newAngle >= static_cast<double>(MAX_ANGLE)) {
            newAngle = static_cast<double>(MAX_ANGLE);
            m_currentDirection = SweepDirection::BACKWARD;
            emit directionChanged(m_currentDirection);
        }
    } else {
        newAngle -= deltaAngle;

        // Check if we've reached the beginning
        if (newAngle <= static_cast<double>(MIN_ANGLE)) {
            newAngle = static_cast<double>(MIN_ANGLE);
            m_currentDirection = SweepDirection::FORWARD;
            emit directionChanged(m_currentDirection);
            emit sweepCycleCompleted();
        }
    }

    // Update current angle
    const std::uint16_t roundedAngle = static_cast<std::uint16_t>(std::round(newAngle));
    if (roundedAngle != m_currentAngle) {
        m_currentAngle = roundedAngle;
        emit angleChanged(m_currentAngle);
    }
}

} // namespace visualization
} // namespace siren