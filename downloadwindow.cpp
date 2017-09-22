#include "downloadwindow.h"
#include "ui_downloadwindow.h"

//#include "mainwindow.h"

//#define TICKER_TIME 1000

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
    p_scene = new tsugraphicsscene(this);
    p_itemDetails = new itemDetails(this);

    ui->graphicsView->setBackgroundBrush(QColor(23,23,23));
    ui->graphicsView->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    ui->graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui->graphicsView->setRenderHint(QPainter::Antialiasing);
    ui->graphicsView->setScene(p_scene);

    ui->graphicsView->ensureVisible(QRectF(0, 0, 0, 0));
    ui->graphicsView->setDragMode(QGraphicsView::RubberBandDrag);
    ui->graphicsView->setInteractive(true);

    redrawItemsPosition();

//    p_sessionStatisticTimer = new QTimer(this);
//    connect(p_sessionStatisticTimer, SIGNAL(timeout()), this, SLOT(updateSessionStatistics()));
//    p_sessionStatisticTimer->start(TICKER_TIME);

    // SHOW CONTEXT MENU
    connect(ui->graphicsView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showContextMenu(QPoint)));

    qDebug("downloadwindow created");
}

downloadwindow::~downloadwindow()
{
    delete ui;
    qDebug("downloadwindow destroyed");
}

QPair<float, float> downloadwindow::getRate()
{
    float dRate = 0;
    float uRate = 0;

    foreach (tsuItem* item, p_tsulist) {
        dRate += qFabs(item->get_RateDownload());
        uRate += qFabs(item->get_RateUpload());
    }

    return QPair<int, int>(dRate, uRate);
}

void downloadwindow::requestedCancel(const std::string &hash, const bool &deleteFilesToo)
{
    qDebug() << QString("requestedCancel for %0 delete file too %1, emitting %2")
                        .arg(QString::fromStdString(hash))
                        .arg(deleteFilesToo)
                        .arg("sendRequestedCancelToSession");
    emit sendRequestedCancelToSession(hash, deleteFilesToo);
}

void downloadwindow::requestedPause(const std::string &hash)
{
    qDebug() << QString("requestedPause for %0, emitting %1")
                        .arg(QString::fromStdString(hash))
                        .arg("sendRequestedPauseToSession");
    emit sendRequestedPauseToSession(hash);
}

void downloadwindow::requestedResume(const std::string &hash)
{
    qDebug() << QString("requestedResume for %0, emitting %1")
                        .arg(QString::fromStdString(hash))
                        .arg("sendRequestedResumeToSession");
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
    qDebug() << QString("requestedDetails for %0, opening details").arg(QString::fromStdString(hash));
    p_itemDetails->setHash(hash);
    p_itemDetails->exec();
}

void downloadwindow::showContextMenu(const QPoint &pos)
{
//    qDebug("requested context menu");
    QPoint globalPos = this->mapToGlobal(pos);

    QMenu contextMenu;
    contextMenu.addAction("Select all", this, SLOT(selectAll()));
    contextMenu.addAction("Deselect all", this, SLOT(deselectAll()));
    contextMenu.addSeparator();
    contextMenu.addAction("Pause selected", this, SLOT(pauseSelected()));
    contextMenu.addAction("Resume selected", this, SLOT(resumeSelected()));
    contextMenu.addSeparator();
    contextMenu.addAction("Delete selected...", this, SLOT(deleteSelected()));
    contextMenu.exec(globalPos);
}

void downloadwindow::selectAll()
{
    foreach (tsuItem* item, p_tsulist) {
        item->setSelected(true);
    }
}

void downloadwindow::deselectAll()
{
    foreach (tsuItem* item, p_tsulist) {
        item->setSelected(false);
    }
}

void downloadwindow::pauseSelected()
{
    QVector<tsuItem *> listSelected;

    foreach (tsuItem* item, p_tsulist) {
        if (item->isSelected()) {
            listSelected.append(item);
        }
    }

    if (listSelected.length() == 0) {
        emit sendMessageToStatusBar(QString("No item selected"));
        return;
    }

    foreach (tsuItem* item, listSelected) {
        item->executePause();
    }
    emit sendMessageToStatusBar(QString("%0 torrent paused").arg(listSelected.length()));
}

void downloadwindow::resumeSelected()
{
    QVector<tsuItem *> listSelected;

    foreach (tsuItem* item, p_tsulist) {
        if (item->isSelected()) {
            listSelected.append(item);
        }
    }

    if (listSelected.length() == 0) {
        emit sendMessageToStatusBar(QString("No item selected"));
        return;
    }

    foreach (tsuItem* item, listSelected) {
        item->executeResume();
    }
    emit sendMessageToStatusBar(QString("%0 torrent resumed").arg(listSelected.length()));
}

void downloadwindow::deleteSelected()
{
    QVector<tsuItem *> listSelected;

    foreach (tsuItem* item, p_tsulist) {
        if (item->isSelected()) {
            listSelected.append(item);
        }
    }

    if (listSelected.length() == 0) {
        emit sendMessageToStatusBar(QString("No item selected"));
        return;
    }

    QMessageBox mbox;
    QString msg = QString("<center>Do you really want to cancel <b>%0</b> torrents from download?</center>").arg(listSelected.length());
    mbox.setText(msg);
    mbox.setStandardButtons( QMessageBox::Yes | QMessageBox::No );

    mbox.addButton("Yes and delete files too", QMessageBox::YesRole);

    bool proceedWithCancel = false;
    bool cancelFilesOnDelete = false;

    switch (mbox.exec()) {
        case QMessageBox::Yes:
            proceedWithCancel = true;
            break;
        case QMessageBox::No:
            proceedWithCancel = false;
            break;
        default:
            proceedWithCancel = true;
            cancelFilesOnDelete = true;
            break;
    }

    if (proceedWithCancel) {
        foreach (tsuItem* item, listSelected) {
            qDebug() << "requesting remove for" << item->get_Head();
            item->executeItemRemove(cancelFilesOnDelete);
        }
        qInfo() << QString("requested remove for %0 torrents").arg(listSelected.length());
    }
    emit sendMessageToStatusBar(QString("%0 torrent removed").arg(listSelected.length()));

}

