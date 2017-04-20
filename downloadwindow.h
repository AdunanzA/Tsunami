#ifndef DOWNLOADWINDOW_H
#define DOWNLOADWINDOW_H

#include <QtGui>
#include <QWidget>

#include "flowlayout.h"
#include "tsucard.h"

namespace Ui {
    class downloadwindow;
}

class downloadwindow : public QWidget
{
    Q_OBJECT

public:
    explicit downloadwindow(QWidget *parent = 0);
    ~downloadwindow();

    void addTorrent(QString name);

private:
    Ui::downloadwindow *ui;
    FlowLayout *flowLayout;

private slots:
    void requestedCancel(tsucard *sender);
};

#endif // DOWNLOADWINDOW_H
