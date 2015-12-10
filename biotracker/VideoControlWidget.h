#pragma once

#include <QIcon>
#include <QWidget>

#include "biotracker/BioTrackerApp.h"
#include "biotracker/TrackerStatus.h"
#include "ui_VideoControlWidget.h"
#include "biotracker/util/QtRaiiWrapper.hpp"
#include "VideoView.h"
#include "biotracker/util/QtRaiiWrapper.hpp"

namespace BioTracker {
namespace Gui {

class VideoControlWidget : public QWidget {
    Q_OBJECT
  public:
    typedef Util::QtRaiiWrapper<Ui::VideoControlWidget, QWidget> VideoControlUi;

    explicit VideoControlWidget(QWidget *parent, Core::BioTrackerApp &facade,
                                VideoView *videoView);

    VideoControlUi &getUi() {
        return m_ui;
    }

    void updateWidgets();

  public Q_SLOTS:
    void fileOpened(const std::string filename, const size_t totalFrames, const double tfps);

  private:
    VideoControlUi m_ui;
    Core::BioTrackerApp &m_bioTracker;
    VideoView *m_videoView;

    QIcon m_iconPause;
    QIcon m_iconPlay;
    QTimer m_timer;

    bool m_isPanZoomMode;
    bool m_sliderVideoWasRunning;

    void initConnects();
    void initShortcuts();
  public Q_SLOTS:
    void frameCalculated(const size_t frameNumber, const std::string filename,
                         const double currentFps);

  private Q_SLOTS:
    void playPause();
    void stop();
    void setFrame(const size_t frame);
    void nextFrame();
    void previousFrame();
    void changeCurrentFrameByEdit();
    void takeScreenshot();
    void switchPanZoomMode();
    void videoSliderReleased();
    void videoSliderPressed();
    void speedSliderValueChanged(int value);
    void viewChanged(QString);
    void registerViews(const std::vector<TrackingAlgorithm::View> view);
};

}
}

