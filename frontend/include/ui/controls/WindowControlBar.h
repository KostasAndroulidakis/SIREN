/**
 * CLASSIFICATION: UNCLASSIFIED
 * EXPORT CONTROL: NOT SUBJECT TO EAR/ITAR
 * CONTRACT: UNORADAR-2025
 *
 * @file WindowControlBar.h
 * @brief Window control bar container widget (MIL-STD-1472 compliant)
 * @author unoRadar Defense Systems
 * @date 2025
 *
 * MISRA C++ 2008 Compliant
 * DO-178C Level A Certifiable
 * Single Responsibility: Container and layout management for window controls
 */

#ifndef UNORADAR_UI_CONTROLS_WINDOWCONTROLBAR_H
#define UNORADAR_UI_CONTROLS_WINDOWCONTROLBAR_H

#include <QWidget>
#include <QHBoxLayout>
#include <memory>

// Forward declarations
namespace unoRadar {
namespace UI {
namespace Controls {
    class MinimizeButton;
    class MaximizeButton;
    class CloseButton;
}
}
}

namespace unoRadar {
namespace UI {
namespace Controls {

/**
 * @class WindowControlBar
 * @brief Container widget for window control buttons
 * 
 * Single Responsibility:
 * - Manages layout and positioning of window control buttons
 * - Provides unified interface for window control operations
 * - Implements proper tab order for keyboard navigation
 * - Handles window state synchronization
 * 
 * MISRA C++ Compliance:
 * - Explicit constructor
 * - Virtual destructor for proper cleanup
 * - No dynamic memory allocation after initialization
 * - Smart pointer usage for automatic resource management
 */
class WindowControlBar final : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief Explicit constructor
     * @param parent Parent widget (may be nullptr)
     */
    explicit WindowControlBar(QWidget* parent = nullptr);

    /**
     * @brief Virtual destructor for proper cleanup
     * @note Defined in .cpp file to handle unique_ptr with incomplete types
     */
    virtual ~WindowControlBar();

    /**
     * @brief Copy constructor - deleted for MISRA compliance
     */
    WindowControlBar(const WindowControlBar&) = delete;

    /**
     * @brief Copy assignment operator - deleted for MISRA compliance
     */
    WindowControlBar& operator=(const WindowControlBar&) = delete;

    /**
     * @brief Move constructor - deleted for MISRA compliance
     */
    WindowControlBar(WindowControlBar&&) = delete;

    /**
     * @brief Move assignment operator - deleted for MISRA compliance
     */
    WindowControlBar& operator=(WindowControlBar&&) = delete;

    /**
     * @brief Update maximize button state based on window state
     * @param isMaximized Current window maximized state
     */
    void updateMaximizedState(bool isMaximized);

    /**
     * @brief Enable or disable all window controls
     * @param enabled Control state
     */
    void setControlsEnabled(bool enabled);

    /**
     * @brief Get current controls enabled state
     * @return true if controls are enabled
     */
    bool areControlsEnabled() const;

signals:
    /**
     * @brief Signal emitted when minimize is requested
     */
    void minimizeRequested();

    /**
     * @brief Signal emitted when maximize is requested
     */
    void maximizeRequested();

    /**
     * @brief Signal emitted when restore is requested
     */
    void restoreRequested();

    /**
     * @brief Signal emitted when close is requested
     */
    void closeRequested();

protected:
    /**
     * @brief Handle paint events for custom styling
     * @param event Paint event details
     */
    void paintEvent(QPaintEvent* event) override;

    /**
     * @brief Handle resize events for layout management
     * @param event Resize event details
     */
    void resizeEvent(QResizeEvent* event) override;

private:
    /**
     * @brief Initialize the control bar and its components
     */
    void initializeControlBar();

    /**
     * @brief Create and configure all control buttons
     */
    void createControlButtons();

    /**
     * @brief Set up the layout for the control bar
     */
    void setupLayout();

    /**
     * @brief Connect button signals to control bar signals
     */
    void connectButtonSignals();

    /**
     * @brief Apply military-standard styling to the control bar
     */
    void applyMilitaryStyle();

    /**
     * @brief Set up proper tab order for keyboard navigation
     */
    void setupTabOrder();

    // Member variables
    std::unique_ptr<QHBoxLayout> m_layout;           ///< Horizontal layout for buttons
    std::unique_ptr<MinimizeButton> m_minimizeButton; ///< Minimize button
    std::unique_ptr<MaximizeButton> m_maximizeButton; ///< Maximize/restore button
    std::unique_ptr<CloseButton> m_closeButton;       ///< Close button
    bool m_controlsEnabled;                          ///< Current controls enabled state
};

} // namespace Controls
} // namespace UI
} // namespace unoRadar

#endif // UNORADAR_UI_CONTROLS_WINDOWCONTROLBAR_H