//void downloadwindow::updateSessionStatistics()
//{
//    try {
//        p_downRate = 0;
//        p_upRate = 0;
//        foreach (tsuItem* item, p_tsulist) {
//            p_totalDownload += qFabs(item->get_Downloaded());
//            p_totalUpload += qFabs(item->get_Uploaded());
//            p_downRate += qFabs(item->get_RateDownload());
//            p_upRate += qFabs(item->get_RateUpload());
//        }

//        // update mainwindow statusbar message
//        tsuItem ts;
//        QString ups = QString("%1%2/s (%3%4) / %5%6/s (%7%8)").arg(ts.convertSize(p_downRate)).arg(ts.convertSizeUnit(p_downRate))
//                                                              .arg(ts.convertSize(p_totalDownload)).arg(ts.convertSizeUnit(p_totalDownload))
//                                                              .arg(ts.convertSize(p_upRate)).arg(ts.convertSizeUnit(p_upRate))
//                                                              .arg(ts.convertSize(p_totalUpload)).arg(ts.convertSizeUnit(p_totalUpload));
//        emit sendUpdateToStatusBar(ups);

//        // update mainwindow gauge
//        double dval = (p_downRate * 8)/1000000; // Ethernet 100 BASE-T -> http://www.convert-me.com/en/convert/data_transfer_rate/byte_s.html?u=byte%2Fs&v=1
//        double uval = (p_upRate * 8)/1000000;
//        emit sendUpdateGauge(dval, uval);

//        // update statistics page
//        emit sendStatisticsUpdate(QPair<int, int>(p_downRate, p_upRate));

//    } catch (std::exception &exc) {
//        qDebug() << QString("updateSessionStatistics throws %0").arg(exc.what());
//    }
//}

void downloadwindow::updateFromSession(const QVector<tsuEvents::tsuEvent> &events)
{
    if (p_itemDetails->isVisible())
        return;

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
        qCritical() << QString("updateFromSession throws %0").arg(exc.what());
    }
}

void downloadwindow::addFromSession(const tsuEvents::tsuEvent &event)
{
    qDebug() << QString("received addFromSession for '%0' - '%1'").arg(event.name).arg(QString::fromStdString(event.hash));
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
    qDebug() << QString("received torrentDeletedFromSession for '%0'").arg(QString::fromStdString(hash));
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
//    qDebug("redrawing tsuCard position");
    p_itemsPerRow = floor(ui->graphicsView->size().width()/((tsuItem::ItemWidth + tsuItem::ItemGlowRadius) * p_transformFactor));
    int row = -1;
    int col = -1;
    int itemWidth = (tsuItem::ItemGlowRadius + tsuItem::ItemWidth) * p_transformFactor;
    int itemHeight = (tsuItem::ItemGlowRadius + tsuItem::ItemHeight) * p_transformFactor;
    for (int i = 0; i < p_tsulist.count(); i++) {
        if (col == p_itemsPerRow-1) { row++; col = 0; } else { col++; }
        tsuItem *ti = p_tsulist[i];
        ti->setPos(QPointF((col*itemWidth)+tsuItem::ItemGlowRadius,
                           (row*itemHeight)+tsuItem::ItemGlowRadius));
    }
    ui->graphicsView->ensureVisible(QRectF(0, 0, 0, 0));
}

void downloadwindow::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
    redrawItemsPosition();
}

void downloadwindow::changeEvent(QEvent *e)
{
    if(e->type() == QEvent::LanguageChange)
    {
        qDebug("received QEvent::LanguageChange, retranslateUi");
        ui->retranslateUi(this);
    }
}

//void downloadwindow::mousePressEvent(QMouseEvent *event)
//{
//    if (event->button() == Qt::RightButton) {
//        qDebug("right!");
//        return;
//    } else {
//        qDebug("else");
//        QWidget::mousePressEvent(event);
//    }
//}


//void downloadwindow::wheelEvent(QWheelEvent *event)
//{
//    if (event->modifiers() == Qt::ControlModifier) {
//        if (p_tsulist.isEmpty()) return;

//        QPoint numDegrees = event->angleDelta() / 8;
//        if (!numDegrees.isNull()) {
//            QPoint numSteps = numDegrees / 15;
//            int value = numSteps.y();

//            if ((value > 0 && p_transformFactor >= 3) || (value < 0 && p_transformFactor <= 1)) return;
//            p_transformFactor += value;
//            qDebug() << QString("setting transformation to %0").arg(QString::number(p_transformFactor));

//            foreach (tsuItem* item, p_tsulist) {
//                item->set_FactorTransform(p_transformFactor);
//            }
//            redrawItemsPosition();
//            event->ignore();
//            return;
//        }
//    }
//}

void downloadwindow::on_btnZoomIn_released()
{
    if (p_transformFactor == 3) return;
    p_transformFactor += 0.25;
    foreach (tsuItem* item, p_tsulist) {
        item->set_FactorTransform(p_transformFactor);
    }
    redrawItemsPosition();
}

void downloadwindow::on_btnZoomOut_released()
{
    if (p_transformFactor == 1) return;
    p_transformFactor -= 0.25;
    foreach (tsuItem* item, p_tsulist) {
        item->set_FactorTransform(p_transformFactor);
    }
    redrawItemsPosition();
}
