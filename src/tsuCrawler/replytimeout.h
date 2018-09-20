#ifndef REPLYTIMEOUT_H
#define REPLYTIMEOUT_H

#include <QNetworkReply>
#include <QTimerEvent>
#include <QBasicTimer>

class ReplyTimeout : public QObject {
    Q_OBJECT
    QBasicTimer m_timer;
    int m_timeout;
public:
    ReplyTimeout(QNetworkReply* reply, const int timeout) : QObject(reply) {
        Q_ASSERT(reply);
        if (reply && reply->isRunning()) {
            m_timeout = timeout;
            connect(reply, &QNetworkReply::downloadProgress, this, &ReplyTimeout::downloadProgress);
            m_timer.start(m_timeout, this);
        }
    }

public slots:
    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal) {
        Q_UNUSED(bytesReceived);
        Q_UNUSED(bytesTotal);
        m_timer.start(m_timeout, this); // on download progress, reset timeout
    }

    static void set(QNetworkReply* reply, const int timeout) {
        new ReplyTimeout(reply, timeout);
    }
protected:

    void timerEvent(QTimerEvent * ev) {
        if (!m_timer.isActive() || ev->timerId() != m_timer.timerId())
            return;
        auto reply = static_cast<QNetworkReply*>(parent());
        if (reply->isRunning())
            reply->abort();
        m_timer.stop();
    }
};

#endif // REPLYTIMEOUT_H
