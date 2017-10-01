#include "updatemanager.h"

#ifdef Q_OS_WIN
#include <windows.h>
QString shortPathName(const QString & file) // returns 8.3 file format from long path
{
    wchar_t* input=new wchar_t[file.size()+1];
    file.toWCharArray(input);
    input[file.size()]=L'\0'; // terminate string
    long length = GetShortPathName(input, NULL, 0);
    wchar_t* output=new wchar_t[length];
    GetShortPathName(input,output,length);
    QString ret=QString::fromWCharArray(output,length-1); // discard
    delete [] input;
    delete [] output;
    return ret;
}
#endif

updatemanager::updatemanager(QObject *parent) : QObject(parent)
{
    p_checkProcess = new QProcess(this);
    p_checkProcess->setProcessChannelMode(QProcess::MergedChannels); // merge process stdout to app stdout

    p_updateProcess = new QProcess(this);
    p_updateProcess->setProcessChannelMode(QProcess::MergedChannels); // merge process stdout to app stdout

    // short connect version
    connect(p_updateProcess, &QProcess::readyRead, this, &updatemanager::readFromUpdateProcess);

    // cannot use short connect version because finished has overloads
    connect(p_updateProcess, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(updateProcessFinished(int,QProcess::ExitStatus)));

    p_nam = new QNetworkAccessManager(this);
//    connect(p_nam, &QNetworkAccessManager::finished, this, &updatemanager::finishedDownloadingNewSplash);

    // download new splashscreen
    QSettings settings(qApp->property("iniFilePath").toString(), QSettings::IniFormat);
    QString downloadUrl = settings.value("Download/splashUrl", "http://tsunami.biuken.com/splash.jpg").toString();
    if (!downloadUrl.isEmpty()) {
        QUrl url(downloadUrl);
        QNetworkRequest request(url);
//        p_nam->get(request);
        QNetworkReply *reply = p_nam->get(request);

        QEventLoop loop;
        connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
        loop.exec();

        if (reply->error() == QNetworkReply::NoError) {
            QByteArray jpegData = reply->readAll();
            QPixmap pixmap;
            pixmap.loadFromData(jpegData);
 //            p_splash_img.loadFromData(reply->readAll());
            if (pixmap.isNull()) qDebug("image is null");
            p_splash_img = pixmap;
            qDebug("new splash downloaded");
        } else {
            qDebug() << QString("cannot download new splash, error %0").arg(reply->errorString());
        }
    }

    QString version = VERSION;
    QFont font("Arial", 9, QFont::Normal);
    QPen pen(Qt::white);
    int paddingRight = 9;
    int paddingBottom = 9;

    QPainter painter(&p_splash_img);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(pen);
    painter.setFont(font);
    painter.drawText(p_splash_img.width() - paddingRight - painter.fontMetrics().width(version), p_splash_img.height() - paddingBottom, version);

    p_splash.setPixmap(p_splash_img);
    p_splash.setWindowFlags(p_splash.windowFlags() | Qt::WindowStaysOnTopHint);
    p_splash.setDisabled(true);
    p_splash.setFont(font);

    setAppUrl(QApplication::applicationDirPath());

    qDebug("created");
}

void updatemanager::setAppUrl(QString path)
{
#ifdef Q_OS_WIN
    p_appDir = shortPathName(path);
#else
    p_appDir = path;
#endif

    p_appDir = path;
    if (p_appDir.right(1) != "/") {
        p_appDir += "/";
    }
    p_appDir += "../";
    p_appDir = QDir::toNativeSeparators(p_appDir);
}

