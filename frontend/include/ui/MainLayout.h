#ifndef SIREN_MAIN_LAYOUT_H
#define SIREN_MAIN_LAYOUT_H

// SIREN Military-Grade Radar System
// Main Window Layout Manager - ONLY manages widget arrangement
// Compliant with MISRA C++ 2023, SRP, SSOT

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>

namespace siren {
namespace ui {

/**
 * @brief Layout manager for main window - Single Responsibility: Widget Arrangement
 *
 * This class has ONE job: Arrange widgets in the main window layout.
 * It does NOT create widgets, style them, or manage their behavior.
 *
 * Layout Structure:
 * +------------------------------------------+
 * |            Status Widget                 |
 * +----------+----------------------+--------+
 * | Control  |                      | Data   |
 * | Widget   |    Center Widget     | Widget |
 * |          |                      |        |
 * +----------+----------------------+--------+
 * |          Performance Widget              |
 * +------------------------------------------+
 *
 * MISRA C++ Compliance:
 * - Rule 12.4.1: No dynamic allocation after initialization
 * - Rule 21.2.1: RAII for all resources
 * - Rule 5.0.1: No magic numbers
 */
class MainLayout final : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief Construct layout manager
     * @param parent Parent widget (Qt memory management)
     */
    explicit MainLayout(QWidget* parent = nullptr);

    /**
     * @brief Destructor - RAII cleanup
     */
    ~MainLayout() override = default;

    // MISRA C++ Rule 12.1.1: Disable copy/move for Qt widgets
    MainLayout(const MainLayout&) = delete;
    MainLayout& operator=(const MainLayout&) = delete;
    MainLayout(MainLayout&&) = delete;
    MainLayout& operator=(MainLayout&&) = delete;

    /**
     * @brief Set the status widget (top bar)
     * @param widget Widget to place in status position (ownership NOT transferred)
     */
    void setStatusWidget(QWidget* widget);

    /**
     * @brief Set the control widget (left panel)
     * @param widget Widget to place in control position (ownership NOT transferred)
     */
    void setControlWidget(QWidget* widget);

    /**
     * @brief Set the center widget (main display area)
     * @param widget Widget to place in center position (ownership NOT transferred)
     */
    void setCenterWidget(QWidget* widget);

    /**
     * @brief Set the data widget (right panel)
     * @param widget Widget to place in data position (ownership NOT transferred)
     */
    void setDataWidget(QWidget* widget);

    /**
     * @brief Set the performance widget (bottom bar)
     * @param widget Widget to place in performance position (ownership NOT transferred)
     */
    void setPerformanceWidget(QWidget* widget);

private:
    /**
     * @brief Initialize the layout structure
     */
    void initializeLayout();

    // Layout managers (managed by Qt parent-child hierarchy)
    QVBoxLayout* m_mainLayout{nullptr};      // Vertical: status, center, performance
    QHBoxLayout* m_centerLayout{nullptr};    // Horizontal: control, center, data

    // Current widgets in each position (NOT owned by this class)
    QWidget* m_statusWidget{nullptr};
    QWidget* m_controlWidget{nullptr};
    QWidget* m_centerWidget{nullptr};
    QWidget* m_dataWidget{nullptr};
    QWidget* m_performanceWidget{nullptr};
};

} // namespace ui
} // namespace siren

#endif // SIREN_MAIN_LAYOUT_H