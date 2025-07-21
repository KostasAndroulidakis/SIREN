// SIREN Sonar System
// Panel Factory Implementation
// Single Responsibility: Create Panel Widgets ONLY

#include "ui/PanelFactory.h"
#include "constants/LayoutConstants.h"
#include <QLabel>
#include <QVBoxLayout>

namespace siren {
namespace ui {

QFrame* PanelFactory::createPanel(PanelType type, QWidget* parent)
{
    QFrame* panel = new QFrame(parent);

    // Configure basic frame properties
    configureFrameProperties(panel);

    // Set dimensions based on panel type
    configurePanelDimensions(panel, type);

    // Set object name for styling
    switch (type) {
        case PanelType::STATUS:
            panel->setObjectName("statusPanel");
            break;
        case PanelType::CONTROL:
            panel->setObjectName("controlPanel");
            break;
        case PanelType::SONAR:
            panel->setObjectName("sonarPanel");
            break;
        case PanelType::DATA:
            panel->setObjectName("dataPanel");
            break;
        case PanelType::PERFORMANCE:
            panel->setObjectName("performancePanel");
            break;
    }

    return panel;
}

QFrame* PanelFactory::createPlaceholder(const QString& title, QWidget* parent)
{
    QFrame* placeholder = new QFrame(parent);
    configureFrameProperties(placeholder);

    // Create layout for centering text
    QVBoxLayout* layout = new QVBoxLayout(placeholder);
    layout->setContentsMargins(
        constants::layout::PANEL_MARGIN,
        constants::layout::PANEL_MARGIN,
        constants::layout::PANEL_MARGIN,
        constants::layout::PANEL_MARGIN
    );

    // Create centered label
    QLabel* label = new QLabel(title, placeholder);
    label->setAlignment(Qt::AlignCenter);
    label->setObjectName("placeholderLabel");

    layout->addWidget(label);

    return placeholder;
}

QFrame* PanelFactory::createSection(QWidget* parent)
{
    QFrame* section = new QFrame(parent);
    section->setFrameStyle(QFrame::NoFrame);
    section->setObjectName("sectionFrame");

    return section;
}

QFrame* PanelFactory::createGroupBox(QWidget* parent)
{
    QFrame* group = new QFrame(parent);
    configureFrameProperties(group);
    group->setObjectName("groupBox");

    return group;
}

void PanelFactory::configurePanelDimensions(QFrame* panel, PanelType type)
{
    using namespace constants::layout;

    switch (type) {
        case PanelType::STATUS:
            panel->setFixedHeight(STATUS_PANEL_HEIGHT);
            break;

        case PanelType::CONTROL:
            panel->setFixedWidth(CONTROL_PANEL_WIDTH);
            break;

        case PanelType::DATA:
            panel->setFixedWidth(DATA_PANEL_WIDTH);
            break;

        case PanelType::PERFORMANCE:
            panel->setFixedHeight(PERFORMANCE_PANEL_HEIGHT);
            break;

        case PanelType::SONAR:
            // Sonar panel is flexible size (fills remaining space)
            // No fixed dimensions needed
            break;
    }
}

void PanelFactory::configureFrameProperties(QFrame* frame)
{
    // Set frame shape and shadow (actual styling done by theme)
    frame->setFrameShape(QFrame::Box);
    frame->setFrameShadow(QFrame::Plain);
    frame->setLineWidth(constants::layout::PANEL_BORDER_WIDTH);
}

} // namespace ui
} // namespace siren