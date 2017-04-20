#include "preferences.h"
#include <QFileInfo>

#define PREFERENCES_FILENAME "conf"
#define DEFAULT_PATH_DOWNLOAD "download"

preferences::preferences(QObject *parent) : QObject(parent)
{
    loadFromFile();
}

void preferences::loadFromFile()
{
    if (!fileExists(PREFERENCES_FILENAME)) {
        set_logLevel(logger::logLevel::INFO);
        set_pathDownload(DEFAULT_PATH_DOWNLOAD);
        writeToFile();
    } else {
        // READ FROM FILE
    }
}

bool preferences::fileExists(QString path) { // http://stackoverflow.com/a/26991243
    QFileInfo check_file(path);
    return check_file.exists() && check_file.isFile();
}

void preferences::writeToFile()
{
    // WRITE TO FILE
}

QString preferences::get_pathDownload() const
{
    return p_pathDownload;
}

void preferences::set_pathDownload(const QString &value)
{
    p_pathDownload = value;
}

logger::logLevel preferences::get_logLevel() const
{
    return p_logLevel;
}

void preferences::set_logLevel(const logger::logLevel &value)
{
    p_logLevel = value;
}
