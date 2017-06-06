#include "settingswindow.h"
#include "ui_settingswindow.h"

#include <QSettings>

settingswindow::settingswindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::settingswindow)
{
    ui->setupUi(this);

//    QSettings settings;
//    p_settingsFile = QString("%0:/%1.ini").arg(QApplication::applicationDirPath().left(1)).arg(PROJECT_NAME);

}

settingswindow::~settingswindow()
{
    delete ui;
}

void settingswindow::loadSettings()
{

}

void settingswindow::saveSettings()
{

}
