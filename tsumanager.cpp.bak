#include "tsumanager.h"

int tsuManager::outstanding_resume_data = 0;

tsuManager::tsuManager()
{
    lt::settings_pack settings;

    QString user_agent = QString("Tsunami/%0.%1.%2").arg(VERSION_MAJOR).arg(VERSION_MINOR).arg(VERSION_BUGFIX);
    settings.set_str(lt::settings_pack::user_agent, user_agent.toStdString());

    // from picotorrent (and qbittorrent)
    settings.set_str(lt::settings_pack::string_types::dht_bootstrap_nodes,
        "router.bittorrent.com:6881" ","
        "router.utorrent.com:6881" ","
        "dht.transmissionbt.com:6881" ","
        "dht.aelitis.com:6881");

    // from qbittorrent
    settings.set_bool(lt::settings_pack::upnp_ignore_nonrouters, true);
    settings.set_bool(lt::settings_pack::announce_to_all_trackers, true);
    settings.set_bool(lt::settings_pack::announce_to_all_tiers, true);
    settings.set_bool(lt::settings_pack::enable_incoming_utp, true);
    settings.set_bool(lt::settings_pack::enable_outgoing_utp, true);
    settings.set_bool(lt::settings_pack::use_dht_as_fallback, false);
    settings.set_bool(lt::settings_pack::anonymous_mode, false);
    settings.set_bool(lt::settings_pack::dont_count_slow_torrents, false);
    settings.set_bool(lt::settings_pack::rate_limit_ip_overhead, false);
    settings.set_bool(lt::settings_pack::strict_super_seeding, false);
    settings.set_bool(lt::settings_pack::no_connect_privileged_ports, false);
    settings.set_bool(lt::settings_pack::apply_ip_filter_to_trackers, false);
    settings.set_int(lt::settings_pack::stop_tracker_timeout, 1);
    settings.set_int(lt::settings_pack::auto_scrape_interval, 1200);
    settings.set_int(lt::settings_pack::auto_scrape_min_interval, 900);
    settings.set_int(lt::settings_pack::cache_size, -1);
    settings.set_int(lt::settings_pack::cache_expiry, 60);
    settings.set_int(lt::settings_pack::disk_io_read_mode, 0);
    settings.set_int(lt::settings_pack::active_seeds, 3);
    settings.set_int(lt::settings_pack::active_downloads, 3);
    settings.set_int(lt::settings_pack::active_limit, 5);
    settings.set_int(lt::settings_pack::active_tracker_limit, -1);
    settings.set_int(lt::settings_pack::active_dht_limit, -1);
    settings.set_int(lt::settings_pack::active_lsd_limit, -1);
    settings.set_int(lt::settings_pack::connections_limit, 500);
    settings.set_int(lt::settings_pack::unchoke_slots_limit, -1);
    settings.set_int(lt::settings_pack::mixed_mode_algorithm, 0);
    settings.set_int(lt::settings_pack::connection_speed, 20);
    settings.set_int(lt::settings_pack::seed_choking_algorithm, 1);

    p_session = QSharedPointer<lt::session>::create(settings);

    setupManager();
}

tsuManager::tsuManager(const lt::settings_pack & sp)
{
    p_session = QSharedPointer<lt::session>::create(sp);
    setupManager();
}

void tsuManager::setupManager()
{
//    timerUpdate = new QTimer(this);
//    connect(timerUpdate, SIGNAL(timeout()), this, SLOT(postUpdates()));
//    connect(this, SIGNAL(stopTimer()), timerUpdate, SLOT(stop()));
//    connect(this, SIGNAL(finished()), timerUpdate, SLOT(deleteLater()));

    p_session->set_alert_notify([this]()
    {
//        QTimer::singleShot(0, this, SLOT(alertsHandler()));
        QMetaObject::invokeMethod(this, "alertsHandler", Qt::QueuedConnection);
    });
}

