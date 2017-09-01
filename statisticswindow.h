#ifndef STATISTICSWINDOW_H
#define STATISTICSWINDOW_H

#include <QWidget>
#include <QStandardItemModel>
#include <QDateTime>
#include <QDebug>

#include "chartxy/chartxy.h"

namespace Ui {
class statisticswindow;
}

class statisticswindow : public QWidget
{
    Q_OBJECT

public:
    explicit statisticswindow(QWidget *parent = 0);
    ~statisticswindow();

public slots:
    void updateStats(const QPair<int, int> & values);

private:
    Ui::statisticswindow *ui;
    QStandardItemModel *model;
    ChartXY *chart;

    void setupModel(void);
    void setupChart(void);
    void changeEvent(QEvent *e);

};

#endif // STATISTICSWINDOW_H
