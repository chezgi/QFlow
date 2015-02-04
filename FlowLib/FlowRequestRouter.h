#ifndef FLOWREQUESTMAPPER_H
#define FLOWREQUESTMAPPER_H

#include <QtCore/QObject>
#include <QtCore/QMap>
#include <QtCore/QDateTime>
#include <QtCore/QTimer>
#include <QVariantMap>

class FlowRequestRouter : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(bool debug READ debug WRITE setDebug NOTIFY debugChanged)
    Q_PROPERTY(int ttl READ ttl WRITE setTtl NOTIFY ttlChanged)
    struct RequestInfo
    {
        RequestInfo(){
            count = 0;
            outPortId = 0;
            updateTime = QDateTime::currentDateTime();
        }

        QString flowId;
        QDateTime updateTime;
        int count;
        int outPortId;
        QObject* requester;
    };

public:
    explicit FlowRequestRouter(QObject *parent = 0);
    ~FlowRequestRouter();

    Q_INVOKABLE void add(QVariantMap flowPacket,QObject *object);
    Q_INVOKABLE bool contains(QVariantMap flowPacket);
    Q_INVOKABLE QObject *get(QVariantMap flowPacket);
    Q_INVOKABLE QObject *take(QVariantMap flowPacket);
    Q_INVOKABLE void clear();

    // used for node
    Q_INVOKABLE void setCount(QVariantMap flowPacket,int count);
    Q_INVOKABLE int getCount(QVariantMap flowPacket);

    Q_INVOKABLE void setOutPortId(QVariantMap flowPacket,int port);
    Q_INVOKABLE int getOutPortId(QVariantMap flowPacket);

    //void update(QString sessionid);

private slots:
    void doCleaup();
    void requestPortObjectDeleted();

private:
    QMap<QString,RequestInfo *> m_requestMap;
    QTimer *cleanupTimer;

//----------------------------------------------- properties
public:
    int ttl() const
    {
        return m_ttl;
    }

    QString name() const
    {
        return m_name;
    }

    bool debug() const
    {
        return m_debug;
    }

signals:
    void ttlChanged(int arg);

    void nameChanged(QString arg);

    void debugChanged(bool arg);

public slots:
    void setTtl(int arg)
    {
        if (m_ttl != arg) {
            m_ttl = arg;
            cleanupTimer->setInterval( qMax(m_ttl/2,1) * 1000);
            emit ttlChanged(arg);
        }
    }

    void setName(QString arg)
    {
        if (m_name != arg) {
            m_name = arg;
            emit nameChanged(arg);
        }
    }

    void setDebug(bool arg)
    {
        if (m_debug != arg) {
            m_debug = arg;
            emit debugChanged(arg);
        }
    }

private:
    int m_ttl;

    QString m_name;
    bool m_debug;
};
#endif // FLOWREQUESTMAPPER_H
