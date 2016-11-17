#include "GraphicsView.h"

GraphicsView::GraphicsView(QWidget *parent, IController *controller, IModel *model) :
    IGraphicsView(parent, controller, model)
{
    m_GraphicsScene = new QGraphicsScene();
    this->setScene(m_GraphicsScene);

    this->show();
}

void GraphicsView::getNotified()
{

}
