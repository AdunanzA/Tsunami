/**
  @file
  @author Stefan Frings
*/

#include "staticfilecontroller.h"
#include <QFileInfo>
#include <QDir>
#include <QDateTime>
#include <QStandardPaths>

using namespace stefanfrings;

StaticFileController::StaticFileController(QSettings* settings, QObject* parent)
    :HttpRequestHandler(parent)
{
    maxAge=settings->value("maxAge","60000").toInt();
    encoding=settings->value("encoding","UTF-8").toString();
    docroot=settings->value("path","www").toString();
//    if(!(docroot.startsWith(":/") || docroot.startsWith("qrc://")))
//    {
//        // Convert relative path to absolute, based on the directory of the config file.
//        #ifdef Q_OS_WIN32
//            if (QDir::isRelativePath(docroot) && settings->format()!=QSettings::NativeFormat)
//        #else
//            if (QDir::isRelativePath(docroot))
//        #endif
//        {
//            QFileInfo configFile(settings->fileName());
//            docroot=QFileInfo(configFile.absolutePath(),docroot).absoluteFilePath();
//        }
//    }

//    docroot = QString("%0/%1").arg(QDir::currentPath()).arg(docroot);
    docroot = QString("%0/%1").arg(QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation)).arg(docroot);
    docroot = QDir::toNativeSeparators(docroot);

    qDebug("StaticFileController: docroot=%s, encoding=%s, maxAge=%i",qPrintable(docroot),qPrintable(encoding),maxAge);
    maxCachedFileSize=settings->value("maxCachedFileSize","65536").toInt();
    cache.setMaxCost(settings->value("cacheSize","1000000").toInt());
    cacheTimeout=settings->value("cacheTime","60000").toInt();
    qDebug("StaticFileController: cache timeout=%i, size=%i",cacheTimeout,cache.maxCost());
}


void StaticFileController::service(HttpRequest& request, HttpResponse& response)
{
    QByteArray path=request.getPath();
    // Check if we have the file in cache
    qint64 now=QDateTime::currentMSecsSinceEpoch();
    mutex.lock();
    CacheEntry* entry=cache.object(path);

    // added false to always miss cache
    if (false && entry && (cacheTimeout==0 || entry->created>now-cacheTimeout))
    {
        QByteArray document=entry->document; //copy the cached document, because other threads may destroy the cached entry immediately after mutex unlock.
        QByteArray filename=entry->filename;
        mutex.unlock();
#ifdef SUPERVERBOSE
        qDebug("StaticFileController: Cache hit for %s",path.data());
#endif
        setContentType(filename,response);
        response.setHeader("Cache-Control","max-age="+QByteArray::number(maxAge/1000));
        response.write(document);
    }
    else
    {
        mutex.unlock();
        // The file is not in cache.
#ifdef SUPERVERBOSE
        qDebug("StaticFileController: Cache miss for %s",path.data());
#endif

        // Forbid access to files outside the docroot directory
        if (path.contains("/.."))
        {
            qWarning("StaticFileController: detected forbidden characters in path %s",path.data());
            response.setStatus(403,"forbidden");
            response.write("403 forbidden",true);
            return;
        }

        // if is request of a .map file, ignore it
        if (path.endsWith(".map")) {
            response.setStatus(404,"not found");
            response.write("404 not found",true);
            return;
        }

        // If the filename is a directory, append index.html.
        if (QFileInfo(docroot+path).isDir())
        {
            path+="/index.html";
        }

        // Try to open the file
        QFile file(docroot+path);
#ifdef SUPERVERBOSE
        qDebug("StaticFileController: Open file %s",qPrintable(file.fileName()));
#endif
        if (file.open(QIODevice::ReadOnly))
        {
            setContentType(path,response);
            response.setHeader("Cache-Control","max-age="+QByteArray::number(maxAge/1000));
            if (file.size()<=maxCachedFileSize)
            {
                // Return the file content and store it also in the cache
                entry = new CacheEntry();
                while (!file.atEnd() && !file.error())
                {
                    QByteArray buffer=file.read(65536);
                    response.write(buffer);
                    entry->document.append(buffer);
                }
                entry->created=now;
                entry->filename=path;
                mutex.lock();
                cache.insert(request.getPath(),entry,entry->document.size());
                mutex.unlock();
            }
            else
            {
                // Return the file content, do not store in cache
                while (!file.atEnd() && !file.error())
                {
                    response.write(file.read(65536));
                }
            }
            file.close();
        }
        else {
            if (file.exists())
            {
                qWarning("StaticFileController: Cannot open existing file %s for reading", qPrintable(file.fileName()));
                response.setStatus(403,"forbidden");
                response.write("403 forbidden",true);
            }
            else
            {
                qDebug("requesting download for file %s",qPrintable(file.fileName()));

                // try to download from web
                QString localWww = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation); // win -> C:\Users\user\AppData\Local\Tsunami
                localWww = QString("%0/%1").arg(localWww).arg("www");
                localWww = QDir::toNativeSeparators(localWww);

                QStringList parts = file.fileName().split("/");
                QString dir = file.fileName().replace(parts.at(parts.length() - 1), "");
                if (!QDir(dir).exists()) {
                    QDir().mkpath(dir);
                }

                QNetworkAccessManager *nam = new QNetworkAccessManager();

                if (path.left(1) == "/" ) path = path.mid(1);
                QString stringUrl = QString("%0%1").arg(downloadUrl).arg(path.data());//.replace("//", "/");

                QUrl url(stringUrl);
                qDebug() << "requested url" << url;
                QNetworkRequest request(url);
                QNetworkReply *reply = nam->get(request);

                QEventLoop loop;
                connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
                loop.exec();

                if (reply->error() == QNetworkReply::NoError) {
                    QByteArray fileArray = reply->readAll();
                    QFile fileLocal(file.fileName());
                    fileLocal.open(QIODevice::WriteOnly);
                    fileLocal.write(fileArray);
                    fileLocal.close();
                    qDebug() << file.fileName() << "downloaded and saved";
                    setContentType(path,response);
                    response.setHeader("Cache-Control","max-age="+QByteArray::number(maxAge/1000));
                    response.write(fileArray);
                } else {
                    response.setStatus(404,"not found");
                    response.write("404 not found",true);
                }
                reply->deleteLater();
                nam->deleteLater();
            }
        }
    }
}

