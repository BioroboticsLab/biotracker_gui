#ifndef GRAPHICSVIEW_H
#define GRAPHICSVIEW_H

#include "Interfaces/IView/IGraphicsView.h"
#include "QGraphicsScene"

class GraphicsView : public IGraphicsView
{
public:
    GraphicsView(QWidget *parent = 0, IController *controller = 0, IModel *model = 0);

    void addGraphicsItem(QGraphicsItem *item);
    void addPixmapItem(QGraphicsItem *item);

    // IGraphicsView interface
public Q_SLOTS:
    void getNotified() override;

private:
    QGraphicsScene *m_GraphicsScene;

    // QWidget interface
protected:
    void wheelEvent(QWheelEvent *event) override;
};

#endif // GRAPHICSVIEW_H
