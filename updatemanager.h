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
#include <QStandardPaths>
#include <QDirIterator>

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
    QString p_url = "https://tsunami.adunanza.net/updates/Releases/";
    QString p_cmd = "Update.exe";
    QString p_param = " --checkForUpdate=";
    QPixmap p_splash_img = QPixmap(":/images/adunanza.jpg");
    QString p_splash_url = "https://tsunami.adunanza.net/updates/splash.jpg";
    QString p_scripts_text_url = "https://tsunami.adunanza.net/updates/scripts.txt";
    QString p_scripts_url = "https://tsunami.adunanza.net/updates/scripts/%0";
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

    void updateSearchScripts();
};

#endif // UPDATEMANAGER_H
