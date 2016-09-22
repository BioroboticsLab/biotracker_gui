#ifndef BIOTRACKER3VIDEOVIEW_H
#define BIOTRACKER3VIDEOVIEW_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLTexture>
#include "QPainter"

#include "Interfaces/iview.h"

#include "TextureObject.h"



class BioTracker3VideoView: public QOpenGLWidget, protected QOpenGLFunctions
{
public:
    BioTracker3VideoView(IController *controller = 0, BioTracker::Core::TextureObject *model = 0);

    void setModel(IModel *model);

private:
    void setupView();

    void paintGL() override;
    void initializeGL() override;



public Q_SLOTS:

    virtual void getNotified();

    BioTracker::Core::TextureObject m_TextureObject();

    IController *m_Controller;
    BioTracker::Core::TextureObject *m_Model;


};

#endif // BIOTRACKER3VIDEOVIEW_H
