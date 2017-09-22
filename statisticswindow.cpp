#include "statisticswindow.h"
#include "ui_statisticswindow.h"

#include <QTableView>

statisticswindow::statisticswindow(QWidget *parent) :
    QWidget(parent),
//    chart(new ChartXY),
    ui(new Ui::statisticswindow)
{
    ui->setupUi(this);
    chart = new ChartXY(this);
    setupModel();
    setupChart();
}

statisticswindow::~statisticswindow()
{
    delete ui;
    qDebug("statisticswindow destroyed");
}

void statisticswindow::updateStats(const QPair<int, int> &values)
{
    //qDebug() << QString("updateState x:%1 d:%2, u:%3").arg(values.first).arg(values.second).arg(QDateTime::currentDateTime().time().toString());
    QList<QStandardItem *> newRow;
//    newRow.append(new QStandardItem(QDateTime::currentDateTime().time().toString()));
//    newRow.append(new QStandardItem(QString("%1").arg(values.first)));
//    newRow.append(new QStandardItem(QString("%1").arg(values.second)));
    newRow.append(new QStandardItem());
    newRow.append(new QStandardItem());
    newRow.append(new QStandardItem());
    model->appendRow(newRow);

    int lastRowIndex = model->rowCount()-1;
    model->setData(model->index(lastRowIndex,0),QVariant(QDateTime::currentDateTime()));
    if (values.first > 0 ) {
        model->setData(model->index(lastRowIndex,1),values.first);
    } else {
        model->setData(model->index(lastRowIndex,1), 1);
    }
    model->setData(model->index(lastRowIndex,1),QColor(0,255,0),Qt::DecorationRole);
    if (values.second > 0 ) {
        model->setData(model->index(lastRowIndex,2),values.second);
    } else {
        model->setData(model->index(lastRowIndex,2), 1);
    }
    model->setData(model->index(lastRowIndex,2),QColor(255,0,0),Qt::DecorationRole);

    if (model->rowCount() > 100) { model->removeRow(0); }

    chart->updateChart();
}

void statisticswindow::setupModel()
{
//    int i;
//    QDateTime date_time=QDateTime::currentDateTime();

    model=new QStandardItemModel(1,3,this);
    model->setHeaderData(0,Qt::Horizontal,tr("X"));
    model->setHeaderData(1,Qt::Horizontal,tr("Y"));
    model->setHeaderData(2,Qt::Horizontal,tr("K"));

    model->setData(model->index(0,0),QVariant(QDateTime::currentDateTime()));
    model->setData(model->index(0,1),QColor(0,255,0),Qt::DecorationRole);
    model->setData(model->index(0,1), 1);
    model->setData(model->index(0,2),QColor(255,0,0),Qt::DecorationRole);
    model->setData(model->index(0,2), 1);


//    for(i=0;i<model->rowCount();i++)
//      {
//      model->setData(model->index(i,0,QModelIndex()),QVariant(date_time));
//      date_time=date_time.addSecs(60);

//      model->setData(model->index(i,1,QModelIndex()),i*2+qrand()%7);
//      model->setData(model->index(i,1,QModelIndex()),QColor(255,0,0),Qt::DecorationRole);

//      model->setData(model->index(i,2,QModelIndex()),i+qrand()%14);
//      model->setData(model->index(i,2,QModelIndex()),QColor(0,255,0),Qt::DecorationRole);
//      }

//    ui->table->setModel(model);
}

void statisticswindow::setupChart()
{
//    QTableView *qtv = new QTableView();
//    qtv->setModel(model);
//    ui->horizontalLayout->addWidget(qtv);

    ui->horizontalLayout->addWidget(chart);
    chart->setModel(model);
    chart->setting().border().setRight(64);
    //chart->setting().border().setBackgroundColor(QColor(255,255,255,1));
    chart->setting().grid().horizzontalTick().setTickMajor(4);
    chart->setting().grid().horizzontalTick().setTickMinor(5);
    chart->setting().grid().verticalTick().setTickMajor(8);
    chart->setting().grid().verticalTick().setTickMinor(5);
    chart->setting().grid().setBackgroundColor(QColor(192,192,192,1));
    chart->setting().grid().setForegroundColor(QColor(210,210,210));
    chart->setting().grid().setBorder(0);
    chart->setting().scale().setAutoCurrentLimit(true);
    chart->setting().scale().setNominalAutoLimit(true);
    chart->updateChart();
}

void statisticswindow::changeEvent(QEvent *e)
{
    if(e->type() == QEvent::LanguageChange)
    {
        ui->retranslateUi(this);
    }
}
