#include "BioTracker3MainWindow.h"
#include "ui_BioTracker3MainWindow.h"

#include "BioTrackerController.h"
#include "BioTracker3VideoControllWidget.h"
#include "qfiledialog.h"
#include "QLayout"

BioTracker3MainWindow::BioTracker3MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::BioTracker3MainWindow)
{
    ui->setupUi(this);
}

BioTracker3MainWindow::~BioTracker3MainWindow()
{
    delete ui;
}

void BioTracker3MainWindow::addController(IController *controller)
{
    mController = controller;
}

void BioTracker3MainWindow::addVideoControllWidget(QWidget *widget)
{
    QLayout *layout = new QGridLayout(this);
    layout->addWidget(widget);
    ui->videoControls->setLayout(layout);
}

// change to IView
void BioTracker3MainWindow::addVideoView(BioTracker3VideoView *videoView)
{
    m_videoView = videoView;
    m_videoView->setParent(ui->trackingArea);
    ui->videoViewLayout->addWidget(m_videoView);
}

void BioTracker3MainWindow::on_actionOpen_Video_triggered()
{
    static const QString videoFilter("Video files (*.avi *.wmv *.mp4 *.mkv *.mov)");
    std::vector<boost::filesystem::path> files;

    QString filename = QFileDialog::getOpenFileName(this,
                             "Open video", "", videoFilter);

    if (!filename.isEmpty()) {
        dynamic_cast<BioTrackerController *> (mController)->loadVideo(filename);
    }
}
