#include "mainwindow.h"
#include "ui_mainwindow.h"

#define TIME_TRAY_BALLOON 5000
#define PROJECT "Tsunami++"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    p_session_thread = new QThread();
    sessionManager = new tsuManager();
    sessionManager->moveToThread(p_session_thread);

    settingsPage = new settingswindow(this);
    searchPage = new searchwindow(this);
    statusLabel = new QLabel(this);
    statisticsPage = new statisticswindow(this);
    downloadPage = new downloadwindow(this);

    initializeScreen();
    readSettings();
    loadLanguage();

    statusBar()->addPermanentWidget(statusLabel, 0);
    statusBar()->showMessage("Welcome!");

    // add from SessionManager goes to downloadPage to add tsuCard
    connect(sessionManager, SIGNAL(addFromSessionManager(const tsuEvents::tsuEvent &)), downloadPage, SLOT(addFromSession(const tsuEvents::tsuEvent &)));//, Qt::QueuedConnection);

    // update from sessionManager goes to downloadPage to manage tsuCard values
    connect(sessionManager, SIGNAL(updateFromSessionManager(const QVector<tsuEvents::tsuEvent> &)), downloadPage, SLOT(updateFromSession(const QVector<tsuEvents::tsuEvent> &)));//, Qt::DirectConnection);

    // deleted torrent from sessionManager goes to downloadPage to remove it from list
    connect(sessionManager, SIGNAL(torrentDeleted(std::string)), downloadPage, SLOT(torrentDeletedFromSession(std::string)));//, Qt::QueuedConnection);

    // assigned external ip from sessionManager goes to Main to update ico
    connect(sessionManager, &tsuManager::externalIpAssigned, this, &MainWindow::externalIpAssigned);

    // dht bootstrap signal from sessionManager goes to Main to update ico
    connect(sessionManager, &tsuManager::dhtBootstrapExecuted, this, &MainWindow::dhtBootstrapExecuted);

    // listening on port signal from sessionManager goes to Main to update ico
    connect(sessionManager, &tsuManager::listenerUpdate, this, &MainWindow::listenerUpdate);

    // requested cancel from downloadPage (emitted from a tsucard) goes to sessionManager to remove torrent from libtorrent
    connect(downloadPage, SIGNAL(sendRequestedCancelToSession(std::string,bool)), sessionManager, SLOT(getCancelRequest(std::string,bool)));//, Qt::QueuedConnection);

    // requested pause from downloadPage (emitted from a tsucard) goes to sessionManager to pause torrent
    connect(downloadPage, SIGNAL(sendRequestedPauseToSession(std::string)), sessionManager, SLOT(getPauseRequest(std::string)));//, Qt::QueuedConnection);

    // requested resume from downloadPage (emitted from a tsucard) goes to sessionManager to resume torrent
    connect(downloadPage, SIGNAL(sendRequestedResumeToSession(std::string)), sessionManager, SLOT(getResumeRequest(std::string)));//, Qt::QueuedConnection);

    // update from downloadPage to statusbar text
    connect(downloadPage, SIGNAL(sendUpdateToStatusBar(const QString &)), this, SLOT(updateStatusBarStatistics(const QString &)));//, Qt::QueuedConnection);

    // update from downloadPage to statistics page
    connect(downloadPage, SIGNAL(sendStatisticsUpdate(const QPair<int,int> &)), statisticsPage, SLOT(updateStats(const QPair<int,int> &)));//, Qt::QueuedConnection);

    // popup message from downloadPage
    connect(downloadPage, SIGNAL(sendPopupInfo(QString)), this, SLOT(popupInfo(QString)));//, Qt::QueuedConnection);

    // update from downloadPage to gauge
    connect(downloadPage, SIGNAL(sendUpdateGauge(double, double)), this, SLOT(updateGauge(double, double)));//, Qt::QueuedConnection);

    // update statusBar from downloadPage
    connect(downloadPage, SIGNAL(sendMessageToStatusBar(const QString &)), this, SLOT(updateStatusBarLabel(const QString &)));

    // update statusBar from settings
    connect(settingsPage, SIGNAL(sendMessageToStatusBar(const QString &)), this, SLOT(updateStatusBarLabel(const QString &)));

    // update libtorrent settings from settingsWindow
    connect(settingsPage, SIGNAL(sendRefreshSettings()), sessionManager, SLOT(refreshSettings()));

    // Constructions events
    connect(p_session_thread, SIGNAL(started()), sessionManager, SLOT(startManager()));

    // Destructions events
    connect(this, SIGNAL(stopSessionManager()), sessionManager, SLOT(stopManager()), Qt::BlockingQueuedConnection);
    connect(sessionManager, SIGNAL(finished()), p_session_thread, SLOT(quit()));
    connect(sessionManager, SIGNAL(finished()), sessionManager, SLOT(deleteLater()));
//    connect(p_session_thread, SIGNAL(finished()), p_session_thread, SLOT(deleteLater()));

    p_session_thread->start();
    qDebug("MainWindow started");

    QString imgUrl = "image: url(:/images/state_warning.svg);";
    ui->lblIp->setStyleSheet(imgUrl);
    ui->lblDht->setStyleSheet(imgUrl);
    ui->lblTcp->setStyleSheet(imgUrl);
    ui->lblUdp->setStyleSheet(imgUrl);

}

MainWindow::~MainWindow()
{
    qDebug("starting destroy");
//    p_session_thread->wait();
//    qDebug("session thread waited");
    p_session_thread->deleteLater();
    qDebug("session thread executed deleteLater");
    delete ui;
    qDebug("ui deleted");
}

