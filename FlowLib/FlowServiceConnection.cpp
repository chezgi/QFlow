#include "FlowServiceConnection.h"
#include <QUuid>

FlowServiceConnection::FlowServiceConnection(QWebSocket *sock, QObject *parent) :
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

FlowServiceConnection::~FlowServiceConnection()
{
    if(m_socket)
    {
        if(m_socket->state() == QAbstractSocket::ConnectedState)
            m_socket->close();
        m_socket->deleteLater();
    }
}

void FlowServiceConnection::sendMessageToClient(const QByteArray &message)
{
    if(socket()->state() != QAbstractSocket::ConnectedState)
        return;
    socket()->sendTextMessage(message);
}

void FlowServiceConnection::closeConnection()
{
    m_socket->close();
}

void FlowServiceConnection::socketReadEvent(QString message)
{
    emit messageFromClient(message);
}

void FlowServiceConnection::socketErrorEvent()
{
    qDebug() << "[FlowConnection]: Socket Error:" << description() << socket()->errorString();
}

void FlowServiceConnection::socketDisconnectEvent()
{
    emit clientDisconnected();
}
