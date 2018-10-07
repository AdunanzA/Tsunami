#include "settingswindow.h"
#include "ui_settingswindow.h"


settingswindow::settingswindow(QWidget *parent) :
    QWidget(parent),
    settings(qApp->property("iniFilePath").toString(), QSettings::IniFormat),
    ui(new Ui::settingswindow)
{
    ui->setupUi(this);

    ui->frameGeneral->setVisible(true); // default to General
    ui->frameAbout->setVisible(false);
    ui->frameMessages->setVisible(false);
    ui->frameTorrent->setVisible(false);
    ui->frameWeb->setVisible(false);

    ui->settingsMenu->item(0)->setSelected(true); // default to General

    loadSettings();
}

settingswindow::~settingswindow()
{
    delete ui;
    qDebug("settingswindow destroyed");
}

void settingswindow::loadSettings()
{
//    QSettings settings(settingsFileName, settingsFileFormat);
//    settings.sync();
//    QStringList Groups = settings.childGroups();
//    foreach (QString group, Groups) {
//        QTreeWidgetItem *wi = new QTreeWidgetItem(ui->treeWidget);
//        wi->setForeground(0, QColor(255,255,255));
//        wi->setText(0, group);
//        settings.beginGroup(group);
//        QStringList List = settings.allKeys();
//        foreach (QString item, List) {
//            QTreeWidgetItem *wi2 = new QTreeWidgetItem(wi);
//            wi2->setForeground(0, QColor(180,180,180));
//            wi2->setText(0, item);
//        }
//        settings.endGroup();
//    }

    QStorageInfo storage = QStorageInfo::root();
    QString downloadPath = settings.value("Download/downloadPath", storage.rootPath()).toString();

    int debugLevel = settings.value("Debug/Level", 1).toInt();
    int currentLanguage = settings.value("Language", 0).toInt();
    int downLimit = settings.value("libtorrent/download_rate_limit", 0).toInt();
    int upLimit = settings.value("libtorrent/upload_rate_limit", 0).toInt();
    int port = settings.value("libtorrent/port", 6881).toInt();

    // HTTP and WebSocket server settings
    bool webEnabled = settings.value("WebInterfaceOn", true).toBool();
    int webport = settings.value("listener/port", 8080).toInt();
    int websocketport = settings.value("websocket/port", 8081).toInt();

    bool msgOnFinish = settings.value("Messages/onFinish", true).toBool();
    bool msgOnChat = settings.value("Messages/onChat", false).toBool();


    ui->txtDownloadPath->setText(downloadPath);
    qDebug() << "download path" << downloadPath;

    ui->cmbDebugLevel->setCurrentIndex(debugLevel);
    qDebug() << "debug level" << ui->cmbDebugLevel->currentText();

    ui->cmbLanguage->setCurrentIndex(currentLanguage);
    qDebug() << "current language" << ui->cmbLanguage->currentText();

    ui->numLimitDown->setValue(downLimit);
    ui->numLimitUp->setValue(upLimit);
    qDebug() << QString("limits: download %0 KB/s, upload %1 KB/s").arg(downLimit).arg(upLimit);

    ui->numPort->setValue(port);
    qDebug() << "libtorrent listening on port" << port;

    ui->chkMsgDownFinish->setChecked(msgOnFinish);
    ui->chkMsgNewChat->setChecked(msgOnChat);
    ui->chkActivateWeb->setChecked(webEnabled);

    ui->numWebPort->setValue(webport);
    ui->numWebSocketPort->setValue(websocketport);

    // needed by downloadwindow when a tsuCard emit a downloadFinished event
    qApp->setProperty("msgOnFinish", ui->chkMsgDownFinish->isChecked());

    qInfo("settings loaded and applied");
}

void settingswindow::saveSettings()
{
    settings.setValue("Language", ui->cmbLanguage->currentIndex());
    qDebug() << "current language" << ui->cmbLanguage->currentText();

    settings.beginGroup("Download");
    settings.setValue("downloadPath", ui->txtDownloadPath->text());
    qDebug() << "download path" << ui->txtDownloadPath->text();
    settings.endGroup();

    settings.setValue("Debug/Level", ui->cmbDebugLevel->currentIndex());
    qDebug() << "debug level" << ui->cmbDebugLevel->currentText();

    settings.beginGroup("libtorrent");
    settings.setValue("download_rate_limit", ui->numLimitDown->value());
    settings.setValue("upload_rate_limit", ui->numLimitUp->value());
    settings.setValue("port", ui->numPort->value());
    settings.endGroup();

    settings.setValue("WebInterfaceOn", ui->chkActivateWeb->isChecked());
    settings.setValue("Messages/onFinish", ui->chkMsgDownFinish->isChecked());
    settings.setValue("Messages/onChat", ui->chkMsgNewChat->isChecked());

    // HTTP server settings
    settings.beginGroup("listener");
    settings.setValue("host", "localhost");
    settings.setValue("port", ui->numWebPort->value());
    settings.setValue("minThreads", 4);
    settings.setValue("maxThreads", 100);
    settings.setValue("cleanupInterval", 60000);
    settings.setValue("readTimeout", 60000);
    settings.setValue("maxRequestSize", 16000);
    settings.setValue("maxMultiPartSize", 10000000);
    settings.endGroup();

    settings.beginGroup("docroot");
    settings.setValue("path", "www");
    settings.setValue("encoding", "UTF-8");
    settings.setValue("maxAge", 0);
    settings.setValue("cacheTime", 0);
    settings.setValue("cacheSize", 1000000);
    settings.setValue("maxCachedFileSize", 65536);
    settings.endGroup();

    settings.beginGroup("sessions");
    settings.setValue("expirationTime", 3600000);
    settings.setValue("cookieName", "sessionid");
    settings.setValue("cookiePath", "/");
    settings.setValue("cookieComment", "Identifies the user");
    settings.setValue("cookieDomain", "tsunamiWeb");
    settings.endGroup();

    // WebSocket server
    settings.beginGroup("websocket");
    settings.setValue("port", ui->numWebSocketPort->value());
    settings.setValue("debug", false);
    settings.endGroup();

    settings.sync();

    // needed by downloadwindow when a tsuCard emit a downloadFinished event
    qApp->setProperty("msgOnFinish", ui->chkMsgDownFinish->isChecked());

    qInfo("settings saved");
}

