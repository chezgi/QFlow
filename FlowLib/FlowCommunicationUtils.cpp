#include "FlowCommunicationUtils.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>

QVariant FlowCommunicationUtils::byteArrayToVariant(const QByteArray &data)
{
    QVariant variant;
    QDataStream recvstram(data);
    recvstram >> variant;
    return variant;
}

QByteArray FlowCommunicationUtils::variantToByteArray(const QVariant &variant)
{
    QByteArray result;
    QDataStream stream(&result,QIODevice::WriteOnly);
    stream << variant;
    return result;
}

bool FlowCommunicationUtils::sendVariant(QIODevice *socket, const QVariant &variant, bool len4byte)
{
    QByteArray sendbuf;
    QDataStream sendstream(&sendbuf,QIODevice::WriteOnly);
    sendstream << variant;
    return sendByteArray(socket,sendbuf,len4byte);

//    quint32 len;
//    QByteArray sendbuf;
//    QDataStream sendstream(&sendbuf,QIODevice::WriteOnly);
//    sendstream << variant;
//    len = sendbuf.length();
//    sendbuf.prepend((char *)&len,4);
//    qint64 sent = socket->write(sendbuf);
//    if(sent != sendbuf.length())
//        return false;
//    return true;
}

QVariant FlowCommunicationUtils::getVariant(QIODevice *socket,bool len4byte)
{
    QByteArray recvbuf = getByteArray(socket,len4byte);
    if(recvbuf.isEmpty())
        return QVariant();
    return byteArrayToVariant(recvbuf);
}

QByteArray FlowCommunicationUtils::getByteArray(QIODevice *socket,bool len4byte)
{
    if(len4byte)
    {
        // 4byte mode
        if(socket->bytesAvailable()<4)
            return QByteArray();
        quint32 len;
        socket->peek((char *)&len,4);
        if(socket->bytesAvailable()<4+len)
            return QByteArray();
        QByteArray recvbuf = socket->read(len+4);
        recvbuf = recvbuf.mid(4); //remove length part.
        return recvbuf;
    }else
    {
        // 2byte mode
        if(socket->bytesAvailable()<2)
            return QByteArray();
        quint16 len;
        socket->peek((char *)&len,2);
        if(socket->bytesAvailable()<2+len)
            return QByteArray();
        QByteArray recvbuf = socket->read(len+2);
        recvbuf = recvbuf.mid(2); //remove length part.
        return recvbuf;
    }
}

void FlowCommunicationUtils::setupServerSocketOptions(int desc)
{
    int opt=1;
    setsockopt(desc,SOL_SOCKET,SO_KEEPALIVE,&opt,sizeof(opt) );
    opt=1;
    setsockopt(desc,IPPROTO_TCP,TCP_NODELAY,&opt,sizeof(opt) );
    opt=1;
    setsockopt(desc, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
}

void FlowCommunicationUtils::setKeepAlive(int sockfd, int idle_time, int probe_intvl, int probe_count)
{
    int opt = 1;
    setsockopt(sockfd,IPPROTO_TCP,TCP_NODELAY,&opt,sizeof(opt) );
//    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    opt=1;
    setsockopt(sockfd,SOL_SOCKET,SO_KEEPALIVE,&opt,sizeof(opt) );
    setsockopt(sockfd,IPPROTO_TCP,TCP_KEEPIDLE,&idle_time,sizeof(idle_time));
    setsockopt(sockfd,IPPROTO_TCP,TCP_KEEPINTVL,&probe_intvl,sizeof(probe_intvl));
    setsockopt(sockfd,IPPROTO_TCP,TCP_KEEPCNT,&probe_count,sizeof(probe_count));
//    opt=1;
//    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
}

bool FlowCommunicationUtils::sendByteArray(QIODevice *socket, const QByteArray &data, bool len4byte)
{
    QByteArray sendbuf = data;
    if(len4byte)
    {
        // 4byte mode
        quint32 len;
        len = data.length();
        sendbuf.prepend((char *)&len,4);
    }else
    {
        // 2byte mode
        quint16 len;
        len = data.length();
        sendbuf.prepend((char *)&len,2);
    }
    return socket->write(sendbuf) == sendbuf.length();
}
