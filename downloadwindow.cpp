#include "downloadwindow.h"
#include "ui_downloadwindow.h"
#include "tsucard.h"

downloadwindow::downloadwindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::downloadwindow),
    flowLayout(new FlowLayout)
{
    ui->setupUi(this);
    setLayout(flowLayout);

}

downloadwindow::~downloadwindow()
{
    delete ui;
}

void downloadwindow::addTorrent(QString name) {
    qDebug() << QString("addTorrent(%1) called from %2").arg(name).arg(this->objectName());
    tsucard *ts = new tsucard(this);
    ts->set_Name(name);
    connect(ts, SIGNAL(cancelRequested(tsucard*)), this, SLOT(requestedCancel(tsucard*)));
    flowLayout->addWidget(ts);
}

void downloadwindow::requestedCancel(tsucard *sender)
{
    qDebug() << QString("requestedCancel for %1").arg(sender->get_Name());
    flowLayout->removeWidget(sender);
    sender->close();
}




