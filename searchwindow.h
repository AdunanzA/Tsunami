#ifndef SEARCHWINDOW_H
#define SEARCHWINDOW_H

#include <QWidget>

namespace Ui {
class searchwindow;
}

class searchwindow : public QWidget
{
    Q_OBJECT

public:
    explicit searchwindow(QWidget *parent = 0);
    ~searchwindow();

private:
    Ui::searchwindow *ui;
};

#endif // SEARCHWINDOW_H
