#ifndef UPDATEMANAGER_H
#define UPDATEMANAGER_H

#include <QObject>

#include <QProcess>
#include <QDebug>
#include <QFile>
#include <QDir>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QSplashScreen>
#include <QPainter>
#include <QMetaEnum>
#include <QApplication>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QSettings>

#include <chrono>
#include <thread>

class updatemanager : public QObject
{
    Q_OBJECT
public:
    explicit updatemanager(QObject *parent = 0);

    void setAppUrl(QString path);
    void checkUpdate();

    void showSplashScreen(int millisecs);

    bool isFinished();
    bool appNeedRestart();

public slots:
    void readFromUpdateProcess();
    void updateProcessFinished(int finishCode, QProcess::ExitStatus exitStatus);

    void finishedDownloadingNewSplash(QNetworkReply *reply);

private:
    QProcess *p_checkProcess;
    QProcess *p_updateProcess;
    QString p_appDir;
//    QString p_url = "https://tsunami.adunanza.net/releases/Releases/"; http://tsunami.biuken.com/Releases/
    QString p_url = "http://tsunami.biuken.com/Releases/";
    QString p_cmd = "Update.exe";
    QString p_param = " --checkForUpdate=";
    QPixmap p_splash_img = QPixmap(":/images/adunanza.jpg");
    QSplashScreen p_splash;

    QNetworkAccessManager *p_nam;

    bool p_finished = false;
    bool p_appNeedRestart = false;

    void updateSplashMessage(QString message);
    void updateDownloadProgress(QString message);
    void processNewUpdate();
    void waitAndClose(int millisecs);
    void wait(int millisecs);
    void close();
};

#endif // UPDATEMANAGER_H
