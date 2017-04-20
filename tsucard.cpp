#include "tsucard.h"
#include "ui_tsucard.h"

tsucard::tsucard(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::tsucard)
{
    ui->setupUi(this);
    connect(ui->btnCancel, SIGNAL(released()), this, SLOT(btnCancelClick()));
    setAttribute(Qt::WA_DeleteOnClose);
}

tsucard::~tsucard()
{
    delete ui;
}

void tsucard::set_Name(QString name)
{
    p_name = name;
    ui->lblTitle->setText(p_name);
}

QString tsucard::get_Name()
{
    return p_name;
}

void tsucard::btnCancelClick()
{
    //emit cancelRequested(this->get_Name());
    emit cancelRequested(this);
}
