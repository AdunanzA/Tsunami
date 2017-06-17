#include "mainwindow.h"
#include "ui_mainwindow.h"

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
    sessionManager(new tsuManager)
{
    ui->setupUi(this);

    initializeScreen();
    readSettings();

    statusBar()->addPermanentWidget(statusLabel, 0);
    statusBar()->showMessage("Welcome!");

    // update from sessionManager (torrent add and update) goes to downloadPage to manage tsuCard and session values
    connect(sessionManager, SIGNAL(updateFromItem(const QVector<tsuEvents::tsuEvent> &)), downloadPage, SLOT(updateFromSession(const QVector<tsuEvents::tsuEvent> &)), Qt::QueuedConnection);

    // deleted torrent from sessionManager goes to downloadPage to remove it from list
    connect(sessionManager, SIGNAL(torrentDeleted(std::string)), downloadPage, SLOT(torrentDeletedFromSession(std::string)), Qt::QueuedConnection);


    // requested cancel from downloadPage (emitted from a tsucard) goes to sessionManager to remove torrent from libtorrent
    connect(downloadPage, SIGNAL(sendRequestedCancelToSession(std::string,bool)), sessionManager, SLOT(getCancelRequest(std::string,bool)), Qt::QueuedConnection);

    // requested pause from downloadPage (emitted from a tsucard) goes to sessionManager to pause torrent
    connect(downloadPage, SIGNAL(sendRequestedPauseToSession(std::string)), sessionManager, SLOT(getPauseRequest(std::string)), Qt::QueuedConnection);

    // requested resume from downloadPage (emitted from a tsucard) goes to sessionManager to resume torrent
    connect(downloadPage, SIGNAL(sendRequestedResumeToSession(std::string)), sessionManager, SLOT(getResumeRequest(std::string)), Qt::QueuedConnection);


    // update from downloadPage to statusbar text
    connect(downloadPage, SIGNAL(sendUpdateToStatusBar(const QString &)), this, SLOT(updateStatusBar(const QString &)), Qt::QueuedConnection);

    // update from downloadPage to statistics page
    connect(downloadPage, SIGNAL(sendStatisticsUpdate(const QPair<int,int> &)), statisticsPage, SLOT(updateStats(const QPair<int,int> &)), Qt::QueuedConnection);

    // popup message from downloadPage
    connect(downloadPage, SIGNAL(sendPopupInfo(QString)), this, SLOT(popupInfo(QString)), Qt::QueuedConnection);

    // update from downloadPage to gauge
    connect(downloadPage, SIGNAL(sendUpdateGauge(double)), this, SLOT(updateGauge(double)), Qt::QueuedConnection);

}

MainWindow::~MainWindow()
{
    delete sessionManager;
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
    setWindowIcon(QIcon(":/images/logo_tsunami_tiny.ico"));
    setWindowTitle(PROJECT " " VERSION);
//    setWindowFlags(Qt::FramelessWindowHint);
    createTrayIcon();

    QFontDatabase::addApplicationFont(":/font/BEBAS___.TTF");

    connect(ui->btnSettings, SIGNAL(released()), this, SLOT(btnMenuClick()));
    connect(ui->btnUser, SIGNAL(released()), this, SLOT(btnMenuClick()));
    connect(ui->btnSearch, SIGNAL(released()), this, SLOT(btnMenuClick()));
    connect(ui->btnDownload, SIGNAL(released()), this, SLOT(btnMenuClick()));
    connect(ui->btnArchive, SIGNAL(released()), this, SLOT(btnMenuClick()));
    connect(ui->btnStreaming, SIGNAL(released()), this, SLOT(btnMenuClick()));
    connect(ui->btnStatistics, SIGNAL(released()), this, SLOT(btnMenuClick()));
    connect(ui->btnChat, SIGNAL(released()), this, SLOT(btnMenuClick()));

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

    ui->content->addWidget(settingsPage);
    ui->content->addWidget(searchPage);
    ui->content->addWidget(statisticsPage);
    ui->content->addWidget(downloadPage);

    p_scaleGauge = new QScale(this);
    p_scaleGauge->setInvertedAppearance(true);
    ui->contentGauge->addWidget(p_scaleGauge);

}

void MainWindow::readSettings()
{
    QSettings settings(settingsPage->settingsFileName, QSettings::IniFormat);

    settings.beginGroup("MainWindow");
    resize(settings.value("size", QSize(840, 670)).toSize());
    move(settings.value("pos", QPoint(200, 200)).toPoint());
    if ( settings.value("fullScreen", false).toBool() ) setWindowState(Qt::WindowFullScreen);
    settings.endGroup();
}

void MainWindow::writeSettings()
{
    QSettings settings(settingsPage->settingsFileName, QSettings::IniFormat);
    settings.beginGroup("MainWindow");
    settings.setValue("size", size());
    settings.setValue("pos", pos());
    settings.setValue("fullScreen", isFullScreen());
    settings.endGroup();
}


void MainWindow::createTrayIcon()
{
    // Tray icon
    m_systrayIcon = new QSystemTrayIcon(QIcon(":/images/logo_tsunami_tiny.ico"), this);

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
    downloadPage->setHidden(btn != ui->btnDownload->objectName());

    if (btn == ui->btnChat->objectName()) {
        QString link = "https://discordapp.com/invite/0pfzTOXuEjt9ifvF";
        QDesktopServices::openUrl(QUrl(link));
    }
}

void MainWindow::btnAddClick()
{
    QFileDialog fd(this, tr("Select torrent"), settingsPage->getLastBrowsedDir(), tr("Torrents (*.torrent)"));
    QStringList fileNames = fd.getOpenFileNames();
    if (fileNames.isEmpty()) return;
    settingsPage->setLastBrowsedDir(QFileInfo(fileNames.first()).absolutePath());
    sessionManager->addItems(std::move(fileNames), settingsPage->getDownloadPath());
    ui->btnDownload->released(); // switch to downloadpage
}

void MainWindow::popupInfo(const QString &msg)
{
    m_systrayIcon->showMessage(PROJECT, msg, QSystemTrayIcon::Information, TIME_TRAY_BALLOON);
}


void MainWindow::closeEvent(QCloseEvent *event)
{
    writeSettings();
    event->accept();
}
