#ifndef PREFERENCES_H
#define PREFERENCES_H

#include <QObject>

#include "logger.h"

class preferences : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(preferences)
public:
    explicit preferences(QObject *parent = 0);
    void loadFromFile();
    void writeToFile();

    QString get_pathDownload() const;
    void set_pathDownload(const QString &value);

    logger::logLevel get_logLevel() const;
    void set_logLevel(const logger::logLevel &value);

signals:

public slots:

private:
    QString p_pathDownload;
    logger::logLevel p_logLevel;
    bool fileExists(QString path);
};

#endif // PREFERENCES_H
