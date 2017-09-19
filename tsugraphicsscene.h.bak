#ifndef TSUGRAPHICSSCENE_H
#define TSUGRAPHICSSCENE_H

#include <QObject>
#include <QWidget>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>

class tsugraphicsscene : public QGraphicsScene, QWidget
{
    Q_OBJECT

public:
    tsugraphicsscene(QWidget *parent = 0);
    ~tsugraphicsscene();

    // QGraphicsScene interface
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
};

#endif // TSUGRAPHICSSCENE_H
