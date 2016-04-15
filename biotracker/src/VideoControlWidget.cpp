#include "../VideoControlWidget.h"


#include <QKeySequence>
#include <QShortcut>
#include <thread>

#include "../VideoView.h"


namespace BioTracker {
namespace Gui {

VideoControlWidget::VideoControlWidget(QWidget *parent,
                                       Core::BioTrackerApp &facade, VideoView *videoView)
    : QWidget(parent)
    , m_ui(parent)
    , m_bioTracker(facade)
    , m_videoView(videoView)
    , m_isPanZoomMode(false) {
    m_iconPause.addFile(QStringLiteral(":/BioTracker/resources/pause-sign.png"),
                        QSize(), QIcon::Normal, QIcon::Off);
    m_iconPlay.addFile(QStringLiteral(":/BioTracker/resources/arrow-forward1.png"),
                       QSize(), QIcon::Normal, QIcon::Off);

    initConnects();
    initShortcuts();
}

void VideoControlWidget::updateWidgets() {

    const bool validFile = m_bioTracker.getStatus() ==
                           BioTracker::Core::TrackerStatus::Running ||
                           m_bioTracker.getStatus() == BioTracker::Core::TrackerStatus::Paused;

    const bool hasNext = m_bioTracker.getCurrentFrameNumber() <
                         m_bioTracker.getNumFrames()-1;
    const bool hasPrev = m_bioTracker.getCurrentFrameNumber() > 0;

    m_ui.button_nextFrame->setEnabled(validFile && hasNext);
    m_ui.button_previousFrame->setEnabled(validFile && hasPrev);

    m_ui.button_stop->setEnabled(validFile && hasPrev);

    switch (m_bioTracker.getStatus()) {
    case BioTracker::Core::TrackerStatus::Invalid:
    case BioTracker::Core::TrackerStatus::NothingLoaded:
        m_ui.button_playPause->setIcon(m_iconPlay);
        m_ui.button_playPause->setEnabled(false);
        m_ui.sld_video->setEnabled(false);
        break;
    case BioTracker::Core::TrackerStatus::Running:
        m_ui.button_playPause->setIcon(m_iconPause);
        m_ui.button_playPause->setEnabled(true);
        m_ui.sld_video->setEnabled(true);
        break;
    case BioTracker::Core::TrackerStatus::Paused:
        m_ui.button_playPause->setIcon(m_iconPlay);
        m_ui.button_playPause->setEnabled(hasNext);
        m_ui.sld_video->setEnabled(true);
        break;
    default:
        assert(false);
    }
}

void VideoControlWidget::initShortcuts() {
    const QString shortcutPanKey = QString::fromStdString(
                                       m_bioTracker.getSettings().getValueOrDefault<std::string>
                                       (GuiParam::SHORTCUT_ZOOM,"Z"));
    const QShortcut *shortcutPan = new QShortcut(QKeySequence(shortcutPanKey),
            this);
    QObject::connect(shortcutPan, &QShortcut::activated, m_ui.button_panZoom,
                     &QPushButton::click);

    const QString shortcutPlayKey = QString::fromStdString(
                                        m_bioTracker.getSettings().getValueOrDefault<std::string>
                                        (GuiParam::SHORTCUT_PLAY,"Space"));
    const QShortcut *shortcutPlay = new QShortcut(QKeySequence(shortcutPlayKey),
            this);
    QObject::connect(shortcutPlay, &QShortcut::activated, m_ui.button_playPause,
                     &QPushButton::click);

    const QString shortcutNextKey = QString::fromStdString(
                                        m_bioTracker.getSettings().getValueOrDefault<std::string>
                                        (GuiParam::SHORTCUT_NEXT,"Right"));
    const QShortcut *shortcutNext = new QShortcut(QKeySequence(shortcutNextKey),
            this);
    QObject::connect(shortcutNext, &QShortcut::activated, m_ui.button_nextFrame,
                     &QPushButton::click);

    const QString shortcutPrevKey = QString::fromStdString(
                                        m_bioTracker.getSettings().getValueOrDefault<std::string>
                                        (GuiParam::SHORTCUT_PREV,"Left"));
    const QShortcut *shortcutPrev = new QShortcut(QKeySequence(shortcutPrevKey),
            this);
    QObject::connect(shortcutPrev, &QShortcut::activated, m_ui.button_previousFrame,
                     &QPushButton::click);
}

void VideoControlWidget::initConnects() {
    QObject::connect(m_ui.button_playPause, &QPushButton::clicked, this,
                     &VideoControlWidget::playPause);
    QObject::connect(m_ui.button_nextFrame, &QPushButton::clicked, this,
                     &VideoControlWidget::nextFrame);
    QObject::connect(m_ui.button_previousFrame, &QPushButton::clicked, this,
                     &VideoControlWidget::previousFrame);
    QObject::connect(m_ui.frame_num_edit, &QLineEdit::returnPressed, this,
                     &VideoControlWidget::changeCurrentFrameByEdit);
    QObject::connect(m_ui.button_screenshot, &QPushButton::clicked, this,
                     &VideoControlWidget::takeScreenshot);
    QObject::connect(m_ui.button_panZoom, &QPushButton::clicked, this,
                     &VideoControlWidget::switchPanZoomMode);
    QObject::connect(m_ui.button_stop, &QPushButton::clicked, this,
                     &VideoControlWidget::stop);

    // video slider
    QObject::connect(m_ui.sld_video, &QSlider::sliderReleased, this,
                     &VideoControlWidget::videoSliderReleased);
    QObject::connect(m_ui.sld_video, &QSlider::sliderPressed, this,
                     &VideoControlWidget::videoSliderPressed);

    // speed slider
    QObject::connect(m_ui.sld_speed, &QSlider::valueChanged, this,
                     &VideoControlWidget::speedSliderValueChanged);

    QObject::connect(m_ui.comboBoxSelectedView,
                     static_cast<void(QComboBox::*)(const QString &)>(&QComboBox::currentIndexChanged),
                     this, &VideoControlWidget::viewChanged);

    QObject::connect(&m_bioTracker, &Core::BioTrackerApp::registerViews, this,
                     &VideoControlWidget::registerViews);
    QObject::connect(&m_bioTracker, &Core::BioTrackerApp::requestPaint, this,
                     &VideoControlWidget::onRequestRepaint);

}

void VideoControlWidget::playPause() {
    switch (m_bioTracker.getStatus()) {
    case BioTracker::Core::TrackerStatus::Paused:
        m_bioTracker.play();
        break;
    case BioTracker::Core::TrackerStatus::Running:
        m_bioTracker.pause();
        break;
    default:
        assert(false);
    }

    updateWidgets();
}

void VideoControlWidget::stop() {
    m_bioTracker.pause();
    setFrame(0);
}

void VideoControlWidget::setFrame(const size_t frame) {
    assert(frame < m_bioTracker.getNumFrames());
    m_bioTracker.setFrame(frame);

    updateWidgets();
}

void VideoControlWidget::nextFrame() {
    assert(m_bioTracker.getCurrentFrameNumber() < m_bioTracker.getNumFrames());
    m_bioTracker.setFrame(m_bioTracker.getCurrentFrameNumber() + 1);

    updateWidgets();
}

void VideoControlWidget::fileOpened(const std::string filename,
                                    const size_t totalFrames,
                                    const double targetFps) {
    (void)filename; // "un-use" filename. FileOpen is a generic event, but we dont
    // need the filename at this place
	assert(totalFrames > 0);
    m_ui.sld_video->setMaximum(static_cast<int>(totalFrames - 1));
    m_ui.fps_label->setText(QString::number(targetFps));
    const int fpsAsInt = static_cast<int>(targetFps + 0.5);
    m_ui.sld_speed->setValue(fpsAsInt);
}

void VideoControlWidget::previousFrame() {
    assert(m_bioTracker.getCurrentFrameNumber() > 0);
    m_bioTracker.setFrame(m_bioTracker.getCurrentFrameNumber() - 1);
    updateWidgets();
}

void VideoControlWidget::videoSliderReleased() {


    const int frame = m_ui.sld_video->value();
    if (frame <= m_ui.sld_video->maximum()) {
        if (frame == m_ui.sld_video->maximum()) {
            m_bioTracker.pause();
        }
        setFrame(frame);
    }

    if (m_sliderVideoWasRunning) {
        m_bioTracker.play();
    }

    updateWidgets();
}

void VideoControlWidget::videoSliderPressed() {
    m_sliderVideoWasRunning = m_bioTracker.isRunning();
    m_bioTracker.pause();
    updateWidgets();
}

void VideoControlWidget::speedSliderValueChanged(int speed) {
    if (speed < m_ui.sld_speed->maximum()) {
        m_bioTracker.setMaxSpeed(false);
        m_bioTracker.setTargetFps(speed);
        m_ui.fps_label->setText(QString::number(speed));
    } else {
        m_bioTracker.setMaxSpeed(true);
        m_ui.fps_label->setText("max");
    }
}

void VideoControlWidget::viewChanged(QString n) {
    auto view = Core::TrackingAlgorithm::OriginalView;
    if (n != "Original") {
        view.name = n.toUtf8().constData();
    }
    m_videoView->setView(view);
}

void repaintVideoView(VideoView *videoView) {
    videoView->update();
}

void VideoControlWidget::onRequestRepaint() {
    repaintVideoView(m_videoView);
}

void VideoControlWidget::registerViews(const std::vector<Core::TrackingAlgorithm::View> views) {
    m_ui.comboBoxSelectedView->clear();
    m_ui.comboBoxSelectedView->addItem("Original");
    for (auto view : views) {
        m_ui.comboBoxSelectedView->addItem(QString::fromStdString(view.name));
    }
}

void VideoControlWidget::changeCurrentFrameByEdit() {
    setFrame(m_ui.sld_video->value());
}

void VideoControlWidget::takeScreenshot() {
    QString filename;
    char buffer[80];
    time_t rawtime;
    std::time(&rawtime);
    struct tm *timeinfo = localtime(&rawtime);
    strftime(buffer, 80, "%d-%m-%Y_%I-%M-%S", timeinfo);
    filename.append("/Screenshot_").append(buffer).append(".png");
    QString filepath = QFileDialog::getSaveFileName(this, tr("Save File"),
                       QDir::homePath().append(filename),
                       tr("Images (*.png)"));
    if (filepath.count()) {
        m_bioTracker.getTrackingThread().getTexture().get().save(filepath);
    }
}

void VideoControlWidget::switchPanZoomMode() {
    switch (m_videoView->getMode()) {
    case VideoView::Mode::INTERACTION:
        m_videoView->setMode(VideoView::Mode::PANZOOM);
        break;
    case VideoView::Mode::PANZOOM:
        m_videoView->setMode(VideoView::Mode::INTERACTION);
        break;
    default:
        assert(false);
        break;
    }
}

void VideoControlWidget::frameCalculated(const size_t frameNumber,
        const std::string, const double currentFps) {

    const bool hasNext = frameNumber < m_bioTracker.getNumFrames() - 1;

    repaintVideoView(m_videoView);

    m_ui.sld_video->setValue(static_cast<int>(frameNumber));
    m_ui.frame_num_edit->setText(QString::number(frameNumber));

    if (currentFps >= 0) {
        m_ui.fps_edit->setText(QString::number(currentFps));
    } else {
        m_ui.fps_edit->clear();
    }

    if (!hasNext) {
        m_bioTracker.pause();
    }

    updateWidgets();

}

} // Gui
} // BioTracker
