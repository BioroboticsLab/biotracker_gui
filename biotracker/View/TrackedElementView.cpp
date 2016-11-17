#include "TrackedElementView.h"
#include "Model/TrackedComponents/TrackedElement.h"
#include "QBrush"
#include "QPainter"

TrackedElementView::TrackedElementView(QGraphicsItem *parent, IController *controller, IModel *model) :
    IViewTrackedComponent(parent, controller, model)
{
    TrackedElement *elem = dynamic_cast<TrackedElement *>(getModel());
    m_boundingRect = QRectF(0,0, elem->getX(), elem->getY());
}

QRectF TrackedElementView::boundingRect() const
{

    return m_boundingRect;
}

void TrackedElementView::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    TrackedElement *elem = dynamic_cast<TrackedElement *>(getModel());

    QRectF rec = boundingRect();

    QBrush brush(Qt::blue);

    if(elem->getPressedStatus())
    {
        brush.setColor(Qt::red);
    }
    else
    {
        brush.setColor(Qt::green);
    }

    painter->fillRect(rec,brush);
    painter->drawRect(rec);

}

void TrackedElementView::getNotified()
{
    update();
}

void TrackedElementView::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    TrackedElement *elem = dynamic_cast<TrackedElement *>(getModel());

    elem->pressed();
    update();

    QGraphicsObject::mousePressEvent(event);
}

void TrackedElementView::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    TrackedElement *elem = dynamic_cast<TrackedElement *>(getModel());

    elem->notPressed();
    update();

    QGraphicsObject::mousePressEvent(event);
}
