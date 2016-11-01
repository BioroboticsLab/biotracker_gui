#include "BioTracker3VideoControllWidget.h"
#include "ui_BioTracker3VideoControllWidget.h"

#include "Controller/BioTrackerController.h"
#include "Interfaces/IPlayerState.h"

BioTracker3VideoControllWidget::BioTracker3VideoControllWidget(IController *controller, IModel *model) :
    IViewWidget(0, controller, model),
    ui(new Ui::BioTracker3VideoControllWidget)
{
    ui->setupUi(this);

    m_iconPause.addFile(QStringLiteral(":/BioTracker/resources/pause-sign.png"),
                        QSize(), QIcon::Normal, QIcon::Off);
    m_iconPlay.addFile(QStringLiteral(":/BioTracker/resources/arrow-forward1.png"),
                       QSize(), QIcon::Normal, QIcon::Off);
}

BioTracker3VideoControllWidget::~BioTracker3VideoControllWidget()
{
    delete ui;
}

void BioTracker3VideoControllWidget::getNotified()
{
    ui->button_nextFrame->setEnabled(dynamic_cast<BioTracker3Player *>(getModel())->getStateOfStepForward());
    ui->button_playPause->setEnabled(dynamic_cast<BioTracker3Player *>(getModel())->getStateOfPlay());
    ui->button_previousFrame->setEnabled(dynamic_cast<BioTracker3Player *>(getModel())->getStateOfStepBack());
    ui->button_stop->setEnabled(dynamic_cast<BioTracker3Player *>(getModel())->getStateOfStop());

    bool isPaused = dynamic_cast<BioTracker3Player *>(getModel())->getStateOfPause();
    if (isPaused) {
        ui->button_playPause->setIcon(m_iconPause);
    }
    else {
        ui->button_playPause->setIcon(m_iconPlay);
    }
}

void BioTracker3VideoControllWidget::on_button_nextFrame_clicked()
{
    BioTrackerController *controller = dynamic_cast<BioTrackerController *>(getController());
    controller->nextFrame();
}

void BioTracker3VideoControllWidget::on_button_playPause_clicked()
{
    bool isPaused = dynamic_cast<BioTracker3Player *>(getModel())->getStateOfPause();

    BioTrackerController *controller = dynamic_cast<BioTrackerController *>(getController());

    if (isPaused) {
        controller->pause();
    }
    else {
        controller->play();
    }
}

void BioTracker3VideoControllWidget::on_button_stop_clicked()
{
    BioTrackerController *controller = dynamic_cast<BioTrackerController *>(getController());
    controller->stop();
}

void BioTracker3VideoControllWidget::on_button_previousFrame_clicked()
{
    BioTrackerController *controller = dynamic_cast<BioTrackerController *>(getController());
    controller->prevFrame();
}

void BioTracker3VideoControllWidget::on_sld_video_sliderMoved(int position)
{

}

void BioTracker3VideoControllWidget::on_DurationChanged(int position)
{

}

void BioTracker3VideoControllWidget::on_PositionChanged(int position)
{

}
