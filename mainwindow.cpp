#include <QtGlobal>
#include <QDebug>
#include <QCoreApplication>
#include <QTableWidget>
#include <QScrollArea>

#include <iostream>
#include <thread>
#include <chrono>
#include <fstream>
#include <libtorrent/session.hpp>
#include <libtorrent/add_torrent_params.hpp>
#include <libtorrent/torrent_handle.hpp>
#include <libtorrent/alert_types.hpp>
#include <libtorrent/bencode.hpp>
#include <libtorrent/torrent_status.hpp>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "settingswindow.h"
#include "searchwindow.h"
#include "downloadwindow.h"

#define TIME_TRAY_BALLOON 5000
#define PROJECT "Tsunami++"

namespace lt = libtorrent;
using clk = std::chrono::steady_clock;

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

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    settingsPage(new settingswindow),
    searchPage(new searchwindow),
    downloadPage(new downloadwindow),
    qsa(new QScrollArea)
{
    ui->setupUi(this);
    settingsPage->hide();
    searchPage->hide();
    qsa->hide();

    initializeScreen();
    count = 0;

    statusBar()->showMessage("Welcome!");

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initializeScreen() {
    setWindowIcon(QIcon(":/icons/logo_tsunami_tiny.ico"));
    setWindowTitle(PROJECT " " VERSION);
    //setWindowFlags(Qt::FramelessWindowHint);
    createTrayIcon();

    connect(ui->btnSettings, SIGNAL(released()), this, SLOT(btnMenuClick()));
    connect(ui->btnUser, SIGNAL(released()), this, SLOT(btnMenuClick()));
    connect(ui->btnSearch, SIGNAL(released()), this, SLOT(btnMenuClick()));
    connect(ui->btnDownload, SIGNAL(released()), this, SLOT(btnMenuClick()));
    connect(ui->btnArchive, SIGNAL(released()), this, SLOT(btnMenuClick()));
    connect(ui->btnStreaming, SIGNAL(released()), this, SLOT(btnMenuClick()));
    connect(ui->btnStatistics, SIGNAL(released()), this, SLOT(btnMenuClick()));

    connect(ui->btnAdd, SIGNAL(released()), this, SLOT(btnAddClick()));

    ui->btnSettings->show();
    ui->btnUser->show();
    ui->btnSearch->show();
    ui->btnDownload->show();
    ui->btnArchive->show();
    ui->btnStreaming->show();
    ui->btnStatistics->show();

    ui->btnSettings_on->hide();
    ui->btnUser_on->hide();
    ui->btnSearch_on->hide();
    ui->btnDownload_on->hide();
    ui->btnArchive_on->hide();
    ui->btnStreaming_on->hide();
    ui->btnStatistics_on->hide();

    settingsPage->hide();
    searchPage->hide();
    downloadPage->hide();

    ui->content->addWidget(settingsPage);
    ui->content->addWidget(searchPage);

    qsa->setWidgetResizable(true);
    qsa->setWidget(downloadPage);
    ui->content->addWidget(qsa);

    /*QWidget *defaultPage = new QWidget(this);
    defaultPage->setWindowOpacity(0.5);
    defaultPage->setStyleSheet("background-image: url(:/images/logo_tsunami_big.ico); background-repeat: no-repeat; background-position:center;");
    ui->content->addWidget(defaultPage);*/

    /*
    lt::settings_pack pack;
    pack.set_int(lt::settings_pack::alert_mask
                 , lt::alert::error_notification
                 | lt::alert::storage_notification
                 | lt::alert::status_notification);

    lt::session ses(pack);
    lt::add_torrent_params atp;
    atp.url = "magnet:?xt=urn:btih:8fbe78abe0dd8bd84f675a3bb9d399f19be42dee&dn=Why Him (2016) DVDRip XviD AC3 EVO&tr=http://explodie.org:6969/announce&tr=http://tracker.opentrackr.org:1337/announce&tr=udp://tracker.coppersurfer.tk:6969/announce&tr=udp://tracker.leechers-paradise.org:6969/announce&tr=udp://tracker.sktorrent.net:6969/announce&tr=udp://tracker.zer0day.to:1337/announce";
    atp.save_path = ".";

    lt::torrent_handle th = ses.add_torrent(atp);

    for (;;) {
        std::vector<lt::alert*> alerts;
        ses.pop_alerts(&alerts);

        for (lt::alert const* a : alerts) {
          //qDebug(a->message().c_str());
          if (auto st = lt::alert_cast<lt::state_update_alert>(a)) {
            if (st->status.empty()) continue;
            // we only have a single torrent, so we know which one
            // the status is for
            lt::torrent_status const& s = st->status[0];
            qDebug() << "\r" << QString(state(s.state)) << " " <<
                   (s.download_payload_rate / 1000) << " kB/s " <<
                   (s.total_done / 1000) << " kB (" <<
                   (s.progress_ppm / 10000) << "%) downloaded\x1b[K";

            //std::cout.flush();
          }
          // if we receive the finished alert or an error, we're done
          if (lt::alert_cast<lt::torrent_finished_alert>(a)) {
            goto done;
          }
          if (lt::alert_cast<lt::torrent_error_alert>(a)) {
            goto done;
          }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        ses.post_torrent_updates();
      }
      done:
      qDebug("done");
      */

}


void MainWindow::createTrayIcon()
{
    // Tray icon
    m_systrayIcon = new QSystemTrayIcon(QIcon(":/icons/logo_tsunami_tiny.ico"), this);

    //m_systrayIcon->setContextMenu(trayIconMenu());
    connect(m_systrayIcon, SIGNAL(messageClicked()), this, SLOT(balloonClicked()));
    // End of Icon Menu
    connect(m_systrayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(toggleVisibility(QSystemTrayIcon::ActivationReason)));
    m_systrayIcon->setToolTip(PROJECT " " VERSION);
    m_systrayIcon->show();
    m_systrayIcon->showMessage(PROJECT, "Welcome to the future!", QSystemTrayIcon::Information, TIME_TRAY_BALLOON);
    qDebug("started");
}

// Toggle Main window visibility
void MainWindow::toggleVisibility(QSystemTrayIcon::ActivationReason e)
{
    qDebug("entered toggleVisibility");
    if ((e == QSystemTrayIcon::Trigger) || (e == QSystemTrayIcon::DoubleClick)) {
        if (isHidden()) {
            /*if (m_uiLocked) {
                // Ask for UI lock password
                if (!unlockUI())
                    return;
            }*/
            // Make sure the window is not minimized
            setWindowState(windowState() & (~Qt::WindowMinimized | Qt::WindowActive));
            // Then show it
            show();
            raise();
            activateWindow();
        }
        else {
            hide();
        }
    }
}

void MainWindow::balloonClicked()
{
    if (isHidden()) {
        /*if (m_uiLocked) {
            // Ask for UI lock password
            if (!unlockUI())
                return;
        }*/
        show();
        if (isMinimized())
            showNormal();
    }

    raise();
    activateWindow();
}

void MainWindow::btnMenuClick() {
    QString btn = sender()->objectName();

    ui->btnSettings->setHidden(btn == ui->btnSettings->objectName());
    ui->btnUser->setHidden(btn == ui->btnUser->objectName());
    ui->btnSearch->setHidden(btn == ui->btnSearch->objectName());
    ui->btnDownload->setHidden(btn == ui->btnDownload->objectName());
    ui->btnArchive->setHidden(btn == ui->btnArchive->objectName());
    ui->btnStreaming->setHidden(btn == ui->btnStreaming->objectName());
    ui->btnStatistics->setHidden(btn == ui->btnStatistics->objectName());

    ui->btnSettings_on->setHidden(btn != ui->btnSettings->objectName());
    ui->btnUser_on->setHidden(btn != ui->btnUser->objectName());
    ui->btnSearch_on->setHidden(btn != ui->btnSearch->objectName());
    ui->btnDownload_on->setHidden(btn != ui->btnDownload->objectName());
    ui->btnArchive_on->setHidden(btn != ui->btnArchive->objectName());
    ui->btnStreaming_on->setHidden(btn != ui->btnStreaming->objectName());
    ui->btnStatistics_on->setHidden(btn != ui->btnStatistics->objectName());

    settingsPage->setHidden(btn != ui->btnSettings->objectName());
    searchPage->setHidden(btn != ui->btnSearch->objectName());
    //downloadPage->setHidden(btn != ui->btnDownload->objectName());
    qsa->setHidden(btn != ui->btnDownload->objectName());
}

void MainWindow::btnAddClick()
{
    count++;
    downloadPage->addTorrent(QString("Name %1").arg(count));
    //downloadPage->update();
}
