#include "../Gui.h"

#include <vector>

#include <boost/filesystem.hpp>

#include <QAction>
#include <QFileDialog>

#include <QShortcut>
#include <QKeySequence>
#include <QListWidget>
#include "opencv2/highgui/highgui.hpp"

namespace BioTracker {
namespace Gui {

Gui::Gui()
    : QObject()
    , m_biotracker()
    , m_mainWindow(m_biotracker) {
    initConnects();

    m_mainWindow.show();
}

void Gui::initConnects() {
    //File -> Open Video
    QObject::connect(m_mainWindow.getUi().actionOpen_Video, &QAction::triggered,
                     this, &Gui::browseVideo);
    QObject::connect(m_mainWindow.getUi().actionOpen_Picture, &QAction::triggered,
                     this, &Gui::browsePictures);
    QObject::connect(m_mainWindow.getUi().actionOpen_Camera, &QAction::triggered,
                     this, &Gui::browseCameras);

    QObject::connect(m_mainWindow.getUi().actionLoad_Tracker, &QAction::triggered,
                     this, &Gui::loadTracker);

    QObject::connect(m_mainWindow.getUi().actionLoad_tracking_data,
                     &QAction::triggered, this, &Gui::loadTrackingData);
    QObject::connect(m_mainWindow.getUi().actionSave_tracking_data,
                     &QAction::triggered, this, &Gui::storeTrackingData);

    QObject::connect(m_mainWindow.getUi().actionQuit, &QAction::triggered, this,
                     &Gui::exit);

    QObject::connect(&m_biotracker, &Core::BioTrackerApp::frameCalculated,
                     m_mainWindow.getVideoControl(), &VideoControlWidget::frameCalculated);
    QObject::connect(&m_biotracker, &Core::BioTrackerApp::frameCalculated,
                     &m_mainWindow, &MainWindow::frameCalculated);

    QObject::connect(&m_biotracker, &Core::BioTrackerApp::trackerSelected,
                     &m_mainWindow, &MainWindow::trackerSelected);

    QObject::connect(&m_biotracker, &Core::BioTrackerApp::notify,
                     &m_mainWindow.getNotification(), &NotificationWidget::notify);

    QObject::connect(&m_biotracker, &Core::BioTrackerApp::fileOpened,
                     m_mainWindow.getVideoControl(), &VideoControlWidget::fileOpened);
}

void Gui::browseVideo() {
    static const QString videoFilter("Video files (*.avi *.wmv *.mp4 *.mkv *.mov)");
    std::vector<boost::filesystem::path> files;

    const QString filename = QFileDialog::getOpenFileName(&m_mainWindow,
                             "Open video", "", videoFilter);

    if (!filename.isEmpty()) {
        m_biotracker.openVideo(boost::filesystem::path(filename.toStdString()));
        m_mainWindow.getVideoControl()->updateWidgets();
    }
}

void Gui::browsePictures() {
    static const QString imageFilter(
        "image files (*.png *.jpg *.jpeg *.gif *.bmp *.jpe *.ppm *.tiff *.tif *.sr *.ras *.pbm *.pgm *.jp2 *.dib)");

    std::vector<boost::filesystem::path> files;
    for (QString const &path : QFileDialog::getOpenFileNames(&m_mainWindow,
            "Open image files", "", imageFilter)) {
        files.push_back(boost::filesystem::path(path.toStdString()));
    }

    if (!files.empty()) {
        m_biotracker.openImages(files);
        m_mainWindow.getVideoControl()->updateWidgets();
    }
}

void Gui::browseCameras() {
    OpenCameraDialog &cameraDialog = m_mainWindow.getOpenCameraDialog();
    // Preparing list widget
    QListWidget *cameraListWidget = cameraDialog.getUi().cameraList;
    cameraListWidget->clear();

    BioTracker::Core::Settings settings = m_biotracker.getSettings();
    BioTracker::Core::TrackerStatus trackerStatus = m_biotracker.getTrackingThread().getStatus();
    // Add Item for each camera
    cv::VideoCapture vcap;
    boost::optional<uint8_t> mediaTypeOpt = settings.maybeGetValueOfParam<uint8_t>(GuiParam::MEDIA_TYPE);
    GuiParam::MediaType mediaType = mediaTypeOpt ? static_cast<GuiParam::MediaType>(*mediaTypeOpt) : GuiParam::MediaType::NoMedia;
    boost::optional<int> camIdOpt = settings.maybeGetValueOfParam<int>(CaptureParam::CAP_CAMERA_ID);
    int camId = camIdOpt ? *camIdOpt : -1;
    for (int i = 0; i <= 254; i++) {
        QListWidgetItem* item = new QListWidgetItem(QString("Camera ") + QString::number(static_cast<int>(i)));
        if (mediaType == GuiParam::MediaType::Camera &&  camId == i && trackerStatus != BioTracker::Core::TrackerStatus::NothingLoaded) {
            item->setText(item->text() + QString(" (in use)"));
            item->setFlags(Qt::NoItemFlags);
        } else {
            vcap = cv::VideoCapture(i);
            if (!vcap.isOpened()) {
                break;
            }
            vcap.release();
        }
        cameraListWidget->addItem(item);
    }

    if (cameraDialog.exec() == QDialog::Accepted) {
        // Getting chosen
        int row = cameraListWidget->currentRow();
        if (row >= 0) {
            m_biotracker.openCamera(row);
            m_mainWindow.getVideoControl()->updateWidgets();
        }
    }
}

void Gui::loadTracker() {
    // TODO make this platform dependend (dlls only for windows, so`s only for linux etc)
    static const QString trackerFilter("BioTracker algorithms (*.tracker.dll *.tracker.so *.tracker.zmq)");

    const QString path = QFileDialog::getOpenFileName(&m_mainWindow, "Load Tracker",
                         "", trackerFilter);
    if (!path.isEmpty()) {
        boost::filesystem::path libraryPath(path.toStdString());
        m_biotracker.getRegistry().loadTrackerLibrary(libraryPath);
    }
}

void Gui::loadTrackingData() {

}

void Gui::storeTrackingData() {

}

void Gui::exit() {

}

} // Gui
} // BioTracker
