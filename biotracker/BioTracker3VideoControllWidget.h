#ifndef BIOTRACKER3VIDEOCONTROLLWIDGET_H
#define BIOTRACKER3VIDEOCONTROLLWIDGET_H

#include <QWidget>
#include "Interfaces/iview.h"


namespace Ui {
class BioTracker3VideoControllWidget;
}

class BioTracker3VideoControllWidget : public IView
{
    Q_OBJECT

public:
    explicit BioTracker3VideoControllWidget(IController *controller, IModel *model);
    ~BioTracker3VideoControllWidget();

    void setModel(IModel *model);

    /////      Must implement from Base Class    /////

private:
    void setupView();

public Q_SLOTS:
    void getNotified();

private Q_SLOTS:
        void on_button_playPause_clicked();
        void on_sld_speed_sliderPressed();

        void on_button_playPause_pressed();

        void on_button_stop_clicked();

private:
    Ui::BioTracker3VideoControllWidget *ui;
};

#endif // BIOTRACKER3VIDEOCONTROLLWIDGET_H
