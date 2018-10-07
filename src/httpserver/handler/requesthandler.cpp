#include "requesthandler.h"

RequestHandler::RequestHandler(QObject* parent)
    :HttpRequestHandler(parent)
{
    qDebug("RequestHandler created");
    QString configFileName = qApp->property("iniFilePath").toString();

    // Configure session store
    QSettings* sessionSettings=new QSettings(configFileName,QSettings::IniFormat);
    sessionSettings->beginGroup("sessions");
    sessionStore=new HttpSessionStore(sessionSettings, qApp);

    // Configure static file controller
    QSettings* fileSettings=new QSettings(configFileName,QSettings::IniFormat);
    fileSettings->beginGroup("docroot");
    staticFileController=new StaticFileController(fileSettings, qApp);
}

RequestHandler::~RequestHandler()
{
    qDebug("RequestHandler destroyed");
}

void RequestHandler::service(HttpRequest& request, HttpResponse& response)
{
    QByteArray path=request.getPath();
#ifdef SUPERVERBOSE
    qDebug("RequestMapper: path=%s",path.data());
#endif

    // For the following pathes, each request gets its own new instance of the related controller.
//    if (path.startsWith("/form"))
//    {
//        FormController().service(request, response);
//    }

    if (path.startsWith("/api/upload/"))
    {
        //FileUploadController().service(request, response);

        response.setHeader("Content-Type", "application/json");
        QString fileName = request.getParameter("inputFile[]");

        QTemporaryFile* file=request.getUploadedFile("inputFile[]");
        // file->fileName() returns the temporary filename used for upload, like "C:/Users/user/AppData/Local/Temp/Tsunami.gq9960"
        if (file)
        {
            QByteArray buffer = file->readAll();
            qDebug() << QString("uploaded file %0").arg(fileName);
            response.write("{}");
            emit fileUploaded(buffer, fileName);
        }
        else
        {
            response.write("upload failed");
        }
    }

    else if (path.startsWith("/api/uploadMagnet/"))
    {
        response.setHeader("Content-Type", "application/json");
        response.write("{}");
        QJsonDocument json = QJsonDocument::fromJson(request.getBody());
        if (!json.isEmpty() && json.isObject()) {
            QJsonObject jobj = json.object();
            if (jobj.contains("info")) {
                emit downloadMagnet(jobj["info"].toString());
            }
        }
    }

    else if (path.startsWith("/session"))
    {
        SessionController(sessionStore).service(request, response);
    }

    else if (path.startsWith("/api/torrentlist/"))
    {
        // torrent list requested
        response.setHeader("Content-Type", "application/json");
        response.write("{}");
        emit sendTorrentList();
    }

    else if (path.startsWith("/api/delete/"))
    {
        response.setHeader("Content-Type", "application/json");
        response.write("{}");
        QJsonDocument json = QJsonDocument::fromJson(request.getBody());
        if (!json.isEmpty() && json.isObject()) {
            QJsonObject jobj = json.object();
            if (jobj.contains("hash") && jobj.contains("deletefile")) {
                emit deleteTorrent(jobj["hash"].toString(), jobj["deletefile"].toBool());
            }
        }
    }

    else if (path.startsWith("/api/pause/"))
    {
        response.setHeader("Content-Type", "application/json");
        response.write("{}");
        QJsonDocument json = QJsonDocument::fromJson(request.getBody());
        if (!json.isEmpty() && json.isObject()) {
            QJsonObject jobj = json.object();
            if (jobj.contains("hash")) {
                emit pauseTorrent(jobj["hash"].toString());
            }
        }
    }

    else if (path.startsWith("/api/resume/"))
    {
        response.setHeader("Content-Type", "application/json");
        response.write("{}");
        QJsonDocument json = QJsonDocument::fromJson(request.getBody());
        if (!json.isEmpty() && json.isObject()) {
            QJsonObject jobj = json.object();
            if (jobj.contains("hash")) {
                emit resumeTorrent(jobj["hash"].toString());
            }
        }
    }

    else if (path.startsWith("/api/filelist/"))
    {
        response.setHeader("Content-Type", "application/json");
        response.write("{}");
        QJsonDocument json = QJsonDocument::fromJson(request.getBody());
        if (!json.isEmpty() && json.isObject()) {
            QJsonObject jobj = json.object();
            if (jobj.contains("hash")) {
                emit sendFileList(jobj["hash"].toString());
            }
        }
    }

    else if (path.startsWith("/api/socketport/"))
    {
        response.setHeader("Content-Type", "application/json");
        QJsonObject jobj;
        jobj.insert("port", p_socketPort);
        response.write(QJsonDocument(jobj).toJson(QJsonDocument::Compact));
    }

    else if (path.startsWith("/api/search/"))
    {
        response.setHeader("Content-Type", "application/json");
        response.write("{}");
        QJsonDocument json = QJsonDocument::fromJson(request.getBody());
        if (!json.isEmpty() && json.isObject()) {
            QJsonObject jobj = json.object();
            if (jobj.contains("text") && jobj.contains("category")) {
                emit searchRequested(jobj["text"].toString(), jobj["category"].toInt());
            }
        }
    }

    // All other pathes are mapped to the static file controller.
    // In this case, a single instance is used for multiple requests.
    else
    {
        staticFileController->service(request, response);
    }

#ifdef SUPERVERBOSE
    qDebug("RequestMapper: finished request");
#endif

}

void RequestHandler::setSocketPort(const int port)
{
    p_socketPort = port;
}
