#include "tsumanager.h"

int tsuManager::outstanding_resume_data = 0;

tsuManager::tsuManager()
{

    libtorrent::settings_pack settings;
    settings.set_str(libtorrent::settings_pack::user_agent, "Tsunami/0.0.1");

    p_session = QSharedPointer<libtorrent::session>::create(settings);

    p_session->set_alert_notify([this]()
    {
        QTimer::singleShot(0, this, SLOT(alertsHandler()));
    });

    QString fastresumeFolder = QString("%0\\%1").arg(QCoreApplication::applicationDirPath()).arg("fastresume");

    if (!QDir(fastresumeFolder).exists()) {
        QDir().mkdir(fastresumeFolder);
    } else {
        // resuming
        QDirIterator it(fastresumeFolder, QStringList() << "*.fastresume", QDir::Files, QDirIterator::Subdirectories);
        while (it.hasNext()) {
            QString fileName = it.next();
//            qDebug() << QString("%0 exists %1").arg(fileName).arg(QFile::exists(fileName));
            QFile file(fileName);
            file.open(QIODevice::ReadOnly);
            QByteArray buf = file.readAll();
            libtorrent::error_code ec;
            libtorrent::bdecode_node bdn;
            libtorrent::bdecode(buf.constData(), buf.constData() + buf.size(), bdn, ec);
            p_session->async_add_torrent(libtorrent::read_resume_data(bdn, ec));
        }
    }
}

tsuManager::tsuManager(const libtorrent::settings_pack & sp)
{
    p_session = QSharedPointer<libtorrent::session>::create(sp);
}

void tsuManager::alertsHandler()
{
    std::vector<libtorrent::alert*> alerts;
    QVector<tsuEvents::tsuEvent> eventsArray;

    p_session->pop_alerts(&alerts);
//    qDebug() << QString("processing %1 alerts:").arg(alerts.size());
    for (libtorrent::alert* a : alerts)
    {
        if (a == nullptr) continue;
//        qDebug() << QString(" %1").arg(a->message().c_str());

        if (auto st = libtorrent::alert_cast<libtorrent::state_update_alert>(a))
        {
            if (st == nullptr) continue;
            if (st->status.empty()) continue;
            for (libtorrent::torrent_status s : st->status)
            {
                try
                {
                    statusEnum se = static_cast<statusEnum>((int)s.state);
                    if (s.paused) se = statusEnum::paused;
                    tsuEvents::tsuEvent ev(s.info_hash.to_string(), QString::fromStdString(s.name), s.total_done, s.total_upload, s.download_rate, s.upload_rate, s.total_wanted, (int)se, s.progress_ppm);
                    eventsArray.append(ev);
//                    emit updateFromItem(ev);
                }
                catch (std::exception &exc)
                {
                    qDebug() << QString("timerTick throws %0").arg(exc.what());
                }
            }
        }

        if (auto aa = libtorrent::alert_cast<libtorrent::torrent_deleted_alert>(a)) {
            emit torrentDeleted(aa->info_hash.to_string());
            QString fastresumeFolder = QString("%0/%1").arg(QCoreApplication::applicationDirPath()).arg("fastresume");
            std::stringstream hex;
            hex << aa->info_hash;
            QString fileName = QString("%0/%1.fastresume").arg(fastresumeFolder).arg(QString::fromStdString(hex.str()));
//            qDebug() << QString("%0 exists %1").arg(fileName).arg(QFile::exists(fileName));
            if (QFile::exists(fileName)) {
                QFile file(fileName);
                file.remove();
            }
        }

/*        if (auto aa = libtorrent::alert_cast<libtorrent::torrent_paused_alert>(a)) {
            libtorrent::torrent_status s(aa->handle.status());
            tsuEvents::tsuEvent ev(s.info_hash.to_string(), QString::fromStdString(s.name), s.total_done, s.total_upload, s.download_rate, s.upload_rate, s.total_wanted, statusEnum::paused, s.progress_ppm);
            eventsArray.append(ev);
        }

        if (auto aa = libtorrent::alert_cast<libtorrent::torrent_resumed_alert>(a)) {
            libtorrent::torrent_status s(aa->handle.status());
            tsuEvents::tsuEvent ev(s.info_hash.to_string(), QString::fromStdString(s.name), s.total_done, s.total_upload, s.download_rate, s.upload_rate, s.total_wanted, s.state, s.progress_ppm);
            eventsArray.append(ev);
        }*/

        if (!eventsArray.isEmpty()) {
            emit updateFromItem(eventsArray);
        }

    }
    p_session->post_torrent_updates(libtorrent::alert::status_notification);
}

