#include "archivewindow.h"
#include "ui_archivewindow.h"

archivewindow::archivewindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::archivewindow)
{
    ui->setupUi(this);

    p_treeWidget = new tsuTreeWidget(this);

    ui->content->addWidget(p_treeWidget);
}

archivewindow::~archivewindow()
{
    delete ui;
    qDebug("archivewindow destroyed");
}

void archivewindow::changeEvent(QEvent *e)
{
    if(e->type() == QEvent::LanguageChange)
    {
        qDebug("received QEvent::LanguageChange, retranslateUi");
        ui->retranslateUi(this);
    }
}
