#ifndef BIOTRACKER3VIDEOCONTROLLWIDGET_H
#define BIOTRACKER3VIDEOCONTROLLWIDGET_H

#include <QWidget>
#include "QIcon"
#include "Interfaces/IViewWidget.h"


namespace Ui {
class BioTracker3VideoControllWidget;
}

class BioTracker3VideoControllWidget : public IViewWidget
{
    Q_OBJECT

public:
    explicit BioTracker3VideoControllWidget(IController *controller, IModel *model);
    ~BioTracker3VideoControllWidget();

public Q_SLOTS:
    void getNotified();

private Q_SLOTS:
    void on_sld_video_sliderMoved(int position);
    void on_DurationChanged(int position);
    void on_PositionChanged(int position);

private Q_SLOTS:
    void on_button_nextFrame_clicked();
    void on_button_playPause_clicked();

    void on_button_stop_clicked();

    void on_button_previousFrame_clicked();

private:
    Ui::BioTracker3VideoControllWidget *ui;

    QIcon m_iconPause;
    QIcon m_iconPlay;
};

#endif // BIOTRACKER3VIDEOCONTROLLWIDGET_H
