#ifndef SIREN_SONAR_VISUALIZATION_WIDGET_H
#define SIREN_SONAR_VISUALIZATION_WIDGET_H

// SIREN Military-Grade Sonar System
// Sonar Visualization Widget - Single Responsibility: Render Sonar Display ONLY
// Compliant with MISRA C++ 2023, SRP, SSOT

#include <QWidget>
#include <QPaintEvent>
#include <QResizeEvent>
#include <memory>
#include "data/SonarDataParser.h"

namespace siren {

// Forward declarations
namespace visualization {
    class PolarCoordinateConverter;
    class SonarDataBuffer;
    class SonarAnimationController;
}

namespace ui {

/**
 * @brief Sonar visualization widget - Single Responsibility: Render Display
 * 
 * This widget has ONE job: Paint the sonar display using data from other components.
 * It does NOT manage data, control animation, or convert coordinates.
 * 
 * Features:
 * - Delegates coordinate conversion to PolarCoordinateConverter
 * - Delegates data storage to SonarDataBuffer
 * - Delegates animation timing to SonarAnimationController
 * - Only handles painting and Qt events
 * 
 * MISRA C++ Compliance:
 * - Rule 12.4.1: No dynamic allocation after initialization
 * - Rule 21.2.1: RAII for all resources
 * - Rule 5.0.1: No magic numbers (uses constants)
 */
class SonarVisualizationWidget final : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief Construct sonar visualization widget
     * @param parent Parent widget (Qt memory management)
     */
    explicit SonarVisualizationWidget(QWidget* parent = nullptr);

    /**
     * @brief Destructor - RAII cleanup
     */
    ~SonarVisualizationWidget() override;

    // MISRA C++ Rule 12.1.1: Disable copy/move for Qt widgets
    SonarVisualizationWidget(const SonarVisualizationWidget&) = delete;
    SonarVisualizationWidget& operator=(const SonarVisualizationWidget&) = delete;
    SonarVisualizationWidget(SonarVisualizationWidget&&) = delete;
    SonarVisualizationWidget& operator=(SonarVisualizationWidget&&) = delete;

public slots:
    /**
     * @brief Update with new sonar data
     * @param sonarData New data point to display
     */
    void updateSonarData(const data::SonarDataPoint& sonarData);

    /**
     * @brief Clear all data from display
     */
    void clearDisplay();

    /**
     * @brief Enable or disable sweep animation
     * @param enabled True to enable animation
     */
    void setAnimationEnabled(bool enabled);

protected:
    /**
     * @brief Paint event handler
     * @param event Paint event
     */
    void paintEvent(QPaintEvent* event) override;

    /**
     * @brief Resize event handler
     * @param event Resize event
     */
    void resizeEvent(QResizeEvent* event) override;

private:
    /**
     * @brief Initialize widget components
     */
    void initializeComponents();

    /**
     * @brief Update display geometry after resize
     */
    void updateDisplayGeometry();

    /**
     * @brief Draw background
     * @param painter QPainter instance
     */
    void drawBackground(QPainter& painter) const;

    /**
     * @brief Draw polar grid
     * @param painter QPainter instance
     */
    void drawPolarGrid(QPainter& painter) const;

    /**
     * @brief Draw distance rings
     * @param painter QPainter instance
     */
    void drawDistanceRings(QPainter& painter) const;

    /**
     * @brief Draw angle lines
     * @param painter QPainter instance
     */
    void drawAngleLines(QPainter& painter) const;

    /**
     * @brief Draw scale labels
     * @param painter QPainter instance
     */
    void drawScaleLabels(QPainter& painter) const;

    /**
     * @brief Draw data points
     * @param painter QPainter instance
     */
    void drawDataPoints(QPainter& painter) const;

    /**
     * @brief Draw sweep line
     * @param painter QPainter instance
     */
    void drawSweepLine(QPainter& painter) const;

    /**
     * @brief Draw title
     * @param painter QPainter instance
     */
    void drawTitle(QPainter& painter) const;

    // Component ownership (using unique_ptr for RAII)
    std::unique_ptr<visualization::PolarCoordinateConverter> m_coordinateConverter;
    std::unique_ptr<visualization::SonarDataBuffer> m_dataBuffer;
    std::unique_ptr<visualization::SonarAnimationController> m_animationController;

    // Display geometry
    QPoint m_centerPoint{0, 0};
    int m_displayRadius{0};
    bool m_geometryValid{false};
};

} // namespace ui
} // namespace siren

#endif // SIREN_SONAR_VISUALIZATION_WIDGET_H