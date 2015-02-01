#ifndef FLOWCOMMUNICATIONUTILS_H
#define FLOWCOMMUNICATIONUTILS_H

#include <QtCore/QString>
#include <QVariant>
#include <QIODevice>


class FlowCommunicationUtils
{
public:
    static QVariant byteArrayToVariant(const QByteArray &data);
    static QByteArray variantToByteArray(const QVariant &variant);

    template<class T> static bool sendObject(QIODevice *socket, const T &obj,bool len4byte=true);
    static bool sendVariant(QIODevice *socket, const QVariant &variant,bool len4byte=true);
    static bool sendByteArray(QIODevice *socket, const QByteArray &data,bool len4byte=true);

    template<class T> static bool getObject(QIODevice *socket,T &obj,bool len4byte=true);
    static QVariant getVariant(QIODevice *socket,bool len4byte=true);
    static QByteArray getByteArray(QIODevice *socket,bool len4byte=true);

    static void setupServerSocketOptions(int desc);
    static void setKeepAlive(int sockfd,int idle_time=20,int probe_intvl=5,int probe_count=8);
};

template<class T>
bool FlowCommunicationUtils::sendObject(QIODevice *socket, const T &obj,bool len4byte)
{
    return FlowCommunicationUtils::sendVariant(socket,QVariant::fromValue<T>(obj),len4byte);
}

template<class T>
bool FlowCommunicationUtils::getObject(QIODevice *socket, T &obj,bool len4byte)
{
    QVariant recv_variant = FlowCommunicationUtils::getVariant(socket,len4byte);
    if(!recv_variant.isValid())
        return false;
    if(!recv_variant.canConvert<T>())
        return false;
    obj = recv_variant.value<T>();
    return true;
}

#endif // FLOWCOMMUNICATIONUTILS_H
