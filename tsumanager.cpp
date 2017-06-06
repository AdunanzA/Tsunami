#include "tsumanager.h"

tsuManager::tsuManager(QObject *parent) : QObject(parent)
{
    p_worker = new Worker;
    p_worker->moveToThread(&workerThread);
//    connect(&workerThread, SIGNAL(finished()), p_worker, SLOT(deleteLater()));

    libtorrent::settings_pack settings;
    settings.set_str(libtorrent::settings_pack::user_agent, "Tsunami/0.0.1");

    p_session = std::make_shared<libtorrent::session>(settings);
    p_session.get()->pause();
    p_timer = new QTimer(this);

    connect(p_timer, SIGNAL(timeout()), this, SLOT(timerTick()));
    connect(p_worker, SIGNAL(eventFromLibtorrent(const tsuEvents::tsuEvent)), this, SLOT(handleWorkerResults(const tsuEvents::tsuEvent)));
    connect(p_worker, SIGNAL(torrentDeletedFromLibtorrent(std::string)), this, SLOT(handleWorkerDeleteTorrent(std::string)));

    p_timer->start(p_timerTick);
    p_session.get()->resume();
}

tsuManager::~tsuManager()
{
    workerThread.quit();
    workerThread.wait();
}

void tsuManager::addTorrent(libtorrent::add_torrent_params atp)
{
    p_session.get()->async_add_torrent(atp);
}

void tsuManager::handleWorkerResults(const tsuEvents::tsuEvent event)
{
    emit updateFromItem(event);
}

void tsuManager::handleWorkerDeleteTorrent(const std::string hash)
{
    emit torrentDeleted(hash);
}

void tsuManager::getCancelRequest(const std::string &hash, const bool deleteFilesToo)
{
    try {
        libtorrent::sha1_hash sh(hash);
        libtorrent::torrent_handle th = p_session.get()->find_torrent(sh);
        p_session.get()->remove_torrent(th, deleteFilesToo);
    } catch (std::exception &exc) {
        qDebug() << QString("tsuManager::getCancelRequest throws %1").arg(exc.what());
    }
}

void tsuManager::timerTick()
{
    std::vector<libtorrent::alert*> alerts;
    p_session.get()->pop_alerts(&alerts);
    p_worker->doWork(alerts);

// //    qDebug() << QString("processing %1 alerts:").arg(alerts.size());
//    for (libtorrent::alert* a : alerts) {
//        if (a == nullptr) continue;
// //        qDebug() << QString(" %1").arg(a->message().c_str());

//        if (auto st = libtorrent::alert_cast<libtorrent::state_update_alert>(a)) {
//            if (st == nullptr) continue;
//            if (st->status.empty()) continue;
//            for (libtorrent::torrent_status s : st->status) {
//                try {
//                    tsuEvents::tsuEvent ev(s.info_hash.to_string(), QString::fromStdString(s.name), s.total_done, s.total_upload, s.download_rate, s.upload_rate, s.total_wanted, s.state, s.progress_ppm);
//                    emit updateFromItem(ev);
//                } catch (std::exception &exc) {
//                    qDebug() << QString("timerTick throws %1").arg(exc.what());
//                }
//            }
//        }

//        if (auto aa = libtorrent::alert_cast<libtorrent::torrent_error_alert>(a)) {
//            qDebug() << QString("torrent_error_alert %1").arg(QString::fromStdString(aa->message()));
//        }

//        if (auto aa = libtorrent::alert_cast<libtorrent::add_torrent_alert>(a)) {
//            libtorrent::torrent_handle th = aa->handle;
//            libtorrent::torrent_status st = th.status(libtorrent::torrent_handle::query_name);
//            tsuEvents::tsuEvent ev(st.info_hash.to_string(), QString::fromStdString(st.name), 0, 0, 0, 0, st.total_wanted, 0, 0);
//            emit updateFromItem(ev);
//        }

//        if (auto aa = libtorrent::alert_cast<libtorrent::torrent_finished_alert>(a)) {
//            libtorrent::torrent_handle th = aa->handle;
//            libtorrent::torrent_status st = th.status(libtorrent::torrent_handle::query_name);
// //            emit sendPopupInfo(QString("%1 finished!").arg(QString::fromStdString(st.name)));
//        }

//        if (auto aa = libtorrent::alert_cast<libtorrent::tracker_error_alert>(a)) {
//            qDebug() << QString("tracker_error_alert %1").arg(QString::fromStdString(aa->message()));
//        }

//        if (auto aa = libtorrent::alert_cast<libtorrent::torrent_deleted_alert>(a)) {
//            emit torrentDeleted(aa->info_hash.to_string());
//        }

//    }
    p_session.get()->post_torrent_updates(libtorrent::alert::status_notification);
}