void tsuManager::startManager()
{
//    timerUpdate->start(TIMER_TICK);

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
            lt::error_code ec;
            lt::bdecode_node bdn;
            lt::bdecode(buf.constData(), buf.constData() + buf.size(), bdn, ec);
            if (ec) {
                qDebug() << QString("fastresume error: cannot decode %0").arg(fileName);
                continue;
            }
            if (bdn.type() != lt::bdecode_node::type_t::dict_t) {
                qDebug() << QString("fastresume error: file %0 not a valid file").arg(fileName);
                continue;
            }
            lt::add_torrent_params tp = lt::read_resume_data(bdn, ec);
            if (ec) {
                qDebug() << QString("fastresume error: cannot load fastresume %0").arg(fileName);
                continue;
            }
            tp.name = bdn.dict_find_string_value("zu-fileName").to_string();
            p_session->async_add_torrent(tp);
        }
    }
}

void tsuManager::stopManager()
{
//    emit stopTimer();
    p_session->pause();

    std::vector<lt::torrent_handle> handles = p_session->get_torrents();
    qDebug() << QString("stopManager: Handling %0 handlers").arg(handles.size());

    for (lt::torrent_handle i : handles)
    {
        lt::torrent_handle &h = i;
        if (!h.is_valid()) continue;
        lt::torrent_status s = h.status();
        if (!s.has_metadata) continue;
        if (!s.need_save_resume) continue;
        h.save_resume_data();
        tsuManager::outstanding_resume_data++;
    }

    while (tsuManager::outstanding_resume_data > 0)
    {
//        qDebug() << QString("stopManager: outstanding_resume_data = %0").arg(outstanding_resume_data);
        lt::alert const* a = p_session->wait_for_alert(lt::seconds(10));

        // if we don't get an alert within 10 seconds, abort
        if (a == 0) continue;

        std::vector<lt::alert*> alerts;
        p_session->pop_alerts(&alerts);

        if (alerts.size() == 0) continue;

        for (lt::alert* a : alerts)
        {
            if (a == nullptr) continue;

//            qDebug() << QString("  %0:%1").arg(a->what()).arg(a->message().c_str());

            if (a == nullptr || a->type() == 0) continue;

            switch (a->type()) {
            case lt::save_resume_data_failed_alert::alert_type:
                tsuManager::outstanding_resume_data--;
                break;
            case lt::save_resume_data_alert::alert_type:
                if (a == nullptr) continue;
                lt::save_resume_data_alert const* rd = lt::alert_cast<lt::save_resume_data_alert>(a);
                lt::torrent_handle h = rd->handle;
                lt::torrent_status st = h.status(lt::torrent_handle::query_save_path | lt::torrent_handle::query_name);
                lt::entry &en = lt::write_resume_data(rd->params);
                en.dict().insert({ "zu-fileName", st.name });
                QString fastresumeFolder = QString("%0/%1").arg(QCoreApplication::applicationDirPath()).arg("fastresume");
                std::stringstream hex;
                hex << st.info_hash;
                QString fileName = QString("%0/%1.fastresume").arg(fastresumeFolder).arg(QString::fromStdString(hex.str()));
                std::ofstream out(fileName.toStdString(), std::ios_base::binary);
                out.unsetf(std::ios_base::skipws);
                lt::bencode(std::ostream_iterator<char>(out), en);
                tsuManager::outstanding_resume_data--;
                break;
            }
        }
    }
    qDebug("stopped, emitting finished");
    emit finished();
}

