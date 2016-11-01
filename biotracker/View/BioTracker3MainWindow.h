#ifndef BIOTRACKER3MAINWINDOW_H
#define BIOTRACKER3MAINWINDOW_H

#include <boost/filesystem.hpp>
#include "Interfaces/IViewMainWindow.h"

// delete
#include "BioTracker3VideoView.h"

namespace Ui {
class BioTracker3MainWindow;
}

class IController;
class BioTracker3MainWindow : public IViewMainWindow
{
    Q_OBJECT

public:
    explicit BioTracker3MainWindow(QWidget *parent = 0, IController *controller = 0);
    ~BioTracker3MainWindow();

    void addVideoControllWidget(IView *widget);
    void addVideoView(IView *videoView);

private Q_SLOTS:
    void on_actionOpen_Video_triggered();

private:
    Ui::BioTracker3MainWindow *ui;
};

#endif // BIOTRACKER3MAINWINDOW_H
