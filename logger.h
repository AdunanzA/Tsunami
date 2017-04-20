#ifndef LOGGER_H
#define LOGGER_H

#include <QObject>

class logger : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(logger)

public:
    enum logLevel {
        TRACE,
        DEBUG,
        INFO,
        WARNING,
        ERROR,
        FATAL
    };
    Q_ENUM(logLevel) // enable meta-enum to easy convert enum (for ex: in QVariant)

    explicit logger(QObject *parent = 0);

    void setLevel(logLevel level);

    // properties
    //logLevel level() const;

signals:

public slots:

private:
    logLevel p_level;
};

#endif // LOGGER_H
