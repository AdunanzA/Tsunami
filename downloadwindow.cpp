#include "downloadwindow.h"
#include "ui_downloadwindow.h"

//#include "mainwindow.h"

#define TICKER_TIME 1000

// return the name of a torrent status enum
//char const* state(libtorrent::torrent_status::state_t s)
//{
//  switch(s) {
//    case libtorrent::torrent_status::checking_files: return "checking";
//    case libtorrent::torrent_status::downloading_metadata: return "dl metadata";
//    case libtorrent::torrent_status::downloading: return "downloading";
//    case libtorrent::torrent_status::finished: return "finished";
//    case libtorrent::torrent_status::seeding: return "seeding";
//    case libtorrent::torrent_status::allocating: return "allocating";
//    case libtorrent::torrent_status::checking_resume_data: return "checking resume";
//    default: return "<>";
//  }
//}

//MainWindow *p_mw;

downloadwindow::downloadwindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::downloadwindow)
{
    ui->setupUi(this);
    p_scene = new QGraphicsScene(this);
    p_itemDetails = new itemDetails(this);

    ui->graphicsView->setBackgroundBrush(QColor(23,23,23));
    ui->graphicsView->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    ui->graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui->graphicsView->setRenderHint(QPainter::Antialiasing);
    ui->graphicsView->setScene(p_scene);

    redrawItemsPosition();

    p_sessionStatisticTimer = new QTimer(this);
    connect(p_sessionStatisticTimer, SIGNAL(timeout()), this, SLOT(updateSessionStatistics()));
    p_sessionStatisticTimer->start(TICKER_TIME);

//    p_mw = reinterpret_cast<MainWindow*>(parent);
//    p_mw = (MainWindow*)parent;

}

downloadwindow::~downloadwindow()
{
    delete ui;
}

void downloadwindow::requestedCancel(const std::string &hash, const bool &deleteFilesToo)
{
    emit sendRequestedCancelToSession(hash, deleteFilesToo);
}

void downloadwindow::requestedPause(const std::string &hash)
{
    emit sendRequestedPauseToSession(hash);
}

void downloadwindow::requestedResume(const std::string &hash)
{
    emit sendRequestedResumeToSession(hash);
}

void downloadwindow::requestedDetails(const std::string &hash)
{
//    lt::sha1_hash sh(hash);
//    lt::session *ses = p_mw->sessionManager->p_session.data();
//    lt::torrent_handle th = ses->find_torrent(sh);

//    if (th.is_valid()) {
//        p_itemDetails->setValues(th);
//        p_itemDetails->exec();
//    }
    p_itemDetails->setHash(hash);
    p_itemDetails->exec();
}

void downloadwindow::updateSessionStatistics()
{
    try {
        p_downRate = 0;
        p_upRate = 0;
        foreach (tsuItem* item, p_tsulist) {
            p_totalDownload += qFabs(item->get_Downloaded());
            p_totalUpload += qFabs(item->get_Uploaded());
            p_downRate += qFabs(item->get_RateDownload());
            p_upRate += qFabs(item->get_RateUpload());
        }

        // update mainwindow statusbar message
//        if (p_downRate > 0 || p_upRate > 0 || p_totalDownload > 0 || p_totalUpload > 0) {
        tsuItem ts;
        QString ups = QString("%1%2/s (%3%4) / %5%6/s (%7%8)").arg(ts.convertSize(p_downRate)).arg(ts.convertSizeUnit(p_downRate))
                                                              .arg(ts.convertSize(p_totalDownload)).arg(ts.convertSizeUnit(p_totalDownload))
                                                              .arg(ts.convertSize(p_upRate)).arg(ts.convertSizeUnit(p_upRate))
                                                              .arg(ts.convertSize(p_totalUpload)).arg(ts.convertSizeUnit(p_totalUpload));
        emit sendUpdateToStatusBar(ups);
//        }

        // update mainwindow gauge
        double val = (p_downRate * 8)/1000000; // Ethernet 100 BASE-T -> http://www.convert-me.com/en/convert/data_transfer_rate/byte_s.html?u=byte%2Fs&v=1
        emit sendUpdateGauge(val);

        // update statistics page
        emit sendStatisticsUpdate(QPair<int, int>(p_downRate, p_upRate));

    } catch (std::exception &exc) {
        qDebug() << QString("updateSessionStatistics throws %0").arg(exc.what());
    }

}

void downloadwindow::updateFromSession(const QVector<tsuEvents::tsuEvent> &events)
{
    try {
        for (tsuEvents::tsuEvent event : events) {
//            qDebug() << QString("updateFromSession received for %0, size %1").arg(event.name).arg(event.total_size);
            foreach (tsuItem* item, p_tsulist) {
                if ( item->get_Hash() == event.hash ) {
                    item->setValue(event);
                    continue;
                }
            }
        }
    } catch (std::exception &exc) {
        qDebug() << QString("updateFromSession throws %0").arg(exc.what());
    }
}

void downloadwindow::addFromSession(const tsuEvents::tsuEvent &event)
{
//    qDebug() << QString("received add for '%0' - '%1'").arg(event.name).arg(QString::fromStdString(event.hash));
    tsuItem *ts = new tsuItem(event.hash);
    connect(ts, SIGNAL(cancelRequested(std::string,bool)), this, SLOT(requestedCancel(std::string,bool)), Qt::QueuedConnection);
    connect(ts, SIGNAL(pauseRequested(std::string)), this, SLOT(requestedPause(std::string)), Qt::QueuedConnection);
    connect(ts, SIGNAL(resumeRequested(std::string)), this, SLOT(requestedResume(std::string)), Qt::QueuedConnection);
    connect(ts, SIGNAL(detailsRequested(std::string)), this, SLOT(requestedDetails(std::string)), Qt::QueuedConnection);
    ts->setValue(event);
    p_tsulist.append(ts);
    p_scene->addItem(ts);
    redrawItemsPosition();
}

void downloadwindow::torrentDeletedFromSession(const std::string &hash)
{
    try {
        foreach (tsuItem* item, p_tsulist) {
            if ( item->get_Hash() == hash ) {
                item->deleteLater();
                p_tsulist.removeAll(item);
                redrawItemsPosition();
                return;
            }
        }
    } catch (std::exception &exc) {
        qDebug() << QString("downloadwindow::torrentDeletedFromSession throws %0").arg(exc.what());
    }
}

void downloadwindow::redrawItemsPosition()
{
    int row = -1;
    int col = -1;
    int itemWidth = tsuItem::ItemGlowRadius + tsuItem::ItemWidth;
    int itemHeight = tsuItem::ItemGlowRadius + tsuItem::ItemHeight;
    for (int i = 0; i < p_tsulist.count(); i++) {
//        if (i % itemsPerRow == 0) { row++; col = 0; } else { col++; }
        if (col == p_itemsPerRow-1) { row++; col = 0; } else { col++; }
        tsuItem *ti = p_tsulist[i];
//        qDebug() << QString("item %0 -> row %1, col %2").arg(i).arg(row).arg(col);
        ti->setPos(QPointF((col*itemWidth)+tsuItem::ItemGlowRadius, (row*itemHeight)+tsuItem::ItemGlowRadius));
    }
}

void downloadwindow::resizeEvent(QResizeEvent *event)
{
    p_itemsPerRow = floor(event->size().width()/(tsuItem::ItemWidth + tsuItem::ItemGlowRadius));
    redrawItemsPosition();
}

void downloadwindow::changeEvent(QEvent *e)
{
    if(e->type() == QEvent::LanguageChange)
    {
        ui->retranslateUi(this);
    }
}