void StaticFileController::setContentType(QString fileName, HttpResponse& response) const
{
    if (fileName.endsWith(".png"))
    {
        response.setHeader("Content-Type", "image/png");
    }
    else if (fileName.endsWith(".jpg"))
    {
        response.setHeader("Content-Type", "image/jpeg");
    }
    else if (fileName.endsWith(".gif"))
    {
        response.setHeader("Content-Type", "image/gif");
    }
    else if (fileName.endsWith(".ico"))
    {
        response.setHeader("Content-Type", "image/x-icon");
    }
    else if (fileName.endsWith(".pdf"))
    {
        response.setHeader("Content-Type", "application/pdf");
    }
    else if (fileName.endsWith(".txt"))
    {
        response.setHeader("Content-Type", qPrintable("text/plain; charset="+encoding));
    }
    else if (fileName.endsWith(".html") || fileName.endsWith(".htm"))
    {
        response.setHeader("Content-Type", qPrintable("text/html; charset="+encoding));
    }
    else if (fileName.endsWith(".css"))
    {
        response.setHeader("Content-Type", "text/css");
    }
    else if (fileName.endsWith(".js"))
    {
        response.setHeader("Content-Type", "text/javascript");
    }
    else if (fileName.endsWith(".svg"))
    {
        response.setHeader("Content-Type", "image/svg+xml");
    }
    else if (fileName.endsWith(".woff"))
    {
        response.setHeader("Content-Type", "font/woff");
    }
    else if (fileName.endsWith(".woff2"))
    {
        response.setHeader("Content-Type", "font/woff2");
    }
    else if (fileName.endsWith(".ttf"))
    {
        response.setHeader("Content-Type", "application/x-font-ttf");
    }
    else if (fileName.endsWith(".eot"))
    {
        response.setHeader("Content-Type", "application/vnd.ms-fontobject");
    }
    else if (fileName.endsWith(".otf"))
    {
        response.setHeader("Content-Type", "application/font-otf");
    }
    // Todo: add all of your content types
    else
    {
        qWarning("StaticFileController: unknown MIME type for filename '%s'", qPrintable(fileName));
    }
}
