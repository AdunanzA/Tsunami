#ifndef TSUGRAPHICSSCENE_H
#define TSUGRAPHICSSCENE_H

#include <QObject>
#include <QWidget>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>

#include <QDropEvent>
#include <QMimeData>

class tsugraphicsscene : public QGraphicsScene
{
    Q_OBJECT

public:
    tsugraphicsscene(QWidget *parent = 0);
    ~tsugraphicsscene();

signals:
    void fileDropped(QString fileName);

    // QGraphicsScene interface
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void dragMoveEvent(QGraphicsSceneDragDropEvent *event) override;
    void dragEnterEvent(QGraphicsSceneDragDropEvent *event) override;
    void dropEvent(QGraphicsSceneDragDropEvent *event) override;

};

#endif // TSUGRAPHICSSCENE_H
