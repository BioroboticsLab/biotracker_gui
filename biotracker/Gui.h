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
