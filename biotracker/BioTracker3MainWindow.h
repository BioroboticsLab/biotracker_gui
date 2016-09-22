#ifndef BIOTRACKER3MAINWINDOW_H
#define BIOTRACKER3MAINWINDOW_H

#include <QMainWindow>

#include <boost/filesystem.hpp>
#include "Interfaces/iview.h"

// delete
#include "BioTracker3VideoView.h"

namespace Ui {
class BioTracker3MainWindow;
}

class IController;
class BioTracker3MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit BioTracker3MainWindow(QWidget *parent = 0);
    ~BioTracker3MainWindow();

    // Add the Controller
    void addController(IController * controller);

    // Add Views
    void addVideoControllWidget(QWidget *widget);

    // change to IView
    void addVideoView(BioTracker3VideoView *videoView);

public Q_SLOTS:


private Q_SLOTS:
    void on_actionOpen_Video_triggered();

private:
    Ui::BioTracker3MainWindow *ui;

    // The BioTrackerController of this View
    IController *mController;

    // This ist the VideoControllWidget
    QWidget *m_videoControlWidget;

    // change to IVIEW
    BioTracker3VideoView *m_videoView;
};

#endif // BIOTRACKER3MAINWINDOW_H
