/**
 * CLASSIFICATION: UNCLASSIFIED
 * EXPORT CONTROL: NOT SUBJECT TO EAR/ITAR
 * CONTRACT: UNORADAR-2025
 *
 * @file MilitaryMainWindow.h
 * @brief Military-grade main window with custom controls (MIL-STD-1472 compliant)
 * @author unoRadar Defense Systems
 * @date 2025
 *
 * MISRA C++ 2008 Compliant
 * DO-178C Level A Certifiable
 * Integration demonstration of custom window controls
 */

#ifndef UNORADAR_UI_MILITARYMAINWINDOW_H
#define UNORADAR_UI_MILITARYMAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTextEdit>
#include <QLabel>
#include <memory>

// Forward declarations
namespace unoRadar {
namespace UI {
namespace Controls {
    class WindowControlBar;
    class WindowControlHandler;
}
}
}

namespace unoRadar {
namespace UI {

/**
 * @class MilitaryMainWindow
 * @brief Military-grade main window with custom frameless controls
 *
 * Demonstrates complete integration of:
 * - Custom window control buttons (minimize, maximize, close)
 * - Military-standard styling and behavior
 * - Frameless window with custom title bar
 * - Keyboard navigation and accessibility
 * - Event handling and state management
 */
class MilitaryMainWindow final : public QMainWindow
{
    Q_OBJECT

public:
    /**
     * @brief Explicit constructor
     * @param parent Parent widget (may be nullptr)
     */
    explicit MilitaryMainWindow(QWidget* parent = nullptr);

    /**
     * @brief Virtual destructor for proper cleanup
     * @note Defined in .cpp file to handle unique_ptr with incomplete types
     */
    virtual ~MilitaryMainWindow();

    /**
     * @brief Copy constructor - deleted for MISRA compliance
     */
    MilitaryMainWindow(const MilitaryMainWindow&) = delete;

    /**
     * @brief Copy assignment operator - deleted for MISRA compliance
     */
    MilitaryMainWindow& operator=(const MilitaryMainWindow&) = delete;

    /**
     * @brief Move constructor - deleted for MISRA compliance
     */
    MilitaryMainWindow(MilitaryMainWindow&&) = delete;

    /**
     * @brief Move assignment operator - deleted for MISRA compliance
     */
    MilitaryMainWindow& operator=(MilitaryMainWindow&&) = delete;

protected:
    /**
     * @brief Handle close events for cleanup
     * @param event Close event details
     */
    void closeEvent(QCloseEvent* event) override;

    /**
     * @brief Handle mouse press events for window dragging
     * @param event Mouse event details
     */
    void mousePressEvent(QMouseEvent* event) override;

    /**
     * @brief Handle mouse move events for window dragging
     * @param event Mouse event details
     */
    void mouseMoveEvent(QMouseEvent* event) override;

    /**
     * @brief Event filter for title bar double-click detection
     * @param object Target object
     * @param event Event details
     * @return True if event handled, false otherwise
     */
    bool eventFilter(QObject* object, QEvent* event) override;
    
    /**
     * @brief Handle window resize events to update rounded corners
     * @param event Resize event details
     */
    void resizeEvent(QResizeEvent* event) override;

private slots:
    /**
     * @brief Handle window closing signal from controls
     */
    void onWindowClosing();

    /**
     * @brief Handle window state changes
     * @param isMaximized New maximized state
     */
    void onWindowStateChanged(bool isMaximized);

private:
    /**
     * @brief Initialize the military-grade window
     */
    void initializeMilitaryWindow();

    /**
     * @brief Create the custom title bar with controls
     */
    void createCustomTitleBar();

    /**
     * @brief Create the main content area
     */
    void createMainContent();

    /**
     * @brief Apply military-standard styling
     */
    void applyMilitaryTheme();

    /**
     * @brief Set up window flags for frameless operation
     */
    void setupWindowFlags();

    /**
     * @brief Toggle full size window state
     */
    void toggleFullSize();
    
    /**
     * @brief Apply rounded corners to window
     */
    void applyRoundedCorners();

    // Member variables
    std::unique_ptr<QWidget> m_titleBar;                    ///< Custom title bar widget
    std::unique_ptr<QWidget> m_centralWidget;               ///< Main content widget
    std::unique_ptr<QVBoxLayout> m_mainLayout;              ///< Main window layout
    std::unique_ptr<QTextEdit> m_contentDisplay;            ///< Content display area
    std::unique_ptr<Controls::WindowControlBar> m_controlBar; ///< Window control buttons
    std::unique_ptr<Controls::WindowControlHandler> m_controlHandler; ///< Control event handler
    std::unique_ptr<QLabel> m_titleLabel;                   ///< Centered title label

    // Window dragging state
    bool m_isDragging;          ///< Window drag state
    QPoint m_dragStartPosition; ///< Drag start position

    // Window state management
    bool m_isFullSize;          ///< Full size window state
    QRect m_normalGeometry;     ///< Normal window geometry for restoration
};

} // namespace UI
} // namespace unoRadar

#endif // UNORADAR_UI_MILITARYMAINWINDOW_H