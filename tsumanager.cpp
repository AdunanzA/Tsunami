#include "tsumanager.h"

int tsuManager::outstanding_resume_data = 0;

tsuManager::tsuManager()
{
    // setting default tsunami folder
    QString localTsunami = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation); // win -> C:\Users\user\AppData\Local\Tsunami
    localTsunami = QString("%0/%1").arg(localTsunami).arg("session");
    p_tsunamiSessionFolder = QDir::toNativeSeparators(localTsunami);

    // loading libtorrent stats metric indexes
    std::vector<lt::stats_metric> ssm = lt::session_stats_metrics();
    for(lt::stats_metric & metric : ssm) {
        qApp->setProperty(metric.name, metric.value_index);
    }
    qDebug() << QString("loaded %0 metric index from libtorrent").arg(QString::number(ssm.size()));

    // total number of bytes sent and received by the session (type 0 counter)
    // net.sent_payload_bytes     (included in net.sent_bytes)
    // net.sent_bytes
    // net.sent_ip_overhead_bytes (not included in net.sent_bytes)
    // net.sent_tracker_bytes     (not included in net.sent_bytes)
    
    // net.recv_payload_bytes
    // net.recv_bytes
    // net.recv_ip_overhead_bytes
    // net.recv_tracker_bytes
    
    // "net.recv_bytes" + "net.recv_ip_overhead_bytes" = total download

    p_net_recv_bytes = qApp->property("net.recv_bytes").toInt();
    p_net_recv_ip_overhead_bytes = qApp->property("net.recv_ip_overhead_bytes").toInt();
    p_net_sent_bytes = qApp->property("net.sent_bytes").toInt();
    p_net_sent_ip_overhead_bytes = qApp->property("net.sent_ip_overhead_bytes").toInt();

    p_timerUpdate = new QTimer(this);
    connect(p_timerUpdate, SIGNAL(timeout()), this, SLOT(postUpdates()));
    connect(this, SIGNAL(stopTimer()), p_timerUpdate, SLOT(stop()));
    connect(this, SIGNAL(finished()), p_timerUpdate, SLOT(deleteLater()));
}

void tsuManager::setNotify()
{
    p_session->set_alert_notify([this]()
    {
//        QTimer::singleShot(0, this, SLOT(alertsHandler()));
        QMetaObject::invokeMethod(this, "alertsHandler", Qt::AutoConnection);
    });
}

void tsuManager::loadSettings(lt::settings_pack &settings)
{
//    lt::settings_pack settings = p_session->get_settings();
    QSettings qtSettings(qApp->property("iniFilePath").toString(), QSettings::IniFormat);
    int downLimit = qtSettings.value("libtorrent/download_rate_limit", 0).toInt();
    int upLimit = qtSettings.value("libtorrent/upload_rate_limit", 0).toInt();

    // from KB/s to B/S
    downLimit = downLimit * 1000;
    upLimit = upLimit * 1000;

    QString user_agent = QString("Tsunami/%0.%1.%2").arg(VERSION_MAJOR).arg(VERSION_MINOR).arg(VERSION_BUGFIX);

    settings.set_str(lt::settings_pack::user_agent, user_agent.toStdString());
    settings.set_int(lt::settings_pack::download_rate_limit, downLimit); // B/S , 0 unlimited
    settings.set_int(lt::settings_pack::upload_rate_limit, upLimit); // B/S , 0 unlimited

    // DEFAULTS from picotorrent (and qbittorrent)
    settings.set_str(lt::settings_pack::string_types::dht_bootstrap_nodes,
        "router.bittorrent.com:6881" ","
        "router.utorrent.com:6881" ","
        "dht.transmissionbt.com:6881" ","
        "dht.aelitis.com:6881");
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

    int availThreads = ceil(QThread::idealThreadCount()/4);
    qDebug() << QString("found %0 ideal thread count, assigning %1 to hash threads").arg(QThread::idealThreadCount()).arg(availThreads);
    settings.set_int(lt::settings_pack::aio_threads, availThreads);
//    p_session->apply_settings(settings);
}

