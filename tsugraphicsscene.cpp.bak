#include "tsugraphicsscene.h"

tsugraphicsscene::tsugraphicsscene(QWidget *parent)
    : QWidget(parent)
{

}

tsugraphicsscene::~tsugraphicsscene()
{

}

void tsugraphicsscene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    // Ignore deselect all on right click
    if (event->button() != Qt::LeftButton) {
        event->accept();
        return;
    }
    QGraphicsScene::mousePressEvent(event);
}
