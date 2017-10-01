#include "tsugraphicsscene.h"

tsugraphicsscene::tsugraphicsscene(QWidget *parent)
    : QGraphicsScene(parent)
{
    qDebug("created");
}

tsugraphicsscene::~tsugraphicsscene()
{
    qDebug("destroyed");
}

void tsugraphicsscene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
 //    qDebug("mousePressEvent");
    // Ignore deselect all on right click
    if (event->button() != Qt::LeftButton) {
        event->accept();
        return;
    }
    QGraphicsScene::mousePressEvent(event);
}

void tsugraphicsscene::dragMoveEvent(QGraphicsSceneDragDropEvent *event)
{
    event->accept();
}

void tsugraphicsscene::dragEnterEvent(QGraphicsSceneDragDropEvent *event)
{
    event->accept();
}

void tsugraphicsscene::dropEvent(QGraphicsSceneDragDropEvent *event)
{
    emit fileDropped(event->mimeData()->text());
    event->accept();
}
