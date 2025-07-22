#ifndef SIREN_PANEL_FACTORY_H
#define SIREN_PANEL_FACTORY_H

// SIREN Sonar System
// Panel Factory - Single Responsibility: Create Panel Widgets
// Compliant with MISRA C++ 2023, SRP, SSOT

#include <QFrame>
#include <QString>

namespace siren {
namespace ui {

/**
 * @brief Factory for creating panel widgets - Single Responsibility: Panel Creation
 *
 * This class has ONE job: Create panel widgets with proper structure.
 * It does NOT style panels, manage their lifecycle, or handle their behavior.
 * Styling is handled by Theme, behavior by individual panel classes.
 *
 * MISRA C++ Compliance:
 * - Rule 12.4.1: No dynamic allocation after initialization
 * - Rule 21.2.1: RAII for all resources
 * - Rule 5.0.1: No magic numbers (uses LayoutConstants)
 */
class PanelFactory final
{
public:
    /**
     * @brief Panel types that can be created
     */
    enum class PanelType {
        STATUS,      // Top status bar
        CONTROL,     // Left control panel
        SONAR,       // Center sonar display
        DATA,        // Right data panel
        PERFORMANCE  // Bottom performance metrics
    };

    /**
     * @brief Create a panel frame with proper structure
     * @param type Type of panel to create
     * @param parent Parent widget (for Qt memory management)
     * @return Configured QFrame ready for content (ownership transferred to caller)
     */
    [[nodiscard]] static QFrame* createPanel(PanelType type, QWidget* parent = nullptr);

    /**
     * @brief Create a placeholder panel for development
     * @param title Text to display in placeholder
     * @param parent Parent widget (for Qt memory management)
     * @return Basic QFrame with centered text (ownership transferred to caller)
     */
    [[nodiscard]] static QFrame* createPlaceholder(const QString& title, QWidget* parent = nullptr);

    /**
     * @brief Create a section frame within a panel
     * @param parent Parent widget (for Qt memory management)
     * @return QFrame configured as a section container (ownership transferred to caller)
     */
    [[nodiscard]] static QFrame* createSection(QWidget* parent = nullptr);

    /**
     * @brief Create a group box frame for related controls
     * @param parent Parent widget (for Qt memory management)
     * @return QFrame configured as a group container (ownership transferred to caller)
     */
    [[nodiscard]] static QFrame* createGroupBox(QWidget* parent = nullptr);

private:
    // Static class - no instantiation
    PanelFactory() = delete;
    ~PanelFactory() = delete;
    PanelFactory(const PanelFactory&) = delete;
    PanelFactory& operator=(const PanelFactory&) = delete;

    /**
     * @brief Configure panel dimensions based on type
     * @param panel Panel to configure
     * @param type Type of panel
     */
    static void configurePanelDimensions(QFrame* panel, PanelType type);

    /**
     * @brief Set frame properties for consistent appearance
     * @param frame Frame to configure
     */
    static void configureFrameProperties(QFrame* frame);
};

} // namespace ui
} // namespace siren

#endif // SIREN_PANEL_FACTORY_H