#pragma once

#include <QStringList>

#include "biotracker/BioTrackerApp.h"

#include "MainWindow.h"

namespace BioTracker {
namespace Gui {

class Gui : public QObject {
    Q_OBJECT
  public:
    Gui();
  private:
    Core::BioTrackerApp m_biotracker;
    MainWindow m_mainWindow;

    void initConnects();
    QStringList browse(const QString &title, const QString &filter);

    boost::optional<std::vector<std::string>> getOpenFiles();
    boost::optional<std::string> getFileHash(std::string const& filename, const size_t numFiles) const;
    std::vector<std::string> getFilenamesFromPaths(std::vector<std::string> const& paths) const;

  private Q_SLOTS:
    void browseVideo();
    void browsePictures();
    void browseCameras();

    void loadTracker();

    void loadTrackingData();
    void storeTrackingData();

    void exit();
};



}
}