void tsuManager::startManager()
{
    qDebug("starting");

    lt::settings_pack settings;
    loadSettings(settings);
    p_session = QSharedPointer<lt::session>::create(settings);
    setNotify();
    p_timerUpdate->start(1000);

    if (!QDir(p_tsunamiSessionFolder).exists()) {
        if (QDir().mkpath(p_tsunamiSessionFolder)) {
            qDebug() << "created " << p_tsunamiSessionFolder;
        } else {
            qWarning() << "cannot create " << p_tsunamiSessionFolder;
        }
    } else {
        // resuming
        qDebug() << "using " << p_tsunamiSessionFolder;

        // SESSIONSTATE
        QString sessionFileName = QString("%0/tsunami.session").arg(p_tsunamiSessionFolder);
        sessionFileName = QDir::toNativeSeparators(sessionFileName);

        QFileInfo sessionFileInfo(sessionFileName);
        qDebug() << QString("session file exists %0, is readable %1").arg((sessionFileInfo.exists()) ? "True" : "False")
                                                                     .arg((sessionFileInfo.isReadable()) ? "True" : "False");
        if (sessionFileInfo.exists() && sessionFileInfo.isReadable()) {
            QFile sessionFile(sessionFileName);
            sessionFile.open(QIODevice::ReadOnly);
            QByteArray sessionBuffer = sessionFile.readAll();
            lt::error_code sec;
            lt::bdecode_node session_state;
            lt::bdecode(sessionBuffer.constData(), sessionBuffer.constData() + sessionBuffer.size(), session_state, sec);
            if (sec) {
                qCritical() << QString("session state load error: cannot decode %0, error %1").arg(sessionFileName)
                                                                                              .arg(QString::fromStdString(sec.message()));
            } else {
                p_session->load_state(session_state);
                qDebug("session state loaded");
            }
        } else {
            qWarning("cannot access session state file");
        }

        // FASTRESUMES
        int count = 0;
        QDirIterator it(p_tsunamiSessionFolder, QStringList() << "*.fastresume", QDir::Files, QDirIterator::Subdirectories);
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
                qCritical() << QString("fastresume error: cannot decode %0").arg(fileName);
                continue;
            }
            if (bdn.type() != lt::bdecode_node::type_t::dict_t) {
                qCritical() << QString("fastresume error: file %0 not a valid file").arg(fileName);
                continue;
            }
            lt::add_torrent_params tp = lt::read_resume_data(bdn, ec);
            if (ec) {
                qCritical() << QString("fastresume error: cannot load fastresume %0").arg(fileName);
                continue;
            }
            QString torrentName = fileName.replace("fastresume", "torrent");
            lt::torrent_info ti(torrentName.toStdString());
            tp.ti = std::make_shared<lt::torrent_info>(ti);
            tp.name = bdn.dict_find_string_value("zu-fileName").to_string();
            p_session->async_add_torrent(tp);
            count++;
        }
        if (count == 0) {
            qDebug("no fastresumes to load");
        } else {
            qDebug() << QString("loaded %0 fastresumes").arg(count);
        }
    }
}

void tsuManager::stopManager()
{
    emit stopTimer();
    p_session->pause();

    // SAVE SESSION STATE
    lt::entry entry;
    p_session->save_state(entry);

    QString sessionFileName = QString("%0/tsunami.session").arg(p_tsunamiSessionFolder);
    sessionFileName = QDir::toNativeSeparators(sessionFileName);

    std::ofstream sessionOut(sessionFileName.toStdString(), std::ios_base::binary);
    sessionOut.unsetf(std::ios_base::skipws);

    lt::bencode(std::ostream_iterator<char>(sessionOut), entry);
    qDebug("session state saved");

    // FASTRESUMES
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
                std::stringstream hex;
                hex << st.info_hash;
                QString fileName = QString("%0/%1.fastresume").arg(p_tsunamiSessionFolder).arg(QString::fromStdString(hex.str()));
                fileName = QDir::toNativeSeparators(fileName);
                std::ofstream out(fileName.toStdString(), std::ios_base::binary);
                out.unsetf(std::ios_base::skipws);
                lt::bencode(std::ostream_iterator<char>(out), en);
                tsuManager::outstanding_resume_data--;
                break;
            }
        }
    }
    qDebug("fastresumes saved");

    qDebug("stopped, emitting finished");
    emit finished();
}

