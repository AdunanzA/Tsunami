#include "downloadwindow.h"
#include "ui_downloadwindow.h"
#include "tsucard.h"

#define TICKER_TIME 1000

// return the name of a torrent status enum
char const* state(lt::torrent_status::state_t s)
{
  switch(s) {
    case lt::torrent_status::checking_files: return "checking";
    case lt::torrent_status::downloading_metadata: return "dl metadata";
    case lt::torrent_status::downloading: return "downloading";
    case lt::torrent_status::finished: return "finished";
    case lt::torrent_status::seeding: return "seeding";
    case lt::torrent_status::allocating: return "allocating";
    case lt::torrent_status::checking_resume_data: return "checking resume";
    default: return "<>";
  }
}

downloadwindow::downloadwindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::downloadwindow),
    flowLayout(new FlowLayout)
{
    ui->setupUi(this);
    setLayout(flowLayout);

    lt::settings_pack settings;
    settings.set_str(lt::settings_pack::listen_interfaces, "0.0.0.0:6881");

    p_session = lt::session(settings);
    p_session.pause();
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(timerTick()));

//    p_session.set_alert_mask(lt::alert::error_notification
//                             | lt::alert::storage_notification
//                             | lt::alert::status_notification);
//    p_session.set_alert_mask(lt::alert::status_notification);

//    lt::session_settings ss = p_session.settings();
//    ss.user_agent = "Tsunami/0";
//    // from qbit torrent
//    ss.upnp_ignore_nonrouters = true;
//    ss.use_dht_as_fallback = false;
//    ss.ssl_listen = 0; // Disable support for SSL torrents for now
//    ss.lazy_bitfields = true; // To prevent ISPs from blocking seeding
//    ss.stop_tracker_timeout = 1; // Speed up exit
//    ss.auto_scrape_interval = 1200; // 20 minutes
//    ss.announce_to_all_trackers = true; // from pref->announceToAllTrackers();
//    ss.announce_to_all_tiers = true; // from pref->announceToAllTrackers();
//    ss.auto_scrape_min_interval = 900; // 15 minutes
//    ss.cache_size = -1;
//    ss.cache_expiry = 60;
//    ss.disk_io_read_mode = 0; // enable_os_cache
//    ss.anonymous_mode = false;
//    ss.active_seeds = 3; // MaxActiveUploads
//    ss.dont_count_slow_torrents = false;
//    /*ss.active_downloads = 3; // MaxActiveDownloads + m_extraLimit
//    ss.active_limit = 5; // MaxActiveTorrents + m_extraLimit*/
//    ss.active_tracker_limit = -1;
//    ss.active_dht_limit = -1;
//    ss.active_lsd_limit = -1;
//    ss.ignore_limits_on_local_network = true; // Ignore limits on LAN
//    ss.rate_limit_ip_overhead = false; // Include overhead in transfer limits
//    //ss.announce_ip = Utils::String::toStdString(pref->getNetworkAddress()); // IP address to announce to trackers
//    ss.strict_super_seeding = false; // Super seeding
//    ss.half_open_limit = 20; // * Max Half-open connections
//    ss.connections_limit = 500; // * Max connections limit
//    ss.unchoke_slots_limit = -1; // * Global max upload slots
//    ss.enable_incoming_utp = true; // uTP
//    ss.enable_outgoing_utp = true; // uTP
//    ss.rate_limit_utp = true; // uTP rate limiting
//    ss.mixed_mode_algorithm = 0; // prefer_tcp
//    ss.connection_speed = 20; //default is 10
//    ss.no_connect_privileged_ports = false;
//    ss.seed_choking_algorithm = 1; // fastest_upload
//    ss.apply_ip_filter_to_trackers = false; // FilterTracker


//    ss.tick_interval = TICKER_TIME;

//    p_session.set_settings(ss);

    timer->start(TICKER_TIME);

//    p_session.add_dht_router(std::make_pair(std::string("router.bittorrent.com"), 6881));
//    p_session.add_dht_router(std::make_pair(std::string("router.utorrent.com"), 6881));
//    p_session.add_dht_router(std::make_pair(std::string("dht.transmissionbt.com"), 6881));
//    p_session.add_dht_router(std::make_pair(std::string("dht.aelitis.com"), 6881));

//    p_session.start_natpmp();
//    p_session.start_upnp();
//    p_session.start_dht();
//    p_session.start_lsd();
    p_session.resume();
}

downloadwindow::~downloadwindow()
{
    timer->stop();
    delete timer;
    delete ui;
}

void downloadwindow::addTorrent(lt::add_torrent_params atp) {
    p_session.async_add_torrent(atp);
}

void downloadwindow::requestedCancel(tsucard *sender)
{
    // change sender to QObject::sender() (maybe wrong in multi thread, need test)
    // http://doc.qt.io/qt-5/qobject.html#sender
    timer->stop();
    qDebug() << QString("requestedCancel for %1").arg(sender->get_Name());
    try {
        lt::torrent_handle th = p_session.find_torrent(sender->get_Hash());
        p_session.remove_torrent(th, 1);
        tsulist.removeAll(sender);
        flowLayout->removeWidget(sender);
        sender->close();
    } catch (std::exception &exc) {
        qDebug() << QString("requestedCancel throws %1").arg(exc.what());
    }
    timer->start(TICKER_TIME);

}

