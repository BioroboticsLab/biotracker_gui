#include "../MainWindow.h"

namespace BioTracker {
namespace Gui {

MainWindow::MainWindow(Core::BioTrackerApp &biotracker)
    : m_ui(this)
    , m_algorithmSelection(m_ui.widget_alg, biotracker)
    , m_notification(m_ui.dockWidgetNotificationContents, biotracker)
    , m_openCameraDialog(m_ui.centralWidget, biotracker)
    , m_tools(m_ui.groupBox_tools) {
    initalizeVideoView(biotracker);

    {
        QFile file(QString::fromStdString(ConfigParam::GEOMETRY_FILE.string()));
        if (file.open(QIODevice::ReadOnly)) {
            restoreGeometry(file.readAll());
        }
    }

    {
        QFile file(QString::fromStdString(ConfigParam::STATE_FILE.string()));
        if (file.open(QIODevice::ReadOnly)) {
            restoreState(file.readAll());
        }
    }
}

void MainWindow::initalizeVideoView(Core::BioTrackerApp &biotracker) {
    m_videoView = std::make_unique<VideoView>(m_ui.trackingArea, biotracker);
    m_ui.videoViewLayout->addWidget(m_videoView.get());
    m_videoControl = std::make_unique<VideoControlWidget>(m_ui.videoControls,
                     biotracker, m_videoView.get());
}

void MainWindow::closeEvent(QCloseEvent *event) {
    const auto dialog = QMessageBox::warning(
                            this, "Exit", "Do you really want to close the application?",
                            QMessageBox::Yes | QMessageBox::No);

    if (dialog == QMessageBox::Yes) {
        {
            QFile file(QString::fromStdString(ConfigParam::GEOMETRY_FILE.string()));
            if (file.open(QIODevice::WriteOnly)) {
                file.write(saveGeometry());
            }
        }

        {
            QFile file(QString::fromStdString(ConfigParam::STATE_FILE.string()));
            if (file.open(QIODevice::WriteOnly)) {
                file.write(saveState());
            }
        }

        QMainWindow::closeEvent(event);
    } else {
        event->ignore();
    }
}

void MainWindow::frameCalculated(const size_t, const std::string filename, const double) {
    setWindowTitle("BioTracker [" + QString::fromStdString(Util::stem_filename(filename)) + "]");
}

void MainWindow::trackerSelected(std::shared_ptr<Core::TrackingAlgorithm> tracker) {
    m_tools.addWidget(tracker->getToolsWidget());
    m_ui.groupBox_tools->repaint();
}

} // Gui
} // BioTracker
