#include <mutex> // needed for std::once_flag

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "bytevalue.h" // needed for CByteValue

constexpr int TIME_TRAY_BALLOON {5000}; //!< value in milliseconds to use for tray icons messages

const QString PROJECT("Tsunami++");

class CAppTitleString
{
private:
    const QString m_title;

public:
    CAppTitleString() : m_title(QString("%0 %1").arg(PROJECT).arg(VERSION))
    {
        qDebug() << QString("TITLE IS <%0>").arg(m_title);
    }

    const QString& get() const
    {
        return m_title;
    }
};


const QString& getProjectTitle()
{
    static CAppTitleString title_sigleton;
    return title_sigleton.get();
}

#if defined(Q_OS_MAC) || defined(Q_OS_LINUX)
void MainWindow::showEvent(QShowEvent* event)
{
    static std::once_flag title_set;
    QWidget::showEvent(event);

    std::call_once(title_set, [&] () {setWindowFilePath(getProjectTitle());});
}
#endif

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //setWindowFilePath(getProjectTitle()); on Mac this makes also the icon visible beside the name in caption

    p_session_thread = new QThread();
    sessionManager = new tsuManager();
    sessionManager->moveToThread(p_session_thread);

    settingsPage = new settingswindow(this);
    searchPage = new searchwindow(this);
    statisticsPage = new statisticswindow(this);
    downloadPage = new downloadwindow(this);
    previewPlayerPage = new PreviewPlayerWindow(this);
	archivePage = new archivewindow(this);
	
    statusLabel = new QLabel(this);

    initializeScreen();
    readSettings();
    loadLanguage();


    statusBar()->addPermanentWidget(statusLabel, 0);
    statusBar()->showMessage(tr("Welcome!"));

    /* FROM SESSIONMANAGER */
    // add goes to downloadPage to add tsuCard
    connect(sessionManager, SIGNAL(addFromSessionManager(const tsuEvents::tsuEvent &)), downloadPage, SLOT(addFromSession(const tsuEvents::tsuEvent &)));//, Qt::QueuedConnection);

    // update goes to downloadPage to manage tsuCard values
    connect(sessionManager, SIGNAL(updateFromSessionManager(const QVector<tsuEvents::tsuEvent> &)), downloadPage, SLOT(updateFromSession(const QVector<tsuEvents::tsuEvent> &)));//, Qt::DirectConnection);

    // deleted torrent goes to downloadPage to remove it from list
    connect(sessionManager, SIGNAL(torrentDeleted(std::string)), downloadPage, SLOT(torrentDeletedFromSession(std::string)));//, Qt::QueuedConnection);

    // assigned external ip goes to Main to update ico
    connect(sessionManager, &tsuManager::externalIpAssigned, this, &MainWindow::externalIpAssigned);

    // dht bootstrap signal goes to Main to update ico
    connect(sessionManager, &tsuManager::dhtBootstrapExecuted, this, &MainWindow::dhtBootstrapExecuted);

    // listening on port signal goes to Main to update ico
    connect(sessionManager, &tsuManager::listenerUpdate, this, &MainWindow::listenerUpdate);

    // session statistic update goes to Main to update UI
    connect(sessionManager, &tsuManager::sessionStatisticUpdate, this, &MainWindow::sessionStatisticUpdate);

    // search request from web (sessionManager) goes to searchPage
    connect(sessionManager, &tsuManager::web_NeedSearch, searchPage, &searchwindow::searchRequestFromWeb);


    /* FROM DOWNLOADPAGE */
    // requested cancel (emitted from a tsucard) goes to sessionManager to remove torrent from libtorrent
    connect(downloadPage, SIGNAL(sendRequestedCancelToSession(std::string,bool)), sessionManager, SLOT(getCancelRequest(std::string,bool)));//, Qt::QueuedConnection);

    // requested pause (emitted from a tsucard) goes to sessionManager to pause torrent
    connect(downloadPage, SIGNAL(sendRequestedPauseToSession(std::string)), sessionManager, SLOT(getPauseRequest(std::string)));//, Qt::QueuedConnection);

    // requested resume (emitted from a tsucard) goes to sessionManager to resume torrent
    connect(downloadPage, SIGNAL(sendRequestedResumeToSession(std::string)), sessionManager, SLOT(getResumeRequest(std::string)));//, Qt::QueuedConnection);

    // popup message goes to Main
    connect(downloadPage, SIGNAL(sendPopupInfo(QString)), this, SLOT(popupInfo(QString)));//, Qt::QueuedConnection);

    // update statusBar goes to Main
    connect(downloadPage, SIGNAL(sendMessageToStatusBar(const QString &)), this, SLOT(updateStatusBarLabel(const QString &)));

    // file dropped from graphicsscene goes to Main
    connect(downloadPage, SIGNAL(fileDropped(QString)), this, SLOT(fileDropped(QString)));


    /* FROM SETTINGSPAGE */
    // update statusBar goes to Main
    connect(settingsPage, SIGNAL(sendMessageToStatusBar(const QString &)), this, SLOT(updateStatusBarLabel(const QString &)));

    // update libtorrent settings goes to sessionManager
    connect(settingsPage, SIGNAL(sendRefreshSettings()), sessionManager, SLOT(refreshSettings()));


    /* FROM SEARCHPAGE */
    // request download goes to Main
    connect(searchPage, &searchwindow::downloadFromMagnet, this, &MainWindow::downloadFromSearchPage);

    // item found (for web) goes to web (through sessionManager)
    connect(searchPage, &searchwindow::web_itemFound, sessionManager, &tsuManager::web_itemFound);

    // search finished goes to web (through sessionManager)
    connect(searchPage, &searchwindow::web_finishedSearch, sessionManager, &tsuManager::web_finishedSearch);


    // Constructions events
    connect(p_session_thread, SIGNAL(started()), sessionManager, SLOT(startManager()));

    // Bind clipboard change to get magnet
    connect(QApplication::clipboard(), &QClipboard::dataChanged, this, &MainWindow::clipboardChanged);

    // Destructions events
    connect(this, SIGNAL(stopSessionManager()), sessionManager, SLOT(stopManager()), Qt::BlockingQueuedConnection);
    connect(sessionManager, SIGNAL(finished()), p_session_thread, SLOT(quit()));
    connect(sessionManager, SIGNAL(finished()), sessionManager, SLOT(deleteLater()));

    QString imgUrl = "image: url(:/images/state_warning.svg);";
    ui->lblIp->setStyleSheet(imgUrl);
    ui->lblDht->setStyleSheet(imgUrl);
    ui->lblTcp->setStyleSheet(imgUrl);
    ui->lblUdp->setStyleSheet(imgUrl);

    p_session_thread->start();

    ui->btnDownload->released(); // switch to downloadpage

    qDebug("created");
}

