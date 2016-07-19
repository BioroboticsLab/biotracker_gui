#include "../Gui.h"

#include <vector>

#include <boost/filesystem.hpp>

#include <QAction>
#include <QFileDialog>

#include <QShortcut>
#include <QKeySequence>
#include <QListWidget>
#include "opencv2/highgui/highgui.hpp"

#include "biotracker/serialization/SerializationData.h"

#include <cereal/types/polymorphic.hpp>
#include <cereal/archives/json.hpp>
#include <cereal/types/vector.hpp>

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
    // OpenCV does not have an API to list camera devices https://github.com/Itseez/opencv/issues/4269
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
    static const QString trackingdataFilter("Trackingdata Files (*.tdat)");

    const QString path = QFileDialog::getOpenFileName(&m_mainWindow, "Load trackingdata", "", trackingdataFilter);
    if (!path.isEmpty()) {
//        printGuiMessage("Restoring tracking data from " + filename, MSGS::NOTIFICATION);
        std::ifstream is(path.toStdString());
        cereal::JSONInputArchive ar(is);

        Core::Serialization::Data sdata;
        ar(sdata);

        if (m_biotracker.getTrackingThread().getTrackerType()){
            assert(m_biotracker.getTrackingThread().getTrackerType());
            const std::string trackerType =
                    Core::Registry::getInstance().getStringByType().at(m_biotracker.getTrackingThread().getTrackerType().get());

            if (sdata.getType() != trackerType) {
                QMessageBox::warning(&m_mainWindow, "Unable to load tracking data",
                                     "Tracker type does not match.");
                return;
            }
        } else {
            // try to automatically select the required tracking algorithm
            const auto& typeByString = Core::Registry::getInstance().getTypeByString();
            const auto it = typeByString.find(sdata.getType());
            if (it != typeByString.end()) {
                m_biotracker.getTrackingThread().setTrackingAlgorithm(
                        Core::Registry::getInstance().makeNewTracker(
                                Core::Registry::getInstance().getTypeByString().at(sdata.getType()), m_biotracker.getSettings()
                        )
                );
            } else {
                QMessageBox::warning(&m_mainWindow, "Unable to load tracking data",
                                     "Unknown tracker type.");
                return;
            }
        }

        const boost::optional<std::vector<std::string>> currentFiles = getOpenFiles();

        if (!currentFiles) {
            QMessageBox::warning(&m_mainWindow, "Unable to load tracking data",
                                 "No file opened.");
            return;
        }

        const boost::optional<std::string> hash = getFileHash(currentFiles.get().front(), currentFiles.get().size());

        if (!hash) {
            QMessageBox::warning(&m_mainWindow, "Unable to load tracking data",
                                 "Could not calculate file hash.");
            return;
        }

        if (sdata.getFileSha1Hash() != hash.get()) {
            auto buttonClicked = QMessageBox::warning(&m_mainWindow, "Unable to load tracking data",
                                                      "File hash does not match", QMessageBox::Ok | QMessageBox::Ignore);
            if (buttonClicked == QMessageBox::Ok)
                return;
        }

        m_biotracker.getTrackingThread().loadTrackedObjects(sdata.getTrackedObjects());
    }
}

void Gui::storeTrackingData() {
    if (!m_biotracker.getTrackingThread().getTrackerType()) {
        QMessageBox::warning(&m_mainWindow, "Unable to store tracking data",
                             "No tracker selected.");
        return;
    }

    QFileDialog dialog(&m_mainWindow, "Save tracking data");
    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.setDefaultSuffix("tdat");
    dialog.setNameFilter("Data Files (*.tdat)");
    // set displayed file as default filename:
//    if (_filename.lastIndexOf(".") > 0)
//        dialog.selectFile(_filename.left(_filename.lastIndexOf(".")));
    if (dialog.exec()) {
        const QStringList filenames = dialog.selectedFiles();
        if (!filenames.empty()) {
            assert(m_biotracker.getTrackingThread().getTrackerType());
//            printGuiMessage("Storing tracking data in " + path.toStdString(), MSGS::NOTIFICATION);

            const std::string trackerType =
                    Core::Registry::getInstance().getStringByType().at(m_biotracker.getTrackingThread().getTrackerType().get());
            const boost::optional<std::vector<std::string>> currentFiles = getOpenFiles();

            if (!currentFiles) {
                QMessageBox::warning(&m_mainWindow, "Unable to store tracking data",
                                     "No file opened.");
                return;
            }

            const boost::optional<std::string> hash = getFileHash(currentFiles.get().front(), currentFiles.get().size());

            if (!hash) {
                QMessageBox::warning(&m_mainWindow, "Unable to store tracking data",
                                     "Could not calculate file hash.");
                return;
            }

            Core::Serialization::Data sdata(trackerType, hash.get(), getFilenamesFromPaths(currentFiles.get()),
                                      m_biotracker.getTrackingThread().getTrackedObjects().get());

            std::ofstream ostream(filenames.first().toStdString(), std::ios::binary);
            cereal::JSONOutputArchive archive(ostream);
            archive(std::move(sdata));
        }
    }
}

boost::optional<std::vector<std::string>> Gui::getOpenFiles() {
    Core::Settings settings = m_biotracker.getSettings();
    boost::optional<uint8_t> mediaTypeOpt = settings.maybeGetValueOfParam<uint8_t>(GuiParam::MEDIA_TYPE);
    GuiParam::MediaType mediaType = mediaTypeOpt ? static_cast<GuiParam::MediaType>(*mediaTypeOpt) : GuiParam::MediaType::NoMedia;

    boost::optional<std::vector<std::string>> filename;
    switch (mediaType) {
        case GuiParam::MediaType::Images:
            filename = settings.getValueOfParam<std::vector<std::string>>(PictureParam::PICTURE_FILES);
            break;
        case GuiParam::MediaType::Video:
            filename = std::vector<std::string>();
            filename.get().push_back(settings.getValueOfParam<std::string>(CaptureParam::CAP_VIDEO_FILE));
            break;
        default:
            return boost::optional<std::vector<std::string>>();
    }

    // cap_video_file and picture_file can be set, but empty. therefore, we
    // need to check if the parameter actually contains a file name.
    if (filename && !filename.get().empty()) return filename;

    return boost::optional<std::vector<std::string>>();
}

boost::optional<std::string> Gui::getFileHash(const std::string &filename, const size_t numFiles) const {
    QCryptographicHash sha1Generator(QCryptographicHash::Sha1);
    QFile file(QString::fromStdString(filename));
    if (file.open(QIODevice::ReadOnly)) {
        // calculate hash from first 4096 bytes of file
        sha1Generator.addData(file.peek(4096));
        sha1Generator.addData(QByteArray::number(static_cast<qulonglong>(numFiles)));
        return QString(sha1Generator.result().toHex()).toStdString();
    }

    return boost::optional<std::string>();
}

std::vector<std::string> Gui::getFilenamesFromPaths(const std::vector<std::string> &paths) const {
    std::vector<std::string> filenames;
    filenames.reserve(paths.size());
    for (std::string const& path : paths) {
        const QFileInfo fi(QString::fromStdString(path));
        filenames.push_back(fi.baseName().toStdString());
    }
    return filenames;
}

void Gui::exit() {

}

} // Gui
} // BioTracker
