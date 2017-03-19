#ifndef DOWNLOADWINDOW_H
#define DOWNLOADWINDOW_H

#include <QWidget>

namespace Ui {
class downloadwindow;
}

class downloadwindow : public QWidget
{
    Q_OBJECT

public:
    explicit downloadwindow(QWidget *parent = 0);
    ~downloadwindow();

private:
    Ui::downloadwindow *ui;
};

#endif // DOWNLOADWINDOW_H