tsuManager::~tsuManager()
{
    std::vector<libtorrent::torrent_handle> handles = p_session->get_torrents();
    p_session->pause();
    for (libtorrent::torrent_handle i : handles)
    {
//        qDebug() << QString("Handling %0 handlers").arg(handles.size());
        libtorrent::torrent_handle &h = i;
        if (!h.is_valid()) continue;
        libtorrent::torrent_status s = h.status();
        if (!s.has_metadata) continue;
        if (!s.need_save_resume) continue;
        h.save_resume_data();
        tsuManager::outstanding_resume_data++;
    }

    while (tsuManager::outstanding_resume_data > 0)
    {
//        qDebug() << QString("outstanding_resume_data = %0").arg(outstanding_resume_data);
        libtorrent::alert const* a = p_session->wait_for_alert(libtorrent::seconds(10));

        // if we don't get an alert within 10 seconds, abort
        if (a == 0) continue;

        std::vector<libtorrent::alert*> alerts;
        p_session->pop_alerts(&alerts);

        for (libtorrent::alert* a : alerts)
        {
            if (libtorrent::alert_cast<libtorrent::save_resume_data_failed_alert>(a))
            {
                tsuManager::outstanding_resume_data--;
                continue;
            }

            libtorrent::save_resume_data_alert const* rd = libtorrent::alert_cast<libtorrent::save_resume_data_alert>(a);
            if (rd == 0)
            {
                continue;
            }

            libtorrent::torrent_handle h = rd->handle;
            libtorrent::torrent_status st = h.status(libtorrent::torrent_handle::query_save_path | libtorrent::torrent_handle::query_name);
            libtorrent::entry &en = libtorrent::write_resume_data(rd->params);
            QString fastresumeFolder = QString("%0/%1").arg(QCoreApplication::applicationDirPath()).arg("fastresume");
            std::stringstream hex;
            hex << st.info_hash;
            QString fileName = QString("%0/%1.fastresume").arg(fastresumeFolder).arg(QString::fromStdString(hex.str()));
            std::ofstream out(fileName.toStdString(), std::ios_base::binary);
            out.unsetf(std::ios_base::skipws);
            libtorrent::bencode(std::ostream_iterator<char>(out), en);
            tsuManager::outstanding_resume_data--;
        }
    }
}

void tsuManager::addItems(const QStringList && items, const QString &path)
{
    foreach (const QString &str, items)
    {
        try
        {
            libtorrent::add_torrent_params atp;
            libtorrent::torrent_info ti(str.toStdString());

            if (!ti.metadata()) qDebug("NO METADATA!");
            if (!ti.is_valid()) qDebug("INVALID!");
            atp.ti = std::make_shared<libtorrent::torrent_info>(ti);
            atp.save_path = path.toStdString();

            atp.flags &= ~libtorrent::add_torrent_params::flag_paused; // Start in pause
            atp.flags &= ~libtorrent::add_torrent_params::flag_auto_managed; // Because it is added in paused state
            atp.flags &= ~libtorrent::add_torrent_params::flag_duplicate_is_error; // Already checked

            p_session->async_add_torrent(atp);
        }
        catch (std::exception &exc)
        {
            qDebug() << QString("addItems throws %0").arg(exc.what());
        }
    }
}

void tsuManager::getCancelRequest(const std::string &hash, const bool deleteFilesToo)
{
    try {
        libtorrent::sha1_hash sh(hash);
        libtorrent::torrent_handle th = p_session->find_torrent(sh);
        p_session->remove_torrent(th, deleteFilesToo);
    } catch (std::exception &exc) {
        qDebug() << QString("tsuManager::getCancelRequest throws %0").arg(exc.what());
        emit torrentDeleted(hash);
    }
}

void tsuManager::getPauseRequest(const std::string &hash)
{
    try {
        libtorrent::sha1_hash sh(hash);
        libtorrent::torrent_handle th = p_session->find_torrent(sh);
        th.pause();
    } catch (std::exception &exc) {
        qDebug() << QString("tsuManager::getPauseRequest throws %0").arg(exc.what());
    }
}

void tsuManager::getResumeRequest(const std::string &hash)
{
    try {
        libtorrent::sha1_hash sh(hash);
        libtorrent::torrent_handle th = p_session->find_torrent(sh);
        th.resume();
    } catch (std::exception &exc) {
        qDebug() << QString("tsuManager::getResumeRequest throws %0").arg(exc.what());
    }
}

/*

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
*/
