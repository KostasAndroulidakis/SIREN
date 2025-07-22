// SIREN Sonar System
// Sonar Visualization Widget Implementation
// Single Responsibility: Render Sonar Display ONLY

#include "ui/SonarVisualizationWidget.h"
#include "visualization/PolarCoordinateConverter.h"
#include "visualization/SonarDataBuffer.h"
#include "visualization/SonarAnimationController.h"
#include "constants/VisualizationConstants.h"
#include <QPainter>
#include <QPen>
#include <QBrush>
#include <QFont>
#include <QDateTime>

namespace siren {
namespace ui {

using namespace constants::visualization;

SonarVisualizationWidget::SonarVisualizationWidget(QWidget* parent)
    : QWidget(parent)
    , m_coordinateConverter(std::make_unique<visualization::PolarCoordinateConverter>())
    , m_dataBuffer(std::make_unique<visualization::SonarDataBuffer>())
    , m_animationController(std::make_unique<visualization::SonarAnimationController>())
{
    initializeComponents();

    // Set sonar display size
    setFixedSize(SONAR_DISPLAY_WIDTH, SONAR_DISPLAY_HEIGHT);

    // Enable painting
    setAttribute(Qt::WA_OpaquePaintEvent);

    // Set background
    setAutoFillBackground(true);
    QPalette pal = palette();
    pal.setColor(QPalette::Window, QColor(colors::BACKGROUND));
    setPalette(pal);
}

SonarVisualizationWidget::~SonarVisualizationWidget() = default;

void SonarVisualizationWidget::initializeComponents()
{
    // Connect animation updates to trigger repaints
    connect(m_animationController.get(), &visualization::SonarAnimationController::angleChanged,
            this, [this](std::uint16_t) { update(); });

    // DO NOT start automatic animation - only use real servo data
    // m_animationController->start();
}

void SonarVisualizationWidget::updateSonarData(const data::SonarDataPoint& sonarData)
{
    if (sonarData.valid) {
        // CRITICAL: Synchronize cursor with actual servo position
        m_animationController->syncWithServoPosition(sonarData.angle);

        // Add data point with current timestamp
        const std::uint64_t timestamp = QDateTime::currentMSecsSinceEpoch();
        m_dataBuffer->addPoint(sonarData, timestamp);

        // Remove expired points
        m_dataBuffer->removeExpiredPoints(timestamp, POINT_LIFETIME_MS);

        // Trigger repaint
        update();
    }
}

void SonarVisualizationWidget::clearDisplay()
{
    m_dataBuffer->clear();
    update();
}

void SonarVisualizationWidget::setAnimationEnabled(bool enabled)
{
    if (enabled) {
        m_animationController->start();
    } else {
        m_animationController->stop();
    }
}

void SonarVisualizationWidget::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event)

    if (!m_geometryValid) {
        return;
    }

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Draw display components in order
    drawBackground(painter);
    drawPolarGrid(painter);
    drawDataPoints(painter);
    drawSweepLine(painter);
    drawScaleLabels(painter);
    drawTitle(painter);
}

void SonarVisualizationWidget::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
    updateDisplayGeometry();
}

void SonarVisualizationWidget::updateDisplayGeometry()
{
    // Calculate display center and radius
    const int width = this->width();
    const int height = this->height();

    // Center at bottom middle of widget
    m_centerPoint = QPoint(width / 2, height - DISPLAY_MARGIN);

    // Calculate radius to fit in available space
    const int maxRadiusX = (width - 2 * DISPLAY_MARGIN) / 2;
    const int maxRadiusY = height - 2 * DISPLAY_MARGIN;
    m_displayRadius = std::min(maxRadiusX, maxRadiusY);

    // Update coordinate converter
    m_coordinateConverter->setCenterPoint(m_centerPoint);
    m_coordinateConverter->setDisplayRadius(m_displayRadius);
    m_coordinateConverter->setMaxDistance(DISPLAY_MAX_DISTANCE);

    m_geometryValid = true;
}

void SonarVisualizationWidget::drawBackground(QPainter& painter) const
{
    // Background is handled by widget palette
}

void SonarVisualizationWidget::drawPolarGrid(QPainter& painter) const
{
    drawDistanceRings(painter);
    drawAngleLines(painter);
}

void SonarVisualizationWidget::drawDistanceRings(QPainter& painter) const
{
    painter.setPen(QPen(QColor(colors::GRID_MINOR), GRID_LINE_WIDTH));

    // Draw concentric distance rings
    for (std::uint16_t distance = DISTANCE_RING_INTERVAL;
         distance <= DISPLAY_MAX_DISTANCE;
         distance += DISTANCE_RING_INTERVAL) {

        // Major ring every 100cm
        if (distance % 100 == 0) {
            painter.setPen(QPen(QColor(colors::GRID_MAJOR), GRID_MAJOR_LINE_WIDTH));
        } else {
            painter.setPen(QPen(QColor(colors::GRID_MINOR), GRID_LINE_WIDTH));
        }

        // Calculate ring radius
        const double scaleFactor = static_cast<double>(m_displayRadius) /
                                 static_cast<double>(DISPLAY_MAX_DISTANCE);
        const int radius = static_cast<int>(distance * scaleFactor);

        // Draw arc (180 degrees)
        painter.drawArc(m_centerPoint.x() - radius,
                       m_centerPoint.y() - radius,
                       2 * radius,
                       2 * radius,
                       0, 180 * 16);  // Qt uses 1/16 degree units
    }
}

