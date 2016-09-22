#include "BioTracker3VideoControllWidget.h"
#include "ui_BioTracker3VideoControllWidget.h"

#include "BioTrackerController.h"

BioTracker3VideoControllWidget::BioTracker3VideoControllWidget(IController *controller, IModel *model) :
    IView(0, controller, model),
    ui(new Ui::BioTracker3VideoControllWidget)
{
    ui->setupUi(this);
}

BioTracker3VideoControllWidget::~BioTracker3VideoControllWidget()
{
    delete ui;
}

void BioTracker3VideoControllWidget::setModel(IModel *model)
{
    mModel = model;
}

void BioTracker3VideoControllWidget::setupView()
{

}

void BioTracker3VideoControllWidget::getNotified()
{

}

void BioTracker3VideoControllWidget::on_button_playPause_clicked()
{
     BioTrackerController *controller = dynamic_cast<BioTrackerController *>(getController());
     controller->startTracking();
}

void BioTracker3VideoControllWidget::on_sld_speed_sliderPressed()
{
    BioTrackerController *controller = dynamic_cast<BioTrackerController *>(getController());
    controller->startTracking();
}

void BioTracker3VideoControllWidget::on_button_playPause_pressed()
{
    BioTrackerController *controller = dynamic_cast<BioTrackerController *>(getController());
    controller->startTracking();
}

void BioTracker3VideoControllWidget::on_button_stop_clicked()
{
}
