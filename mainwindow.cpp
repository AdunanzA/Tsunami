#include <QtGlobal>
#include <QDebug>
#include <QCoreApplication>
#include <QTableWidget>
#include <QScrollArea>
#include <QFileDialog>

#include <libtorrent/add_torrent_params.hpp>
#include <libtorrent/torrent_handle.hpp>
#include <libtorrent/bencode.hpp>
#include <libtorrent/torrent_info.hpp>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "settingswindow.h"
#include "searchwindow.h"
#include "downloadwindow.h"

#define TIME_TRAY_BALLOON 5000
#define PROJECT "Tsunami++"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    settingsPage(new settingswindow),
    searchPage(new searchwindow),
    statusLabel(new QLabel),
    downloadPage(new downloadwindow),
    statisticsPage(new statisticswindow),
    qsa(new QScrollArea)
{
    ui->setupUi(this);

    initializeScreen();

    statusBar()->addPermanentWidget(statusLabel, 0);
    statusBar()->showMessage("Welcome!");

    // update from downloadpage to statusbar text
    connect(downloadPage, SIGNAL(sendUpdateToStatusBar(const QString &)), this, SLOT(updateStatusBar(const QString &)));

    // update from downloadpage to statistics page
    connect(downloadPage, SIGNAL(sendStatisticsUpdate(const QPair<int,int> &)), statisticsPage, SLOT(updateStats(const QPair<int,int> &)));

    // popup message from downloadPage
    connect(downloadPage, SIGNAL(sendPopupInfo(QString)), this, SLOT(popupInfo(QString)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::updateStatusBar(const QString &msg)
{
    statusLabel->setText(msg);
}

void MainWindow::updateGauge(const double &value)
{
    p_scaleGauge->setValue(value);
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
    statisticsPage->hide();
    qsa->hide();

    ui->content->addWidget(settingsPage);
    ui->content->addWidget(searchPage);
    ui->content->addWidget(statisticsPage);

    qsa->setFrameShape(QFrame::NoFrame);
    qsa->setWidgetResizable(true);
    qsa->setWidget(downloadPage);
    ui->content->addWidget(qsa);

//    tp = new TsuProgress(this);
//    timer = new QTimer(this);
//    ui->content->addWidget(tp);
//    connect(timer, SIGNAL(timeout()), this, SLOT(timerTick()));
//    timer->start(250);

    /*QWidget *defaultPage = new QWidget(this);
    defaultPage->setWindowOpacity(0.5);
    defaultPage->setStyleSheet("background-image: url(:/images/logo_tsunami_big.ico); background-repeat: no-repeat; background-position:center;");
    ui->content->addWidget(defaultPage);*/

    p_scaleGauge = new QScale(this);
    p_scaleGauge->setInvertedAppearance(true);
    ui->contentGauge->addWidget(p_scaleGauge);

    connect(downloadPage, SIGNAL(sendUpdateGauge(double)), this, SLOT(updateGauge(double)));
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
//    m_systrayIcon->showMessage(PROJECT, "Welcome to the future!", QSystemTrayIcon::Information, TIME_TRAY_BALLOON);
    popupInfo("Welcome to the future!");
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
    statisticsPage->setHidden(btn != ui->btnStatistics->objectName());
    qsa->setHidden(btn != ui->btnDownload->objectName());
}

void MainWindow::btnAddClick()
{
    QStringList fileNames = QFileDialog::getOpenFileNames(this, tr("Select torrent"), ".", tr("Torrents (*.torrent)"));
    foreach (const QString &str, fileNames) {
        lt::add_torrent_params atp;

        NativePtr np(new lt::torrent_info(str.toStdString()));
        atp.ti = np;
        atp.save_path = ".";

        atp.flags &= ~lt::add_torrent_params::flag_paused; // Start in pause
        atp.flags &= ~lt::add_torrent_params::flag_auto_managed; // Because it is added in paused state
        atp.flags &= ~lt::add_torrent_params::flag_duplicate_is_error; // Already checked

        downloadPage->addTorrent(atp);
        ui->btnDownload->released();
    }
}

void MainWindow::popupInfo(const QString &msg)
{
    m_systrayIcon->showMessage(PROJECT, msg, QSystemTrayIcon::Information, TIME_TRAY_BALLOON);
}

//void MainWindow::timerTick()
//{
//    if(count==100) {
//        timer->stop();
//        return;
//    }
//    count = count + 1; // %
//    tp->setValue(count);
//    //QCoreApplication::processEvents();
//    //qDebug() << QString("tick %1").arg(count);
//}
