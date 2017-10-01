#ifndef ARCHIVEWINDOW_H
#define ARCHIVEWINDOW_H

#include <QWidget>

#include <tsutreewidget.h>

namespace Ui {
class archivewindow;
}

class archivewindow : public QWidget
{
    Q_OBJECT

public:
    explicit archivewindow(QWidget *parent = 0);
    ~archivewindow();

private:
    Ui::archivewindow *ui;
    tsuTreeWidget *p_treeWidget;

    void changeEvent(QEvent *e);

};

#endif // ARCHIVEWINDOW_H