void settingswindow::setLastBrowsedDir(const QString &path)
{
    settings.setValue("Download/lastBrowsedDir", path);
}

QString settingswindow::getLastBrowsedDir()
{
    return settings.value("Download/lastBrowsedDir", QCoreApplication::applicationDirPath()).toString();
}

QString settingswindow::getDownloadPath() const
{
    return ui->txtDownloadPath->text();
}

void settingswindow::on_btnSave_released()
{
    int savedLanguage = settings.value("Language", 0).toInt();
    int savedDownLimit = settings.value("libtorrent/download_rate_limit", 0).toInt();
    int savedUpLimit = settings.value("libtorrent/upload_rate_limit", 0).toInt();
    bool webEnabled = settings.value("WebInterfaceOn", true).toBool();

    bool needRestart = (ui->cmbLanguage->currentIndex() != savedLanguage);
    bool needRefreshTorrentSettings = ( (ui->numLimitDown->value() != savedDownLimit) ||
                                        (ui->numLimitUp->value() != savedUpLimit) ||
                                        (ui->chkActivateWeb->isChecked() != webEnabled));

    saveSettings();

    // send message settings saved to user
    emit sendMessageToStatusBar(QString("Settings saved"));
    qInfo("Settings saved");

    if (needRestart) {
        qDebug("need restart");
        QMessageBox mbox;
//        QString msg = QString("<center>Please restart Tsunami to apply new settings<br/>Do you want to restart now?</center>");
        QString msg = QString("<center>Please restart Tsunami to apply new settings<br/></center>");
        mbox.setText(msg);
        mbox.exec();
//        mbox.setStandardButtons( QMessageBox::Yes | QMessageBox::No );

//        bool proceedWithRestart = false;

//        switch (mbox.exec()) {
//            case QMessageBox::Yes:
//                proceedWithRestart = true;
//                break;
//            default:
//                proceedWithRestart = false;
//                break;
//        }
//        if (proceedWithRestart) {
//            qDebug("restarting");

//            // due to squirrel standard behaviour, qApp->arguments()[0] returs path to a tsunami version older than one just downloaded
//            // retrieve path of tsunami.exe loader outside current path and launch
//            QString fileName = QFileInfo(QCoreApplication::applicationFilePath()).fileName();
//            QDir oldPath(QCoreApplication::applicationDirPath());
//            oldPath.cdUp();
//            QString newPath = oldPath.filePath(fileName);

//            qApp->quit();
//            QProcess::startDetached(newPath, qApp->arguments());
//        } else {
//            if (needRefreshTorrentSettings) {
//                qDebug("send refresh to libtorrent");
//                emit sendRefreshSettings();
//            }
//        }
    } else {
        qDebug("don't need restart");
//        if (needRefreshTorrentSettings) {
//            qDebug("send refresh to libtorrent");
//            emit sendRefreshSettings();
//        }
    }

    if (needRefreshTorrentSettings) {
        qDebug("send refresh to libtorrent");
        emit sendRefreshSettings();
    }

}

void settingswindow::on_btnCancel_released()
{
    loadSettings();
}

int settingswindow::getDebugLevelIndex() const
{
    return ui->cmbDebugLevel->currentIndex();
}

int settingswindow::getCurrentLanguageIndex() const
{
    return ui->cmbLanguage->currentIndex();
}

void settingswindow::changeEvent(QEvent *e)
{
    if(e->type() == QEvent::LanguageChange)
    {
        qDebug("received QEvent::LanguageChange, retranslateUi");
        ui->retranslateUi(this);
        loadSettings();
    }
}

void settingswindow::on_btnFolder_released()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Choose download path"),
                                                 ui->txtDownloadPath->text(),
                                                 QFileDialog::ShowDirsOnly
                                                 | QFileDialog::DontResolveSymlinks);
    if (!dir.isEmpty()) {
        ui->txtDownloadPath->setText(dir);
    }
}

void settingswindow::on_commandLinkButton_released()
{
    changelog *cl = new changelog();
    cl->exec();
}

void settingswindow::on_settingsMenu_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
{
    Q_UNUSED(current);
    Q_UNUSED(previous);

    int row = ui->settingsMenu->currentRow();

    ui->frameGeneral->setVisible(row == 0);
    ui->frameTorrent->setVisible(row == 1);
    ui->frameMessages->setVisible(row == 2);
    ui->frameWeb->setVisible(row == 3);
    ui->frameAbout->setVisible(row == 4);

}

void settingswindow::on_btnOpenWeb_released()
{
    QString link = QString("http://%0:%1").arg(settings.value("listener/host", "localhost").toString()).arg(settings.value("listener/port", 8080).toString());
    QDesktopServices::openUrl(QUrl(link));
}
