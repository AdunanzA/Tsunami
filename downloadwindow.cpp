#include "downloadwindow.h"
#include "ui_downloadwindow.h"

downloadwindow::downloadwindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::downloadwindow)
{
    ui->setupUi(this);
}

downloadwindow::~downloadwindow()
{
    delete ui;
}
