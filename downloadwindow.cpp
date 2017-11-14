#include "downloadwindow.h"
#include "ui_downloadwindow.h"

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

    p_contextMenu = new QMenu(this);
    p_contextMenu->addAction("Select all", this, SLOT(selectAll()));
    p_contextMenu->addAction("Deselect all", this, SLOT(deselectAll()));
    p_contextMenu->addSeparator();
    p_contextMenu->addAction("Pause selected", this, SLOT(pauseSelected()));
    p_contextMenu->addAction("Resume selected", this, SLOT(resumeSelected()));
    p_contextMenu->addSeparator();
    p_contextMenu->addAction("Delete selected...", this, SLOT(deleteSelected()));

    redrawItemsPosition();

    // SHOW CONTEXT MENU
    connect(ui->graphicsView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showContextMenu(QPoint)));

    // IF FILE DROPPED IN GRAPHICSSCENE EMIT SIGNAL TO MANAGE IN MAINWINDOW
    connect(p_scene, &tsugraphicsscene::fileDropped, this, &downloadwindow::fileDroppedFromGraphicScene);

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

void downloadwindow::updateVisibility()
{
    bool showDownloading = qApp->property("showDownloading").toBool();
    bool showUploading   = qApp->property("showUploading").toBool();
    bool showChecking    = qApp->property("showChecking").toBool();
    bool showPaused      = qApp->property("showPaused").toBool();
    bool showError       = qApp->property("showError").toBool();

    for (int i = 0; i < p_tsulist.count(); i++) {
        tsuItem *ti = p_tsulist[i];

        if ( ti->get_Status() == statusEnum::downloading || ti->get_Status() == statusEnum::downloading_metadata ) {
            ti->set_Visibility(showDownloading);
        }

        if (ti->get_Status() == statusEnum::seeding || ti->get_Status() == statusEnum::finished) {
            ti->set_Visibility(showUploading);
        }

        if (ti->get_Status() == statusEnum::allocating || ti->get_Status() == statusEnum::checking_files || ti->get_Status() == statusEnum::checking_resume_data) {
            ti->set_Visibility(showChecking);
        }

        if (ti->get_Status() == statusEnum::paused) {
            ti->set_Visibility(showPaused);
        }

        if (ti->get_Status() == statusEnum::undefined) {
            ti->set_Visibility(showError);
        }
    }
    redrawItemsPosition();
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

void downloadwindow::downloadFinished(tsuItem *item)
{
    qDebug() << QString("received download finished for %0").arg(item->get_Head());
    bool emitMsg = qApp->property("msgOnFinish").toBool();
    if (emitMsg)
        emit sendPopupInfo("download finished for " + item->get_Head());
}

void downloadwindow::showContextMenu(const QPoint &pos)
{
//    qDebug("requested context menu");
    QPoint globalPos = this->mapToGlobal(pos);
    p_contextMenu->exec(globalPos);
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
    foreach (tsuItem* item, p_tsulist) {
        if (item->get_Hash() == event.hash) {
            qDebug() << QString("torrent %0 already in list, ignoring").arg(event.name);
            return;
        }
    }
    tsuItem *ts = new tsuItem(event.hash);
    connect(ts, SIGNAL(cancelRequested(std::string,bool)), this, SLOT(requestedCancel(std::string,bool)), Qt::QueuedConnection);
    connect(ts, SIGNAL(pauseRequested(std::string)), this, SLOT(requestedPause(std::string)), Qt::QueuedConnection);
    connect(ts, SIGNAL(resumeRequested(std::string)), this, SLOT(requestedResume(std::string)), Qt::QueuedConnection);
    connect(ts, SIGNAL(detailsRequested(std::string)), this, SLOT(requestedDetails(std::string)), Qt::QueuedConnection);
    connect(ts, SIGNAL(downloadFinished(tsuItem*)), this, SLOT(downloadFinished(tsuItem*)), Qt::QueuedConnection);
    ts->setValue(event);
    ts->set_FactorTransform(p_transformFactor);
    p_tsulist.append(ts);
    p_scene->addItem(ts);
    updateVisibility();
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
        tsuItem *ti = p_tsulist[i];

        if (ti->get_Visibility()) {
            if (col == p_itemsPerRow-1) { row++; col = 0; } else { col++; }
            int newX = (col*itemWidth)+tsuItem::ItemGlowRadius;
            int newY = (row*itemHeight)+tsuItem::ItemGlowRadius;
//            ti->setPos(QPointF(newX, newY));

            QPropertyAnimation *a = new QPropertyAnimation(ti, "pos");
            a->setDuration(300);
            a->setStartValue(QPointF(ti->pos().x(), ti->pos().y()));
            a->setEndValue(QPoint(newX, newY));
            a->setEasingCurve(QEasingCurve::Linear);
            a->start(QPropertyAnimation::DeleteWhenStopped);

        }
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

void downloadwindow::fileDroppedFromGraphicScene(QString fileName)
{
    emit fileDropped(fileName);
}