MainWindow::~MainWindow()
{
    qDebug("starting destroy");
    p_session_thread->deleteLater();
    qDebug("session thread executed deleteLater");
    delete p_gaugeSpeedLabel;
    delete p_gaugeUnitLabel;
    delete p_gaugeUpNeedle;
    delete p_gaugeDownNeedle;
    delete p_gaugeValuesItem;
    delete p_gaugeDegreesItem;
    delete p_gauge;
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

void MainWindow::updateGauge(const float &downValue, const float &upValue)
{
    if (downValue > p_gaugeDownNeedle->getMaxValue()) {
        p_gaugeValuesItem->setMaxValue(downValue);
        p_gaugeDegreesItem->setMaxValue(downValue);
        p_gaugeDownNeedle->setMaxValue(downValue);
    }

    if (upValue > p_gaugeUpNeedle->getMaxValue()) {
        p_gaugeUpNeedle->setMaxValue(upValue);
    }

    p_gaugeDownNeedle->setCurrentValue(downValue);
    p_gaugeUpNeedle->setCurrentValue(upValue);
}

void MainWindow::initializeScreen() {
    setWindowIcon(QIcon(":/images/logo_tsunami_tiny.ico"));
#if !defined(Q_OS_MAC) && !defined(Q_OS_LINUX)
    setWindowTitle(getProjectTitle());
#endif
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

    ui->stackedWidget->addWidget(settingsPage);
    ui->stackedWidget->addWidget(searchPage);
    ui->stackedWidget->addWidget(statisticsPage);
    ui->stackedWidget->addWidget(downloadPage);
    ui->stackedWidget->addWidget(previewPlayerPage);
    ui->stackedWidget->addWidget(archivePage);

    p_gauge = new QcGaugeWidget;
    p_gauge->addArc(75);
    p_gaugeDegreesItem = p_gauge->addDegrees(75);
    p_gaugeDegreesItem->setValueRange(0,20);

    p_gaugeValuesItem = p_gauge->addValues(90);
    p_gaugeValuesItem->setValueRange(0,20);

    p_gaugeUnitLabel = p_gauge->addLabel(70);
    p_gaugeUnitLabel->setText("Mbps");
    p_gaugeUnitLabel->setColor(Qt::green);

    p_gaugeUpNeedle = p_gauge->addNeedle(70);
    p_gaugeUpNeedle->setNeedle(QcNeedleItem::NeedleType::FeatherNeedle);
    p_gaugeUpNeedle->setColor(Qt::red);
    p_gaugeUpNeedle->setValueRange(0,20);

    p_gaugeSpeedLabel = p_gauge->addLabel(40);
    p_gaugeSpeedLabel->setText("0.0");
    p_gaugeSpeedLabel->setColor(Qt::green);

    p_gaugeDownNeedle = p_gauge->addNeedle(80);
    p_gaugeDownNeedle->setNeedle(QcNeedleItem::NeedleType::FeatherNeedle);
    p_gaugeDownNeedle->setLabel(p_gaugeSpeedLabel);
    p_gaugeDownNeedle->setColor(Qt::green);
    p_gaugeDownNeedle->setValueRange(0,20);

    ui->contentGauge->addWidget(p_gauge);

}

void MainWindow::readSettings()
{
    QSettings settings(qApp->property("iniFilePath").toString(), QSettings::IniFormat);

    settings.beginGroup("MainWindow");

    if ( settings.value("fullScreen", false).toBool() ) setWindowState(Qt::WindowFullScreen);
    resize(settings.value("size", QSize(840, 670)).toSize());
    move(settings.value("pos", QPoint(200, 200)).toPoint());

    settings.beginGroup("Gauge");
    float downValue = settings.value("maxDownloadValue", 20).toFloat();
    float upValue = settings.value("maxUploadValue", 20).toFloat();
    p_gaugeValuesItem->setMaxValue(downValue);
    p_gaugeDegreesItem->setMaxValue(downValue);
    p_gaugeDownNeedle->setMaxValue(downValue);
    p_gaugeUpNeedle->setMaxValue(upValue);
    settings.endGroup();

    settings.beginGroup("Filters");
    bool showDownloading = settings.value("showDownloading", true).toBool();
    bool showUploading = settings.value("showUploading", true).toBool();
    bool showChecking = settings.value("showChecking", true).toBool();
    bool showPaused = settings.value("showPaused", true).toBool();
    bool showError = settings.value("showError", true).toBool();
    qApp->setProperty("showDownloading", showDownloading);
    qApp->setProperty("showUploading", showUploading);
    qApp->setProperty("showChecking", showChecking);
    qApp->setProperty("showPaused", showPaused);
    qApp->setProperty("showError", showError);
    ui->btnStatDown->setChecked(showDownloading);
    ui->btnStatUp->setChecked(showUploading);
    ui->btnStatCheck->setChecked(showChecking);
    ui->btnStatPause->setChecked(showPaused);
    ui->btnStatError->setChecked(showError);
    settings.endGroup();

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
    settings.beginGroup("Gauge");
    settings.setValue("maxDownloadValue", p_gaugeDownNeedle->getMaxValue());
    settings.setValue("maxUploadValue", p_gaugeUpNeedle->getMaxValue());
    settings.endGroup();
    settings.beginGroup("Filters");
    settings.setValue("showDownloading", ui->btnStatDown->isChecked());
    settings.setValue("showUploading", ui->btnStatUp->isChecked());
    settings.setValue("showChecking", ui->btnStatCheck->isChecked());
    settings.setValue("showPaused", ui->btnStatPause->isChecked());
    settings.setValue("showError", ui->btnStatError->isChecked());
    settings.endGroup();
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
    m_systrayIcon->setToolTip(getProjectTitle());
    m_systrayIcon->show();

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
    bool playerWindowHiddenBefore = previewPlayerPage->isHidden();
    bool playerWindowHiddenAfter = !playerWindowHiddenBefore;

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

    if (btn == ui->btnSettings->objectName())
        ui->stackedWidget->setCurrentWidget(settingsPage);

//    if (btn == ui->btnUser->objectName())
//        ui->stackedWidget->setCurrentWidget(userPage);

    if (btn == ui->btnSearch->objectName())
        ui->stackedWidget->setCurrentWidget(searchPage);

    if (btn == ui->btnDownload->objectName())
        ui->stackedWidget->setCurrentWidget(downloadPage);

    if (btn == ui->btnArchive->objectName())
        ui->stackedWidget->setCurrentWidget(archivePage);

    if (btn == ui->btnStreaming->objectName()) {
        if (previewPlayerPage && settingsPage) {
            previewPlayerPage->setDownloadPath(settingsPage->getDownloadPath());
        }
        ui->stackedWidget->setCurrentWidget(previewPlayerPage);
    }

    if (btn == ui->btnStatistics->objectName())
        ui->stackedWidget->setCurrentWidget(statisticsPage);

    if (btn == ui->btnChat->objectName()) {
        QString link = "https://discordapp.com/invite/0pfzTOXuEjt9ifvF";
        QDesktopServices::openUrl(QUrl(link));
    }

    playerWindowHiddenAfter = previewPlayerPage->isHidden();
    if((playerWindowHiddenBefore != playerWindowHiddenAfter) && (playerWindowHiddenAfter))
    {
        previewPlayerPage->PauseForHidden();
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

void MainWindow::sessionStatisticUpdate(const quint64 &sent, const quint64 &received,
                                        const quint64 &downloading, const quint64 &uploading,
                                        const quint64 &checking, const quint64 &stopped,
                                        const quint64 &error, const quint64 &queuedDown, const quint64 &queuedSeed)
{
    QPair<float, float> pair = downloadPage->getRate();
    float& downRate = pair.first;
    float& upRate = pair.second;

    QString htmlDown = "<img src='qrc:/images/arrow_down.png'></img>";
    QString htmlUp = "<img src='qrc:/images/arrow_up.png'></img>";

    // just to minimize processing...
    QString downRate_inDU;
    QString downRate_DULabel;
    CByteValue::convertToRankValueAndGetStrings_rate(static_cast<uint64_t>(downRate), downRate_inDU, downRate_DULabel);

    QString upRate_inDU;
    QString upRate_DULabel;
    CByteValue::convertToRankValueAndGetStrings_rate(static_cast<uint64_t>(upRate), upRate_inDU, upRate_DULabel);

    QString received_inDU;
    QString received_DULabel;
    CByteValue::convertToRankValueAndGetStrings_size(static_cast<uint64_t>(received), received_inDU, received_DULabel);


    QString sent_inDU;
    QString sent_DULabel;
    CByteValue::convertToRankValueAndGetStrings_size(static_cast<uint64_t>(sent), sent_inDU, sent_DULabel);

    QString ups = QString("%0 %1%2/s (%3%4) / %5 %6%7/s (%8%9)").arg(htmlDown)
            .arg(downRate_inDU).arg(downRate_DULabel)
            .arg(received_inDU).arg(received_DULabel)
            .arg(htmlUp).arg(upRate_inDU).arg(upRate_DULabel)
            .arg(sent_inDU).arg(sent_DULabel);

    updateStatusBarStatistics(ups);

    updateGauge((downRate * 8)/1000000, (upRate * 8)/1000000);  // Ethernet 100 BASE-T -> http://www.convert-me.com/en/convert/data_transfer_rate/byte_s.html?u=byte%2Fs&v=1

    statisticsPage->updateStats(pair);

    ui->btnStatDown->setText(QString::number(downloading));
    ui->btnStatDown->setToolTip(QString("Downloading %0 torrents (%1 queued)").arg(downloading).arg(queuedDown));
    ui->btnStatUp->setText(QString::number(uploading));
    ui->btnStatUp->setToolTip(QString("Uploading %0 torrents (%1 queued)").arg(uploading).arg(queuedSeed));
    ui->btnStatCheck->setText(QString::number(checking));
    ui->btnStatCheck->setToolTip(QString("Checking %0 torrents").arg(checking));
    ui->btnStatPause->setText(QString::number(stopped));
    ui->btnStatPause->setToolTip(QString("%0 torrents stopped").arg(stopped));
    ui->btnStatError->setText(QString::number(error));
    ui->btnStatError->setToolTip(QString("%0 torrents with error").arg(error));
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    QMessageBox::StandardButton resBtn = QMessageBox::question( this, PROJECT,
                                                                tr("Are you sure you want to quit?\n"),
                                                                QMessageBox::No | QMessageBox::Yes,
                                                                QMessageBox::Yes);
    if (resBtn != QMessageBox::Yes) {
        event->ignore();
    } else {
        qDebug("closing");
        statusLabel->setText("Saving resume data. Please wait.");
        writeSettings();
        emit stopSessionManager();
        event->accept();
    }
}

void MainWindow::changeEvent(QEvent *e)
{
    if(e->type() == QEvent::LanguageChange)
    {
        qDebug() << "Intercepted LanguageChange event by MainWindow";
        ui->retranslateUi(this);
    }
    if (e->type() == QEvent::WindowStateChange && isMinimized())
    {
        hide();
    }

    QMainWindow::changeEvent(e);
}

void MainWindow::on_btnStatDown_toggled(bool checked)
{
    qApp->setProperty("showDownloading", checked);
    downloadPage->updateVisibility();
}

void MainWindow::on_btnStatUp_toggled(bool checked)
{
    qApp->setProperty("showUploading", checked);
    downloadPage->updateVisibility();
}

void MainWindow::on_btnStatCheck_toggled(bool checked)
{
    qApp->setProperty("showChecking", checked);
    downloadPage->updateVisibility();
}

void MainWindow::on_btnStatPause_toggled(bool checked)
{
    qApp->setProperty("showPaused", checked);
    downloadPage->updateVisibility();
}

void MainWindow::on_btnStatError_toggled(bool checked)
{
    qApp->setProperty("showError", checked);
    downloadPage->updateVisibility();
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    // http://doc.qt.io/qt-4.8/dnd.html
    QString fileName = event->mimeData()->text();
    QString msg;
    if (fileName.contains(".TORRENT", Qt::CaseInsensitive)) {
        msg = QString("accepted %0 from drag & drop").arg(fileName);
        event->acceptProposedAction();
    } else {
        msg = QString("wrong extension in file %0").arg(fileName);
        updateStatusBarLabel(msg);
    }
    qDebug() << msg;
}

void MainWindow::dropEvent(QDropEvent *event)
{
    fileDropped(event->mimeData()->text());
    event->acceptProposedAction();
}

void MainWindow::fileDropped(QString fileName)
{
    QStringList fileNames = fileName.replace("file:///", "").split("\n");
    sessionManager->addItems(std::move(fileNames), settingsPage->getDownloadPath());
    ui->btnDownload->released(); // switch to downloadpage
    updateStatusBarLabel(QString("file %0 added to download").arg(fileName.replace("file:///", "")));
}

void MainWindow::clipboardChanged()
{
    QClipboard *clipboard = QApplication::clipboard();
    QString clipboardText = clipboard->text();
    QString metaPrefix = "magnet:?xt=urn:btih:";
    if (clipboardText.mid(0, metaPrefix.length()) == metaPrefix) {
        qDebug() << "magnet paste detected";
        QStringList fileNames = clipboardText.split("\n");
        sessionManager->addFromMagnet(std::move(fileNames), settingsPage->getDownloadPath());
        ui->btnDownload->released(); // switch to downloadpage
        updateStatusBarLabel("file added from magnet");
    }
}

void MainWindow::downloadFromSearchPage(const QString magnet)
{
    QStringList fileNames;
    fileNames << magnet;
    sessionManager->addFromMagnet(std::move(fileNames), settingsPage->getDownloadPath());
    ui->btnDownload->released(); // switch to downloadpage
    updateStatusBarLabel("file added from search page");
}

void MainWindow::on_btnPatreon_released()
{
    QString link = "https://www.patreon.com/bePatron?u=3133703";
    QDesktopServices::openUrl(QUrl(link));
}
