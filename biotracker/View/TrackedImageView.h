#ifndef TRACKEDIMAGEVIEW_H
#define TRACKEDIMAGEVIEW_H

#include "Interfaces/IViewOpenGLWidget.h"

#include "QPainter"
#include "Interfaces/IViewWidget.h"
#include "TextureObject.h"

class TrackedImageView : public IViewOpenGLWidget
{
public:
    TrackedImageView(QWidget *parent, IController *controller, IModel *model);
    ~TrackedImageView();

    // QOpenGLWidget interface
protected:
    void paintGL() override;

    // IViewOpenGLWidget interface
public Q_SLOTS:
    void getNotified() override;
};

#endif // TRACKEDIMAGEVIEW_H