void updatemanager::checkUpdate()
{
    qInfo("checking for update");
    p_finished = false;
    p_splash.show();

    if ( !QFile::exists(p_appDir + p_cmd) ) {
        updateSplashMessage("Cannot check for new version");
        qWarning("Update.exe not found");
        waitAndClose(3000);
        return;
    } else {
        updateSplashMessage("Checking for updates");
        wait(2000);
    }

    QString stringCmd = QString("%0%1%2%3").arg(p_appDir).arg(p_cmd).arg(p_param).arg(p_url);
    qDebug() << "Executing " << stringCmd;
    p_checkProcess->start(stringCmd);
    p_checkProcess->waitForFinished();
    if (p_checkProcess->error() != QProcess::UnknownError)
    {
        qWarning() << "Error checking for updates" << p_checkProcess->state();
        updateSplashMessage("Error checking for updates, open log for details");
        waitAndClose(3000);
        return;
    }

    QString output(p_checkProcess->readAllStandardOutput());
    if (output.isNull() || output.isEmpty()) {
        qWarning() << "No values from update";
        updateSplashMessage("Error checking for updates, open log for details");
        waitAndClose(3000);
        return;
    }

    QList<QString> outpieces = output.split("\r\n");
    QString jsonString = "";

    foreach (QString piece, outpieces) {
        if (piece.left(1) == "{") {
            jsonString = piece;
            break;
        }
    }

    if (jsonString.isNull() || jsonString.isEmpty()) {
        qWarning() << "Cannot parse Update output";
        updateSplashMessage("Error checking for updates, open log for details");
        waitAndClose(3000);
        return;
    }

    QJsonDocument doc(QJsonDocument::fromJson(jsonString.toUtf8()));
    if (doc.isNull() || doc.isEmpty()) {
        qWarning() << "Cannot parse Update document";
        updateSplashMessage("Error checking for updates, open log for details");
        waitAndClose(3000);
        return;
    }

    QJsonObject json = doc.object();
    if (json.isEmpty()) {
        qWarning() << "Cannot parse Update json";
        updateSplashMessage("Error checking for updates, open log for details");
        waitAndClose(3000);
        return;
    }

    if (json["currentVersion"].toString() == json["futureVersion"].toString()) {
        QString noupdateNeeded("No update needed, latest version installed");
        qInfo() << noupdateNeeded;
        updateSplashMessage(noupdateNeeded);
        waitAndClose(3000);
        return;
    }

    updateSplashMessage("Updating to v" + json["futureVersion"].toString());
    wait(2000);
    processNewUpdate();
}

void updatemanager::showSplashScreen(int millisecs)
{
    p_splash.show();
    waitAndClose(millisecs);
}

void updatemanager::processNewUpdate()
{
    qInfo("New version available, downloading");
    p_param = " --update=";

    QString stringCmd = QString("%0%1%2%3").arg(p_appDir).arg(p_cmd).arg(p_param).arg(p_url);
    qDebug() << "Executing " << stringCmd;
    p_updateProcess->start(stringCmd);

}

bool updatemanager::isFinished()
{
    return p_finished;
}

bool updatemanager::appNeedRestart()
{
    return p_appNeedRestart;
}

void updatemanager::readFromUpdateProcess()
{
    QString msg = QString(p_updateProcess->readLine()).simplified();
//    qDebug() << msg.toUtf8().constData();
    updateDownloadProgress(msg);
}

void updatemanager::updateProcessFinished(int finishCode, QProcess::ExitStatus exitStatus)
{
    Q_UNUSED(finishCode)
    QMetaEnum metaEnum = QMetaEnum::fromType<QProcess::ExitStatus>();
    qDebug() << "update finished " << metaEnum.valueToKey(exitStatus);
    updateSplashMessage("Update finished, restarting");
    p_appNeedRestart = true;
    waitAndClose(3000);
    return;
}

void updatemanager::finishedDownloadingNewSplash(QNetworkReply *reply)
{
    if (reply->error() == QNetworkReply::NoError) {
        QByteArray jpegData = reply->readAll();
        QPixmap pixmap;
        pixmap.loadFromData(jpegData);
        if (pixmap.isNull()) qDebug("image is null");
        p_splash_img = pixmap;
        qDebug("new splash downloaded");
    } else {
        qDebug() << QString("cannot download new splash, error %0").arg(reply->errorString());
    }
}

void updatemanager::updateSplashMessage(QString message) {
    if (message.isNull() || message.isEmpty()) return;
    p_splash.showMessage(message, Qt::AlignLeft | Qt::AlignBottom, QColor(Qt::white));
}

void updatemanager::updateDownloadProgress(QString message) {
    if (message.isNull() || message.isEmpty()) return;
    p_splash.showMessage(QString("Downloading %0%").arg(message), Qt::AlignLeft | Qt::AlignBottom, QColor(Qt::white));
}


void updatemanager::waitAndClose(int millisecs)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(millisecs));
    close();
}

void updatemanager::wait(int millisecs)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(millisecs));
}

void updatemanager::close()
{
    p_splash.close();
    p_finished = true;
    this->deleteLater();
}
