#include "websockethandler.h"

webSocketHandler::webSocketHandler(QHostAddress host, quint16 port, bool debug, QObject *parent) :
    QObject(parent),
    m_pWebSocketServer(new QWebSocketServer(QStringLiteral("Tsunami Web Server"),
                                            QWebSocketServer::NonSecureMode, this)),
    m_debug(debug)
{
    QLoggingCategory::setFilterRules("qt.network.ssl=false"); // ignore warning error for missing openssl
    m_host = host;
    m_port = port;

//    if (m_pWebSocketServer->listen(QHostAddress::Any, port)) {
    if (m_pWebSocketServer->listen(m_host, m_port)) {
        if (m_debug) qDebug() << "webSocketHandler Listening on port" << port;
        connect(m_pWebSocketServer, &QWebSocketServer::newConnection, this, &webSocketHandler::onNewConnection);
        connect(m_pWebSocketServer, &QWebSocketServer::closed, this, &webSocketHandler::closed);
    }
}

webSocketHandler::~webSocketHandler()
{
    m_pWebSocketServer->close();
    qDeleteAll(m_clients.begin(), m_clients.end());
    qDebug("webSocketHandler: destroyed");
}

void webSocketHandler::pauseAccepting()
{
    m_pWebSocketServer->pauseAccepting();
}

void webSocketHandler::resumeAccepting()
{
    m_pWebSocketServer->resumeAccepting();
}

void webSocketHandler::listen()
{
    m_pWebSocketServer->listen(m_host, m_port);
}

void webSocketHandler::close()
{
    m_pWebSocketServer->close();
    qDeleteAll(m_clients.begin(), m_clients.end());
}

bool webSocketHandler::isListening()
{
    return (m_pWebSocketServer != NULL && m_pWebSocketServer->isListening());
}

void webSocketHandler::onNewConnection()
{
    QWebSocket *pSocket = m_pWebSocketServer->nextPendingConnection();
//    QUrlQuery urlQuery(pSocket->requestUrl().query());

//    if (!urlQuery.hasQueryItem("id")) {
//        connect(pSocket, &QWebSocket::disconnected, this, &webSocketHandler::socketDisconnected);
//        pSocket->disconnect(pSocket);
//    } else {
//        QString id = urlQuery.queryItemValue("id");

//        connect(pSocket, &QWebSocket::textMessageReceived, this, &webSocketHandler::processTextMessage);
//        connect(pSocket, &QWebSocket::binaryMessageReceived, this, &webSocketHandler::processBinaryMessage);
//        connect(pSocket, &QWebSocket::disconnected, this, &webSocketHandler::socketDisconnected);
//        pSocket->setProperty("id", id);

//        m_clients << pSocket;

//        if (m_debug) qDebug() << "socket" << m_clients.count() << "connected:" << pSocket << "with id" << id;
//        //pSocket->sendTextMessage(QString("{\"action\":\"id\",\"value\":\"%0\"}").arg(index));
//    }

    connect(pSocket, &QWebSocket::textMessageReceived, this, &webSocketHandler::processTextMessage);
    connect(pSocket, &QWebSocket::binaryMessageReceived, this, &webSocketHandler::processBinaryMessage);
    connect(pSocket, &QWebSocket::disconnected, this, &webSocketHandler::socketDisconnected);
    m_clients << pSocket;

    if (m_debug) qDebug() << "socket" << m_clients.count() << "connected:" << pSocket;
}

void webSocketHandler::processTextMessage(QString message)
{
//    QWebSocket *pClient = qobject_cast<QWebSocket *>(sender());
    if (m_debug)
        qDebug() << "Message received:" << message;
//    if (pClient) {
//        pClient->sendTextMessage(message);
//    }
}

void webSocketHandler::processBinaryMessage(QByteArray message)
{
//    QWebSocket *pClient = qobject_cast<QWebSocket *>(sender());
    if (m_debug)
        qDebug() << "Binary Message received:" << message;
//    if (pClient) {
//        pClient->sendBinaryMessage(message);
//    }
}

void webSocketHandler::socketDisconnected()
{
    QWebSocket *pClient = qobject_cast<QWebSocket *>(sender());
    if (m_debug)
        qDebug() << "socket disconnected:" << pClient;
    if (pClient) {
        m_clients.removeAll(pClient);
        pClient->deleteLater();
    }
}

void webSocketHandler::sendMessage(QString message)
{
//    qDebug() << "send message to" << id;
//    for (int i = 0; i < m_clients.count(); i++) {
//        qDebug() << "socket" << i << "of" << m_clients.count() << "with id " << m_clients[i]->property("id");
//        if (m_clients[i]->isValid() && m_clients[i]->property("id") != NULL && m_clients[i]->property("id").toString() == id) {
//            m_clients[i]->sendTextMessage(message);
//            break;
//        }
//    }

    QList<QHostAddress> addressesAlreadyMessaged;
    for (int i = 0; i < m_clients.count(); i++) {
        if (m_clients[i]->isValid() && !addressesAlreadyMessaged.contains(m_clients[i]->localAddress())) {
            m_clients[i]->sendTextMessage(message);
            addressesAlreadyMessaged.append(m_clients[i]->localAddress());
        }
    }
}
