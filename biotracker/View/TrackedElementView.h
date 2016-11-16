#ifndef TRACKEDELEMENTVIEW_H
#define TRACKEDELEMENTVIEW_H

#include "../biotracker/Interfaces/IViewTrackedComponent.h"

class TrackedElementView : public IViewTrackedComponent
{
    Q_OBJECT
public:
    TrackedElementView(QGraphicsItem *parent = 0, IController *controller = 0, IModel *model = 0);

    // QGraphicsItem interface
public:
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    // IViewTrackedComponent interface
public Q_SLOTS:
    void getNotified() override;
};

#endif // TRACKEDELEMENTVIEW_H
