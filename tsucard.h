#ifndef TSUCARD_H
#define TSUCARD_H

#include <QWidget>

#include "tsucard.h"

namespace Ui {
    class tsucard;
}

class tsucard : public QWidget
{
    Q_OBJECT


public:
    explicit tsucard(QWidget *parent = 0);
    ~tsucard();
    void set_Name(QString name);
    QString get_Name();


private:
    Ui::tsucard *ui;
    QString p_name;

private slots:
    void btnCancelClick();

signals:
    void cancelRequested(tsucard *sender);
};

#endif // TSUCARD_H
