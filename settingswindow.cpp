#include "settingswindow.h"
#include "ui_settingswindow.h"

settingswindow::settingswindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::settingswindow)
{
    ui->setupUi(this);
}

settingswindow::~settingswindow()
{
    delete ui;
}
