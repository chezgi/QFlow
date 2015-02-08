#include "FlowRestConnection.h"
#include <QUuid>

FlowRestConnection::FlowRestConnection(QWebSocket *sock, QObject *parent) :
    QObject(parent)
{
    m_uuid = QUuid::createUuid().toString();
    m_whenConnected = QDateTime::currentDateTime();
    m_authenticated = false;
    m_user = nullptr;
    m_socket = sock;    
    m_description = QString("%1:%2").arg(sock->peerAddress().toString()).arg(sock->peerPort());
    connect(m_socket,SIGNAL(textMessageReceived(QString)),SLOT(socketReadEvent(QString)));
    connect(m_socket,SIGNAL(error(QAbstractSocket::SocketError)),SLOT(socketErrorEvent()));
    connect(m_socket,SIGNAL(disconnected()),SLOT(socketDisconnectEvent()));

}

FlowRestConnection::~FlowRestConnection()
{
    if(m_socket)
    {
        if(m_socket->state() == QAbstractSocket::ConnectedState)
            m_socket->close();
        m_socket->deleteLater();
    }
}

void FlowRestConnection::sendMessageToClient(const QByteArray &message)
{
    if(socket()->state() != QAbstractSocket::ConnectedState)
        return;
    socket()->sendTextMessage(message);
}

void FlowRestConnection::closeConnection()
{
    m_socket->close();
}

void FlowRestConnection::socketReadEvent(QString message)
{
    emit messageFromClient(message);
}

void FlowRestConnection::socketErrorEvent()
{
    qDebug() << "[FlowConnection]: Socket Error:" << description() << socket()->errorString();
}

void FlowRestConnection::socketDisconnectEvent()
{
    emit clientDisconnected();
}
