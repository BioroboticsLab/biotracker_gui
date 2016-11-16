#include "TrackedElementView.h"

TrackedElementView::TrackedElementView(QGraphicsItem *parent, IController *controller, IModel *model) :
    IViewTrackedComponent(parent, controller, model)
{

}

QRectF TrackedElementView::boundingRect() const
{
    return QRectF(0,0,0,0);
}

void TrackedElementView::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{

}

void TrackedElementView::getNotified()
{

}