void tsuManager::alertsHandler()
{
    std::vector<lt::alert*> alerts;
    p_session->pop_alerts(&alerts);
//    qDebug() << QString("processing %1 alerts:").arg(alerts.size());

    QVector<tsuEvents::tsuEvent> eventsArray;

    for (lt::alert* alert : alerts)
    {
        if (alert == nullptr) continue;
//        qDebug() << QString("%0::%1").arg(alert->what()).arg(alert->message().c_str());

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
            std::stringstream hex;
            hex << tra->info_hash;
            QString hash = QString::fromStdString(hex.str());
            QString fileName = QString("%0/%1.fastresume").arg(p_tsunamiSessionFolder).arg(hash);
            fileName = QDir::toNativeSeparators(fileName);
            if (QFile::exists(fileName)) {
                QFile file(fileName);
                file.remove();
            }
            fileName = QString("%0/%1.torrent").arg(p_tsunamiSessionFolder).arg(hash);
            fileName = QDir::toNativeSeparators(fileName);
            if (QFile::exists(fileName)) {
                QFile file(fileName);
                file.remove();
            }
//            emit torrentDeleted(tra->info_hash.to_string());
            break;
        }
        case lt::external_ip_alert::alert_type:
        {
            lt::external_ip_alert* eia = lt::alert_cast<lt::external_ip_alert>(alert);
            QString extIp = QString::fromStdString(eia->external_address.to_string());
            qDebug() << QString("received external ip %0").arg(extIp);
            emit externalIpAssigned();
            break;
        }
        case lt::dht_bootstrap_alert::alert_type:
        {
//            lt::dht_bootstrap_alert* dba = lt::alert_cast<lt::dht_bootstrap_alert>(alert);
            qDebug("dht bootstrap done");
            emit dhtBootstrapExecuted();
            break;
        }
        case lt::listen_succeeded_alert::alert_type:
        {
            lt::listen_succeeded_alert* lsa = lt::alert_cast<lt::listen_succeeded_alert>(alert);
            QString type = "";
            if (lsa->socket_type == lt::socket_type_t::tcp) {
                type = "tcp";
            } else if (lsa->socket_type == lt::socket_type_t::udp) {
                type = "udp";
            }
            qDebug() << QString("listen succeeded for %0 on port %1").arg(type).arg(lsa->port);
            emit listenerUpdate(type, true);
            break;
        }
        case lt::listen_failed_alert::alert_type:
        {
            lt::listen_failed_alert* lfa = lt::alert_cast<lt::listen_failed_alert>(alert);
            QString type = "";
            if (lfa->socket_type == lt::socket_type_t::tcp) {
                type = "tcp";
            } else if (lfa->socket_type == lt::socket_type_t::udp) {
                type = "udp";
            }
            qDebug() << QString("listen failed for %0 on port %1").arg(type).arg(lfa->port);
            emit listenerUpdate(type, false);
            break;
        }
        case lt::session_stats_alert::alert_type:
        {
            lt::session_stats_alert *ssa = lt::alert_cast<lt::session_stats_alert>(alert);

            // "net.recv_bytes" + "net.recv_ip_overhead_bytes" = total download
            quint64 recvbytes = ssa->values[p_net_recv_bytes];
//            recvbytes += ssa->values[p_net_recv_ip_overhead_bytes];

            quint64 sentbytes = ssa->values[p_net_sent_bytes];
//            sentbytes += ssa->values[p_net_sent_ip_overhead_bytes];

            emit sessionStatisticUpdate(sentbytes, recvbytes);
            break;
        }
        default:
            break;
        }

        if (!eventsArray.isEmpty()) {
            emit updateFromSessionManager(eventsArray);
        }
    }
    p_session->post_torrent_updates(lt::alert::status_notification | lt::alert::progress_notification);
}

void tsuManager::postUpdates()
{
//    p_session->post_torrent_updates(lt::alert::status_notification | lt::alert::progress_notification);
      p_session->post_session_stats();
}

tsuManager::~tsuManager()
{
    qDebug("tsuManager destroyed");
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

            std::stringstream hex;
            hex << ti.info_hash();
            QString newFilePath = QDir::toNativeSeparators(QString("%0/%1.torrent").arg(p_tsunamiSessionFolder)
                                                           .arg(QString::fromStdString(hex.str())));
            QFile::copy(str, newFilePath);

            qInfo() << QString("torrent %0 added").arg(str);
        }
        catch (std::exception &exc)
        {
            qCritical() << QString("addItems throws %0").arg(exc.what());
        }
    }
}

void tsuManager::getCancelRequest(const std::string &hash, const bool deleteFilesToo)
{
    try {
        lt::sha1_hash sh(hash);
        lt::torrent_handle th = p_session->find_torrent(sh);
        p_session->remove_torrent(th, deleteFilesToo);
        emit torrentDeleted(hash);
    } catch (std::exception &exc) {
        qCritical() << QString("getCancelRequest throws %0").arg(exc.what());
//        emit torrentDeleted(hash);
    }
}

void tsuManager::getPauseRequest(const std::string &hash)
{
    try {
        lt::sha1_hash sh(hash);
        lt::torrent_handle th = p_session->find_torrent(sh);
        th.pause();
    } catch (std::exception &exc) {
        qCritical() << QString("getPauseRequest throws %0").arg(exc.what());
    }
}

void tsuManager::getResumeRequest(const std::string &hash)
{
    try {
        lt::sha1_hash sh(hash);
        lt::torrent_handle th = p_session->find_torrent(sh);
        th.resume();
    } catch (std::exception &exc) {
        qCritical() << QString("getResumeRequest throws %0").arg(exc.what());
    }
}

void tsuManager::refreshSettings()
{
    qDebug("received refreshSettings");
    lt::settings_pack settings = p_session->get_settings();
    loadSettings(settings);
    p_session->apply_settings(settings);
}