void downloadwindow::timerTick()
{
    std::vector<lt::alert*> alerts;
    p_session.pop_alerts(&alerts);

    //int totalDownload = 0;
    //int totalUpload = 0;
    downRate = 0;
    upRate = 0;

//    qDebug() << QString("processing %1 alerts:").arg(alerts.size());
    for (lt::alert* a : alerts) {
        if (a == nullptr) continue;
//        qDebug() << QString(" %1").arg(a->message().c_str());

        if (auto st = lt::alert_cast<lt::state_update_alert>(a)) {
            if (st == nullptr) continue;
            if (st->status.empty()) continue;
            for (lt::torrent_status s : st->status) {
                lt::sha1_hash ch = s.info_hash;
                try {
                    foreach (tsucard* item, tsulist) {
                        if ( item->get_Hash() == ch ) {
                            item->set_State(QString(state(s.state)));
                            item->set_Downloaded(s.total_done);
                            item->set_Uploaded(s.total_upload);
                            item->set_Percentage(s.progress_ppm / 10000);
                            item->set_DownloadRate(s.download_rate);
                            item->set_UploadRate(s.upload_rate);
                            continue;
                        }
                    }
                } catch (std::exception &exc) {
                    qDebug() << QString("timerTick throws %1").arg(exc.what());
                }
            }
        /*qDebug() << "\r" << QString(state(s.state)) << " " <<
               (s.download_payload_rate / 1000) << " kB/s " <<
               (s.total_done / 1000) << " kB (" <<
               (s.progress_ppm / 10000) << "%) downloaded\x1b[K";
        */
      }

//      if (lt::alert_cast<lt::torrent_finished_alert>(a)) {

//      }

//      if (lt::alert_cast<lt::torrent_error_alert>(a)) {

//      }

        if (auto aa = lt::alert_cast<lt::add_torrent_alert>(a)) {
            lt::torrent_handle th = aa->handle;
            lt::torrent_status st = th.status(lt::torrent_handle::query_name);
            qDebug() << QString("torrent_added_alert(%1) called from %2").arg(QString::fromStdString(st.name)).arg(this->objectName());
            tsucard *ts = new tsucard(this);
            ts->set_Hash(st.info_hash);// th.info_hash());
            ts->set_Name(QString::fromStdString(st.name));
            ts->set_Total(st.total_wanted / 1000);
            connect(ts, SIGNAL(cancelRequested(tsucard*)), this, SLOT(requestedCancel(tsucard*)));
            tsulist.append(ts);
            flowLayout->addWidget(ts);
        }

        if (auto aa = lt::alert_cast<lt::torrent_finished_alert>(a)) {
            lt::torrent_handle th = aa->handle;
            lt::torrent_status st = th.status(lt::torrent_handle::query_name);
            emit sendPopupInfo(QString("%1 finished!").arg(QString::fromStdString(st.name)));
        }

        if (auto aa = lt::alert_cast<lt::tracker_error_alert>(a)) {
            qDebug() << QString("tracker_error_alert %1").arg(QString::fromStdString(aa->message()));
        }

    }

    /*
    // update session statistics
    const lt::session_status ss = p_session.status();

    // update mainwindow statusbar message
    tsucard ts;
    QString ups = QString("%1%2/s (%3%4) / %5%6/s (%7%8)").arg(ts.InfoSize(ss.download_rate)).arg(ts.InfoUnit(ss.download_rate))
                                                          .arg(ts.InfoSize(ss.total_download)).arg(ts.InfoUnit(ss.total_download))
                                                          .arg(ts.InfoSize(ss.upload_rate)).arg(ts.InfoUnit(ss.upload_rate))
                                                          .arg(ts.InfoSize(ss.total_upload)).arg(ts.InfoUnit(ss.total_upload));
    emit sendUpdateToStatusBar(ups);

    // update mainwindow gauge
    double val = (ss.download_rate * 8)/1000000; // Ethernet 100 BASE-T -> http://www.convert-me.com/en/convert/data_transfer_rate/byte_s.html?u=byte%2Fs&v=1
    emit sendUpdateGauge(val);

    // update statistics page
    emit sendStatisticsUpdate(QPair<int, int>(ss.download_rate, ss.upload_rate));
    */
    foreach (tsucard* item, tsulist) {
        totalDownload += item->get_Downloaded();
        totalUpload += item->get_Uploaded();
        downRate += item->get_DownloadRate();
        upRate += item->get_UploadRate();
    }

    if (downRate > 0 || upRate > 0 || totalDownload > 0 || totalUpload > 0) {
        tsucard ts;
        QString ups = QString("%1%2/s (%3%4) / %5%6/s (%7%8)").arg(ts.InfoSize(downRate)).arg(ts.InfoUnit(downRate))
                                                              .arg(ts.InfoSize(totalDownload)).arg(ts.InfoUnit(totalDownload))
                                                              .arg(ts.InfoSize(upRate)).arg(ts.InfoUnit(upRate))
                                                              .arg(ts.InfoSize(totalUpload)).arg(ts.InfoUnit(totalUpload));
        emit sendUpdateToStatusBar(ups);
    }

    // update mainwindow gauge
//    if (downRate > 0) {
    double val = (downRate * 8)/1000000; // Ethernet 100 BASE-T -> http://www.convert-me.com/en/convert/data_transfer_rate/byte_s.html?u=byte%2Fs&v=1
    emit sendUpdateGauge(val);
//    }

    // update statistics page
//    if (downRate > 0 || upRate > 0) {
    emit sendStatisticsUpdate(QPair<int, int>(downRate, upRate));
//    }

    p_session.post_torrent_updates(lt::alert::status_notification);
//    p_session.post_session_stats();
//    p_session.post_dht_stats();
}










