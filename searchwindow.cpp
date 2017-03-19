#include "searchwindow.h"
#include "ui_searchwindow.h"

searchwindow::searchwindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::searchwindow)
{
    ui->setupUi(this);
}

searchwindow::~searchwindow()
{
    delete ui;
}
