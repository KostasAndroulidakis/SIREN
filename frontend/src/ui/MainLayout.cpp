// SIREN Military-Grade Radar System
// Main Window Layout Manager Implementation
// Single Responsibility: Widget Arrangement ONLY

#include "ui/MainLayout.h"

namespace siren {
namespace ui {

MainLayout::MainLayout(QWidget* parent)
    : QWidget(parent)
    , m_mainLayout(nullptr)
    , m_centerLayout(nullptr)
    , m_statusWidget(nullptr)
    , m_controlWidget(nullptr)
    , m_centerWidget(nullptr)
    , m_dataWidget(nullptr)
    , m_performanceWidget(nullptr)
{
    initializeLayout();
}

void MainLayout::initializeLayout()
{
    // Create main vertical layout
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    m_mainLayout->setSpacing(0);

    // Create center horizontal layout
    m_centerLayout = new QHBoxLayout();
    m_centerLayout->setContentsMargins(0, 0, 0, 0);
    m_centerLayout->setSpacing(0);

    // Add center layout to main layout
    // Structure: status (top), center area, performance (bottom)
    m_mainLayout->addLayout(m_centerLayout, 1); // Stretch factor 1 for center area
}

void MainLayout::setStatusWidget(QWidget* widget)
{
    if (m_statusWidget != nullptr) {
        m_mainLayout->removeWidget(m_statusWidget);
        m_statusWidget->setParent(nullptr);
    }

    m_statusWidget = widget;

    if (widget != nullptr) {
        // Insert at position 0 (top)
        m_mainLayout->insertWidget(0, widget, 0); // No stretch
    }
}

void MainLayout::setControlWidget(QWidget* widget)
{
    if (m_controlWidget != nullptr) {
        m_centerLayout->removeWidget(m_controlWidget);
        m_controlWidget->setParent(nullptr);
    }

    m_controlWidget = widget;

    if (widget != nullptr) {
        // Insert at position 0 (left)
        m_centerLayout->insertWidget(0, widget, 0); // No stretch
    }
}

void MainLayout::setCenterWidget(QWidget* widget)
{
    if (m_centerWidget != nullptr) {
        m_centerLayout->removeWidget(m_centerWidget);
        m_centerWidget->setParent(nullptr);
    }

    m_centerWidget = widget;

    if (widget != nullptr) {
        // Add to center with stretch
        int pos = (m_controlWidget != nullptr) ? 1 : 0;
        m_centerLayout->insertWidget(pos, widget, 1); // Stretch factor 1
    }
}

void MainLayout::setDataWidget(QWidget* widget)
{
    if (m_dataWidget != nullptr) {
        m_centerLayout->removeWidget(m_dataWidget);
        m_dataWidget->setParent(nullptr);
    }

    m_dataWidget = widget;

    if (widget != nullptr) {
        // Add to right (end of layout)
        m_centerLayout->addWidget(widget, 0); // No stretch
    }
}

void MainLayout::setPerformanceWidget(QWidget* widget)
{
    if (m_performanceWidget != nullptr) {
        m_mainLayout->removeWidget(m_performanceWidget);
        m_performanceWidget->setParent(nullptr);
    }

    m_performanceWidget = widget;

    if (widget != nullptr) {
        // Add at bottom
        m_mainLayout->addWidget(widget, 0); // No stretch
    }
}

} // namespace ui
} // namespace siren