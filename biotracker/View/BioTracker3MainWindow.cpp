#include "BioTracker3MainWindow.h"
#include "ui_BioTracker3MainWindow.h"

#include "Controller/ControllerMainWindow.h"
#include "BioTracker3VideoControllWidget.h"
#include "qfiledialog.h"
#include "QLayout"

BioTracker3MainWindow::BioTracker3MainWindow(QWidget *parent, IController *controller, IModel *model) :
    IViewMainWindow(parent, controller, model),
    ui(new Ui::BioTracker3MainWindow)
{
    ui->setupUi(this);
}

BioTracker3MainWindow::~BioTracker3MainWindow()
{
    delete ui;
}

void BioTracker3MainWindow::addVideoControllWidget(IView *widget)
{
    QLayout *layout = new QGridLayout(this);
    layout->addWidget(dynamic_cast<QWidget *>(widget));
    ui->videoControls->setLayout(layout);
}

void BioTracker3MainWindow::addVideoView(IView *videoView)
{
    dynamic_cast<BioTracker3VideoView *>(videoView)->setParent(ui->trackingArea);
    ui->videoViewLayout->addWidget(dynamic_cast<BioTracker3VideoView *>(videoView));
}

void BioTracker3MainWindow::on_actionOpen_Video_triggered()
{
    static const QString videoFilter("Video files (*.avi *.wmv *.mp4 *.mkv *.mov)");
    std::vector<boost::filesystem::path> files;

    QString filename = QFileDialog::getOpenFileName(this,
                             "Open video", "", videoFilter);

    if (!filename.isEmpty()) {
        dynamic_cast<ControllerMainWindow *> (getController())->loadVideo(filename);
    }
}