void tsuManager::alertsHandler()
{
//    timerUpdate->stop();
    std::vector<lt::alert*> alerts;
    p_session->pop_alerts(&alerts);
//    qDebug() << QString("processing %1 alerts:").arg(alerts.size());

    QVector<tsuEvents::tsuEvent> eventsArray;

    for (lt::alert* alert : alerts)
    {
        if (alert == nullptr) continue;
//        qDebug() << QString(" %1").arg(alert->message().c_str());

        switch (alert->type())
        {
        case lt::add_torrent_alert::alert_type:
        {
            lt::add_torrent_alert* ata = lt::alert_cast<lt::add_torrent_alert>(alert);
            if (ata->error) {
                qDebug() << QString("tsuManager: error adding %0: %1")
                            .arg(alert->message().c_str())
                            .arg(ata->error.message().c_str());
            }
            if (ata->handle.torrent_file()) {
                // FROM .TORRENT
            } else {
                // FROM MAGNET
            }
            lt::torrent_status const &ts = ata->handle.status();
            statusEnum se = static_cast<statusEnum>((int)ts.state);
            if (ts.paused) se = statusEnum::paused;
            tsuEvents::tsuEvent ev(ata->handle.info_hash().to_string(), ts.name.c_str(), ts.total_done, ts.total_upload, ts.download_rate, ts.upload_rate, ts.total_wanted, (int)se, ts.progress_ppm);
            emit addFromSessionManager(ev);
            break;
        }
        case lt::state_update_alert::alert_type:
        {
            lt::state_update_alert* sua = lt::alert_cast<lt::state_update_alert>(alert);
            if (sua->status.empty()) { break; }
            for (lt::torrent_status const& s : sua->status)
            {
                statusEnum se = static_cast<statusEnum>((int)s.state);
                if (s.paused) se = statusEnum::paused;
                tsuEvents::tsuEvent ev(s.info_hash.to_string(), s.name.c_str(), s.total_done, s.total_upload, s.download_rate, s.upload_rate, s.total_wanted, (int)se, s.progress_ppm);
                eventsArray.append(ev);
            }
            break;
        }
        case lt::torrent_deleted_alert::alert_type:
        {
            lt::torrent_deleted_alert* tra = lt::alert_cast<lt::torrent_deleted_alert>(alert);
            QString fastresumeFolder = QString("%0/%1").arg(QCoreApplication::applicationDirPath()).arg("fastresume");
            std::stringstream hex;
            hex << tra->info_hash;
            QString fileName = QString("%0/%1.fastresume").arg(fastresumeFolder).arg(QString::fromStdString(hex.str()));
            if (QFile::exists(fileName)) {
                QFile file(fileName);
                file.remove();
            }
            emit torrentDeleted(tra->info_hash.to_string());
            break;
        }
        default:
            break;
        }

        if (!eventsArray.isEmpty()) {
            emit updateFromSessionManager(eventsArray);
        }
    }
//    timerUpdate->start(TIMER_TICK);

    p_session->post_torrent_updates(lt::alert::status_notification | lt::alert::progress_notification);
}

//void tsuManager::postUpdates()
//{
//    qDebug("ticker ticked, posting updates");
//    p_session->post_torrent_updates(lt::alert::status_notification | lt::alert::progress_notification);
//}

tsuManager::~tsuManager()
{
    qDebug("starting destroy");
//    delete timerUpdate;
//    p_session.clear();
}

void tsuManager::addItems(const QStringList && items, const QString &path)
{
    foreach (const QString &str, items)
    {
        try
        {
            lt::add_torrent_params atp;
            lt::torrent_info ti(str.toStdString());

            if (!ti.metadata()) qDebug("NO METADATA!");
            if (!ti.is_valid()) qDebug("INVALID!");
            atp.ti = std::make_shared<lt::torrent_info>(ti);
            atp.save_path = path.toStdString();

            atp.flags &= ~lt::add_torrent_params::flag_paused; // Start in pause
            atp.flags &= ~lt::add_torrent_params::flag_auto_managed; // Because it is added in paused state
            atp.flags &= ~lt::add_torrent_params::flag_duplicate_is_error; // Already checked

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
        lt::sha1_hash sh(hash);
        lt::torrent_handle th = p_session->find_torrent(sh);
        p_session->remove_torrent(th, deleteFilesToo);
    } catch (std::exception &exc) {
        qDebug() << QString("tsuManager::getCancelRequest throws %0").arg(exc.what());
        emit torrentDeleted(hash);
    }
}

void tsuManager::getPauseRequest(const std::string &hash)
{
    try {
        lt::sha1_hash sh(hash);
        lt::torrent_handle th = p_session->find_torrent(sh);
        th.pause();
    } catch (std::exception &exc) {
        qDebug() << QString("tsuManager::getPauseRequest throws %0").arg(exc.what());
    }
}

void tsuManager::getResumeRequest(const std::string &hash)
{
    try {
        lt::sha1_hash sh(hash);
        lt::torrent_handle th = p_session->find_torrent(sh);
        th.resume();
    } catch (std::exception &exc) {
        qDebug() << QString("tsuManager::getResumeRequest throws %0").arg(exc.what());
    }
}

