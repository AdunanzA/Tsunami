#ifndef DOWNLOADWINDOW_H
#define DOWNLOADWINDOW_H

#include <QtGui>
#include <QWidget>
#include <QTimer>
#include <QVector>

#include "flowlayout.h"
#include "tsucard.h"

#include <libtorrent/session.hpp>
#include <libtorrent/torrent_status.hpp>
#include <libtorrent/alert_types.hpp>
#include "libtorrent/torrent_handle.hpp"
#include "libtorrent/torrent_info.hpp"
#include <libtorrent/session_status.hpp>

//struct my_lt libtorrent;

namespace Ui {
    class downloadwindow;
}

namespace lt = libtorrent;

class downloadwindow : public QWidget
{
    Q_OBJECT

public:
    explicit downloadwindow(QWidget *parent = 0);
    ~downloadwindow();

    void addTorrent(lt::add_torrent_params atp);

private:
    Ui::downloadwindow *ui;
    FlowLayout *flowLayout;
    QTimer *timer;
    lt::session p_session;
    QVector<tsucard *> tsulist;
    int downRate = 0;
    int upRate = 0;
    int totalDownload = 0;
    int totalUpload = 0;

private slots:
    void requestedCancel(tsucard *sender);
    void timerTick();

signals:
    void sendUpdateToStatusBar(const QString & msg);
    void sendUpdateGauge(const double & value);
    void sendStatisticsUpdate(const QPair<int, int> & values);
    void sendPopupInfo(const QString & msg);
};

#endif // DOWNLOADWINDOW_H
