#ifndef REQUESTHANDLER_H
#define REQUESTHANDLER_H

#include <QObject>
#include <QApplication>
#include <QJsonDocument>
#include <QJsonObject>

#include <src/httpserver/httprequesthandler.h>
#include <src/httpserver/httpsessionstore.h>
#include <src/httpserver/staticfilecontroller.h>
#include <src/httpserver/controller/formcontroller.h>
#include <src/httpserver/controller/fileuploadcontroller.h>
#include <src/httpserver/controller/sessioncontroller.h>

using namespace stefanfrings;

class RequestHandler : public HttpRequestHandler
{
    Q_OBJECT
    Q_DISABLE_COPY(RequestHandler)

public:
    RequestHandler(QObject* parent=0);
    ~RequestHandler();

    HttpSessionStore* sessionStore;
    StaticFileController* staticFileController;

    void service(HttpRequest& request, HttpResponse& response);

    void setSocketPort(const int port);

signals:
//    void sendSingleTorrentInfo(const QString &hash);
    void sendTorrentList();
    void deleteTorrent(const QString hash, const bool deleteFile);
    void pauseTorrent(const QString hash);
    void resumeTorrent(const QString hash);
    void sendFileList(const QString hash);
    void searchRequested(const QString textToSearch, const int category);
    void fileUploaded(const QByteArray buffer, const QString fileName);
    void downloadMagnet(const QString magnet);

private:
    int p_socketPort = 0;
};

#endif // REQUESTHANDLER_H
