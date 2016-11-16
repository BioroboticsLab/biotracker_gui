#ifndef BIOTRACKER3VIDEOVIEW_H
#define BIOTRACKER3VIDEOVIEW_H

#include "Interfaces/IViewOpenGLWidget.h"

#include "QPainter"
#include "Interfaces/IViewWidget.h"
#include "TextureObject.h"



class BioTracker3VideoView: public IViewOpenGLWidget
{
public:
    BioTracker3VideoView(QWidget *parent, IController *controller = 0, IModel *model = 0);
    ~BioTracker3VideoView();


    // QObject interface
public:
    bool event(QEvent *event) override;

    void addRect(IModel *model);

private:
    void paintGL() override;


public Q_SLOTS:
    void getNotified();

private:
    IModel *myRect;


};

#endif // BIOTRACKER3VIDEOVIEW_H