void MainWindow::updateStatusBarStatistics(const QString &msg)
{
    statusLabel->setText(msg);
}

void MainWindow::updateStatusBarLabel(const QString &msg)
{
    statusBar()->showMessage(msg);
}

void MainWindow::updateGauge(const double &downValue, const double &upValue)
{
//    p_scaleGauge->setValue(value);
    p_downNeedle->setCurrentValue(downValue);
    p_upNeedle->setCurrentValue(upValue);
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

//    p_scaleGauge = new QScale(this);
//    p_scaleGauge->setInvertedAppearance(true);
//    ui->contentGauge->addWidget(p_scaleGauge);
    p_speedGauge = new QcGaugeWidget;
//    p_SpeedGauge->addBackground(99);

//    QcBackgroundItem *bkg1 = p_SpeedGauge->addBackground(92);
//    bkg1->clearColors();
//    bkg1->addColor(0.1f,Qt::black);
//    bkg1->addColor(1.0,Qt::white);

//    QcBackgroundItem *bkg2 = p_SpeedGauge->addBackground(88);
//    bkg2->clearColors();
//    bkg2->addColor(0.1f,Qt::gray);
//    bkg2->addColor(1.0,Qt::darkGray);

    p_speedGauge->addArc(75);
    p_speedGauge->addDegrees(75)->setValueRange(0,100);

    p_speedGauge->addValues(90)->setValueRange(0,100);
    QcLabelItem *labelType = p_speedGauge->addLabel(70);
    labelType->setText("Mbps");
    labelType->setColor(Qt::green);

    p_upNeedle = p_speedGauge->addNeedle(70);
    p_upNeedle->setNeedle(QcNeedleItem::NeedleType::FeatherNeedle);
    p_upNeedle->setColor(Qt::red);
    p_upNeedle->setValueRange(0,100);

    QcLabelItem *labelDown = p_speedGauge->addLabel(40);
    labelDown->setText("0.0");
    labelDown->setColor(Qt::green);
    p_downNeedle = p_speedGauge->addNeedle(80);
    p_downNeedle->setNeedle(QcNeedleItem::NeedleType::FeatherNeedle);
    p_downNeedle->setLabel(labelDown);
    p_downNeedle->setColor(Qt::green);
    p_downNeedle->setValueRange(0,100);

//    p_SpeedGauge->addBackground(7);
//    p_SpeedGauge->addGlass(88);
    ui->contentGauge->addWidget(p_speedGauge);

}

void MainWindow::readSettings()
{
    QSettings settings(qApp->property("iniFilePath").toString(), QSettings::IniFormat);

    settings.beginGroup("MainWindow");
    if ( settings.value("fullScreen", false).toBool() ) setWindowState(Qt::WindowFullScreen);
    resize(settings.value("size", QSize(840, 670)).toSize());
    move(settings.value("pos", QPoint(200, 200)).toPoint());
    settings.endGroup();
    qDebug() << "Settings read";

    bool justUpdated = settings.value("justUpdated", false).toBool();
    if (justUpdated) {
        settings.setValue("justUpdated", false);
        settings.sync();
        qDebug("just updated, showing change log");
        changelog *cl = new changelog();
        cl->exec();
    }

}

void MainWindow::writeSettings()
{
    QSettings settings(qApp->property("iniFilePath").toString(), QSettings::IniFormat);

    settings.beginGroup("MainWindow");
    settings.setValue("fullScreen", isFullScreen());
    settings.setValue("size", size());
    settings.setValue("pos", pos());
    settings.endGroup();

    settingsPage->saveSettings();

    qDebug() << "Settings wrote";
}

void MainWindow::loadLanguage()
{
//    if (languageIndex.isNull()) {
//        qDebug() << "MainWindow received empty language, ignoring.";
//        return;
//    }
    int languageIndex = settingsPage->getCurrentLanguageIndex();
    qDebug() << QString("Requested language index %0").arg(languageIndex);

    bool response = false;
    if(languageIndex == 1)
    {
        response = p_translator.load(":/languages/italian.qm");
        qDebug() << QString("Requested language index %0 loaded %1").arg(languageIndex).arg((response) ? "true" : "false");
    }
    if(languageIndex != 0)
    {
        response = QCoreApplication::installTranslator(&p_translator);
        qDebug() << QString("Requested language index %0 applied %1").arg(languageIndex).arg((response) ? "true" : "false");
    } else {
        qDebug("Default language applied");
    }
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
    qInfo("started");
}

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

void MainWindow::externalIpAssigned()
{
    ui->lblIp->setStyleSheet("image: url(:/images/state_connected.svg);");
}

void MainWindow::dhtBootstrapExecuted()
{
    ui->lblDht->setStyleSheet("image: url(:/images/state_connected.svg);");
}

void MainWindow::listenerUpdate(QString type, bool success)
{
    QString imgUrl = "image: url(:/images/state_connected.svg);";
    if (!success) {
        imgUrl = "image: url(:/images/state_disconnected.svg);";
    }

    if (type == "tcp") {
        ui->lblTcp->setStyleSheet(imgUrl);
    } else {
        ui->lblUdp->setStyleSheet(imgUrl);
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    qDebug("closing");
    statusLabel->setText("Saving resume data. Please wait.");
    writeSettings();
//    updateTsunami();
    emit stopSessionManager();
    event->accept();
}

void MainWindow::changeEvent(QEvent *e)
{
    if(e->type() == QEvent::LanguageChange)
    {
        qDebug() << "Intercepted LanguageChange event by MainWindow";
        ui->retranslateUi(this);
    }

    QMainWindow::changeEvent(e);
}
