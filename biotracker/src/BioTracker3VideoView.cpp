#include "BioTracker3VideoView.h"

#include <biotracker/util/ScreenHelper.h>
#include "Interfaces/icontroller.h"



// change : IView
BioTracker3VideoView::BioTracker3VideoView(IController *controller, BioTracker::Core::TextureObject *model):
   // IView(0, controller, model)
    m_Controller(controller), m_Model(model)
{

}

void BioTracker3VideoView::paintGL()
{
    QImage img = m_Model->get();

    QRectF target(0, 0 , this->width(), this->height());
    QRectF source(0, 0, m_Model->width(), m_Model->height());


    QPainter painter(this);
    painter.drawImage(target, img, source);
    QPainter p(this);

    update();

}

void BioTracker3VideoView::initializeGL()
{
}


void BioTracker3VideoView::getNotified()
{

}

void BioTracker3VideoView::setModel(IModel *model)
{
    // change
    //mModel = model;
}

void BioTracker3VideoView::setupView()
{

}