void SonarVisualizationWidget::drawAngleLines(QPainter& painter) const
{
    // Draw angle lines across full display range (0-180°)
    for (std::uint16_t angle = 0; angle <= 180; angle += ANGLE_MARKER_INTERVAL) {
        // Major line at 0°, 90°, 180°
        if (angle % 90 == 0) {
            painter.setPen(QPen(QColor(colors::GRID_PRIMARY), GRID_MAJOR_LINE_WIDTH));
        } else {
            painter.setPen(QPen(QColor(colors::GRID_MINOR), GRID_LINE_WIDTH));
        }

        // Draw line from center to edge
        const QPoint edgePoint = m_coordinateConverter->polarToScreen(angle, DISPLAY_MAX_DISTANCE);
        painter.drawLine(m_centerPoint, edgePoint);
    }
}

void SonarVisualizationWidget::drawScaleLabels(QPainter& painter) const
{
    painter.setPen(QColor(colors::SCALE_TEXT));
    QFont font(MONOSPACE_FONT, SCALE_FONT_SIZE);
    painter.setFont(font);

    // Draw distance labels
    for (std::uint16_t distance = 100; distance <= DISPLAY_MAX_DISTANCE; distance += 100) {
        const QPoint labelPoint = m_coordinateConverter->polarToScreen(90, distance);
        const QString label = QString("%1cm").arg(distance);

        const QRect textRect(labelPoint.x() - 30, labelPoint.y() - 10, 60, 20);
        painter.drawText(textRect, Qt::AlignCenter, label);
    }

    // Draw angle labels across full display range (0-180°)
    for (std::uint16_t angle = 0; angle <= 180; angle += ANGLE_MARKER_INTERVAL) {
        const QPoint labelPoint = m_coordinateConverter->polarToScreen(angle, DISPLAY_MAX_DISTANCE + 20);
        const QString label = QString("%1°").arg(angle);

        const QRect textRect(labelPoint.x() - 20, labelPoint.y() - 20, 40, 20);
        painter.drawText(textRect, Qt::AlignCenter, label);
    }
}

void SonarVisualizationWidget::drawDataPoints(QPainter& painter) const
{
    const std::uint64_t currentTime = QDateTime::currentMSecsSinceEpoch();
    const auto& points = m_dataBuffer->getPoints();

    for (const auto& point : points) {
        if (!point.valid) continue;

        // Calculate decay factor for fading
        const double decay = point.getDecayFactor(currentTime, POINT_FADE_START_MS, POINT_LIFETIME_MS);
        if (decay <= 0.0) continue;

        // Convert to screen coordinates
        const QPoint screenPoint = m_coordinateConverter->polarToScreen(point.angle, point.distance);

        // Set color based on age
        QColor pointColor;
        if (decay >= 0.9) {
            pointColor = QColor(colors::DATA_POINT_RECENT);
        } else if (decay >= 0.5) {
            pointColor = QColor(colors::DATA_POINT);
        } else {
            pointColor = QColor(colors::DATA_POINT_FADING);
        }
        pointColor.setAlphaF(decay);

        // Draw point
        painter.setPen(Qt::NoPen);
        painter.setBrush(pointColor);
        painter.drawEllipse(screenPoint, DATA_POINT_SIZE, DATA_POINT_SIZE);
    }
}

void SonarVisualizationWidget::drawSweepLine(QPainter& painter) const
{
    const std::uint16_t sweepAngle = m_animationController->getCurrentAngle();

    // Draw sweep line only (no trailing effect)
    painter.setPen(QPen(QColor(colors::SWEEP_LINE), SWEEP_LINE_WIDTH));
    const QPoint sweepEnd = m_coordinateConverter->polarToScreen(sweepAngle, DISPLAY_MAX_DISTANCE);
    painter.drawLine(m_centerPoint, sweepEnd);
}

void SonarVisualizationWidget::drawTitle(QPainter& painter) const
{
    painter.setPen(QColor(colors::TEXT_PRIMARY));
    QFont font(SYSTEM_FONT, TITLE_FONT_SIZE, QFont::Bold);
    painter.setFont(font);

    const QString title = "SONAR SWEEP DISPLAY";
    const QRect titleRect(0, 10, width(), 30);
    painter.drawText(titleRect, Qt::AlignCenter, title);
}

} // namespace ui
} // namespace siren