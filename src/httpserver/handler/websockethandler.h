#ifndef WEBSOCKETHANDLER_H
#define WEBSOCKETHANDLER_H

#include <QObject>
#include <QWebSocketServer>
#include <QWebSocket>
#include <QUrlQuery>
#include <QLoggingCategory>

class webSocketHandler : public QObject
{
    Q_OBJECT
public:
    explicit webSocketHandler(QHostAddress host, quint16 port, bool debug = false, QObject *parent = nullptr);
    ~webSocketHandler();

    void pauseAccepting();
    void resumeAccepting();

    void listen();
    void close();

    bool isListening();

signals:
    void closed();

public slots:
    void onNewConnection();
    void processTextMessage(QString message);
    void processBinaryMessage(QByteArray message);
    void socketDisconnected();
    void sendMessage(QString message);

private:
    QWebSocketServer *m_pWebSocketServer;
    QList<QWebSocket *> m_clients;
    bool m_debug;

    QHostAddress m_host;
    quint16 m_port;
};

#endif // WEBSOCKETHANDLER_H
