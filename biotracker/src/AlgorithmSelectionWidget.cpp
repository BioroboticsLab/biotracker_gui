#include "../AlgorithmSelectionWidget.h"

#include <QString>
#include <QComboBox>

#include "biotracker/Registry.h"

namespace BioTracker {
namespace Gui {

AlgorithmSelectionWidget::AlgorithmSelectionWidget(QWidget *parent,
        Core::BioTrackerApp &facade)
    : QWidget(parent)
    , m_ui(parent)
    , m_biotracker(facade) {
    initConnects();
    initAlgorithmList();

    m_ui.cb_algorithms->setInsertPolicy(
        QComboBox::InsertPolicy::InsertAlphabetically);
}

void AlgorithmSelectionWidget::addTrackingAlgorithm(const Core::TrackerType type) {
    const QString trackerName = QString::fromStdString(
                                    m_biotracker.getRegistry().getStringByType().at(type));
    m_ui.cb_algorithms->addItem(trackerName);
    const int index = m_ui.cb_algorithms->findText(trackerName);
    m_ui.cb_algorithms->setCurrentIndex(index);
}

void AlgorithmSelectionWidget::initAlgorithmList() {
    // add NoTracker first
    m_ui.cb_algorithms->addItem(QString::fromStdString(
                                    m_biotracker.getRegistry().getStringByType().at(Core::NoTracking)));

    // add Trackers
    for (auto &algByStr : m_biotracker.getRegistry().getTypeByString()) {
        if (algByStr.second != Core::NoTracking) {
            m_ui.cb_algorithms->addItem(QString::fromStdString(algByStr.first));
        }
    }
}

void AlgorithmSelectionWidget::initConnects() {
    QObject::connect(&m_biotracker.getRegistry(), &Core::Registry::newTracker,
                     this, &AlgorithmSelectionWidget::addTrackingAlgorithm);
    QObject::connect(m_ui.cb_algorithms, static_cast<void(QComboBox::*)(const QString &)>(&QComboBox::currentIndexChanged),
                     this, &AlgorithmSelectionWidget::trackingAlgorithmSelected);
    QObject::connect(m_ui.chk_enableTracking, &QCheckBox::toggled,
                     this, &AlgorithmSelectionWidget::enableDisableTracking);
}

void AlgorithmSelectionWidget::trackingAlgorithmSelected(const QString &name) {
    m_biotracker.setTrackingAlgorithm(name.toStdString());

    // check if we have "any" tracking or not
    auto noTrackingStr = QString::fromStdString(m_biotracker.getRegistry().getStringByType().at(Core::NoTracking));
    if (name.compare(noTrackingStr) == 0) {
        m_ui.chk_enableTracking->setChecked(true);
        m_ui.chk_enableTracking->setEnabled(false);
    } else {
        m_ui.chk_enableTracking->setEnabled(true);
    }
}

void AlgorithmSelectionWidget::enableDisableTracking(bool checked) {
    if (checked) {
        m_biotracker.enableTracking();
    } else {
        m_biotracker.disableTracking();
    }
}

}
}
