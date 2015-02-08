#ifndef FLOWRESTCONNECTION_H
#define FLOWRESTCONNECTION_H

#include <QObject>
#include <QWebSocket>
#include <QDateTime>

class FlowRestService;
class FlowNode;
class FlowRestConnection : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString uuid READ uuid WRITE setUuid NOTIFY uuidChanged)
    Q_PROPERTY(bool debug READ debug WRITE setDebug NOTIFY debugChanged)
    Q_PROPERTY(QDateTime whenConnected READ whenConnected WRITE setWhenConnected NOTIFY whenConnectedChanged)

    Q_PROPERTY(QStringList registeredEvents READ registeredEvents WRITE setRegisteredEvents NOTIFY registeredEventsChanged)
    Q_PROPERTY(bool authenticated READ authenticated WRITE setAuthenticated NOTIFY authenticatedChanged)
    Q_PROPERTY(QObject* user READ user WRITE setUser NOTIFY userChanged)
    Q_PROPERTY(QWebSocket* socket READ socket CONSTANT)
    Q_PROPERTY(QString description READ description CONSTANT)

public:
    explicit FlowRestConnection(QWebSocket* sock,QObject *parent = 0);
    ~FlowRestConnection();
    void sendMessageToClient(const QByteArray &message);
    void closeConnection();

signals:
    void messageFromClient(QString);
    void clientDisconnected();

private slots:
    void socketReadEvent(QString message);
    void socketErrorEvent();
    void socketDisconnectEvent();

    //-----------------------------------------------------------------------
public:
    bool debug() const
    {
        return m_debug;
    }

    bool authenticated() const
    {
        return m_authenticated;
    }


    QDateTime whenConnected() const
    {
        return m_whenConnected;
    }

    QString uuid() const
    {
        return m_uuid;
    }

    QObject* user() const
    {
        return m_user;
    }


    QWebSocket* socket() const
    {
        return m_socket;
    }

    QString description() const
    {
        return m_description;
    }

    QStringList registeredEvents() const
    {
        return m_registeredEvents;
    }

signals:
    void debugChanged(bool arg);

    void authenticatedChanged(bool arg);

    void whenConnectedChanged(QDateTime arg);

    void uuidChanged(QString arg);

    void userChanged(QObject* arg);

    void registeredEventsChanged(QStringList arg);

public slots:
    void setDebug(bool arg)
    {
        if (m_debug != arg) {
            m_debug = arg;
            emit debugChanged(arg);
        }
    }

    void setAuthenticated(bool arg)
    {
        if (m_authenticated != arg) {
            m_authenticated = arg;
            emit authenticatedChanged(arg);
        }
    }


    void setWhenConnected(QDateTime arg)
    {
        if (m_whenConnected != arg) {
            m_whenConnected = arg;
            emit whenConnectedChanged(arg);
        }
    }

    void setUuid(QString arg)
    {
        if (m_uuid != arg) {
            m_uuid = arg;
            emit uuidChanged(arg);
        }
    }

    void setUser(QObject* arg)
    {
        if (m_user != arg) {
            m_user = arg;
            emit userChanged(arg);
        }
    }

    void setRegisteredEvents(QStringList arg)
    {
        if (m_registeredEvents != arg) {
            m_registeredEvents = arg;
            emit registeredEventsChanged(arg);
        }
    }

private:
    bool m_debug;
    bool m_authenticated;
    QDateTime m_whenConnected;
    QString m_uuid;
    QObject* m_user;
    QWebSocket* m_socket;
    QString m_description;
    QStringList m_registeredEvents;
};


#endif // FLOWRESTCONNECTION_H
