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

void tsucard::set_Name(const QString name)
{
    ui->lblTitle->setText(name);
}

QString tsucard::get_Name() const
{
    return ui->lblTitle->text();
}

libtorrent::sha1_hash tsucard::get_Hash() const
{
    return p_hash;
}

void tsucard::set_Hash(const libtorrent::sha1_hash &value)
{
    p_hash = value;
}

void tsucard::set_State(const QString actual_state)
{
    QString ss = "background-image: url(:/images/%1.png);";
    QString stateImage = "state_pause";

    if (actual_state == "checking" || actual_state == "checking resume" || actual_state == "<>") {
        stateImage = "state_warning"; }
    else if (actual_state == "dl metadata" || actual_state == "downloading") {
        stateImage = "state_download"; }
    else if (actual_state == "seeding" || actual_state == "allocating") {
        stateImage = "state_loading"; }
    else if (actual_state == "finished") {
        stateImage = "finished";
    }

    ui->iconState->setStyleSheet(ss.arg(stateImage));
    ui->txtState->setText(actual_state);
}

void tsucard::set_Total(const int qty)
{
    total_size = qty;
    ui->txtTotal->setText(InfoSize(qty));
    ui->txtTotalUnit->setText(InfoUnit(qty));
}

void tsucard::set_Downloaded(const int qty)
{
    p_downloaded = qty;
    ui->txtDownloaded->setText(InfoSize(qty));
    ui->txtDownloadedUnit->setText(InfoUnit(qty));
    ui->txtRemaining->setText(InfoSize(total_size-qty));
    ui->txtRemainingUnit->setText(InfoUnit(total_size-qty));
}

void tsucard::set_Uploaded(const int qty)
{
    p_uploaded = qty;
}

void tsucard::set_Percentage(const int qty)
{
    ui->progress->setValue(qty);
}

void tsucard::set_DownloadRate(const int qty)
{
    p_downloadRate = qty;
    ui->txtDownloadRate->setText(InfoSize(qty));
    ui->txtDownloadRateUnit->setText(InfoUnit(qty));
}

void tsucard::set_UploadRate(const int qty)
{
    p_uploadRate = qty;
    ui->txtUploadRate->setText(InfoSize(qty));
    ui->txtUploadRateUnit->setText(InfoUnit(qty));
}

int tsucard::get_Downloaded()
{
    return p_downloaded;
}

int tsucard::get_Uploaded()
{
    return p_uploaded;
}

int tsucard::get_DownloadRate()
{
    return p_downloadRate;
}

int tsucard::get_UploadRate()
{
    return p_uploadRate;
}

QString tsucard::InfoSize(int value)
{
    float num = value;
    QStringList list;
    list << "KB" << "MB" << "GB" << "TB";

    QStringListIterator i(list);
    QString unit("b");

    while(num >= 1024.0 && i.hasNext())
    {
     unit = i.next();
     num /= 1024.0;
    }
    return QString().setNum(num,'f',0);//+" "+unit;
}

QString tsucard::InfoUnit(int value)
{
    float num = value;
    QStringList list;
    list << "KB" << "MB" << "GB" << "TB";

    QStringListIterator i(list);
    QString unit("b");

    while(num >= 1024.0 && i.hasNext())
    {
     unit = i.next();
     num /= 1024.0;
    }
    //return QString().setNum(num,'f',2)+" "+unit;
    return unit;
}

void tsucard::btnCancelClick()
{
    emit cancelRequested(this);
}

