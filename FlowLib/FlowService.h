#ifndef FLOWNODESERVICE_H
#define FLOWNODESERVICE_H

#include <QObject>
//#include <QTcpSocket>
#include <QList>
#include <QWebSocketServer>
#include <QWebSocket>
//#include <QTcpServer>
#include <QTimer>

#include "FlowServiceConnection.h"

/*
   json format:
   "servicename":{}
   "servicename":{}
 */
class FlowNode;
class FlowService : public QObject
{
    friend class FlowNodeServiceInfo;
    Q_OBJECT
    Q_PROPERTY(int listenPort READ listenPort WRITE setListenPort NOTIFY listenPortChanged)
    Q_PROPERTY(bool debug READ debug WRITE setDebug NOTIFY debugChanged)
    Q_PROPERTY(int authenticatedCount READ authenticatedCount WRITE setAuthenticatedCount NOTIFY authenticatedCountChanged)

public:
    explicit FlowService(QObject *parent = 0);
    Q_INVOKABLE void start();
    Q_INVOKABLE void stop();

    QList<FlowServiceConnection *> allConnections();


private:
    void registerServiceProvider(FlowNode *node, QString serviceName, QString serviceApi, bool annonymousAllowed = false);
    void unregisterServiceProvider(QString serviceName);
    FlowServiceConnection *getConnection(QString clientId);

    void handleInternalService(QString clientId,QString serviceName,QVariantMap requestData);

    void broadcast(QString serviceName,QVariantMap data);
    void sendTo(QString clientId,QString serviceName,QVariantMap data);
    void sendErrorTo(QString clientId, QString errorMessage, QString serviceName , QVariant request);
    bool hasRegisteredClient(QString serviceName);

signals:
    void connectionAdded(QString clientId);
    void connectionRemoved(QString clientId);

private slots:
    void newConnectionEvent();
    void connectionMessageReceivedEvent(QString message);
    void connectinDisconnectedEvent();
    void connectionAuthenticationChanged();
    void connectionRegisteredEventsChanged();

private:
    QWebSocketServer *m_serverSocket;
    QMap<QString,FlowServiceConnection*> connectionMap;
    QMap<QString,FlowNode *> serviceProvicerMap;
    QVariantMap serviceApiMap;
    QStringList anonServices;
    QStringList internalServices;

    QMap<QString,QStringList> registeredMap;  // service,clientIdList

    //--------------------------------------------------------- Q_PROPERTY
public:
    int listenPort() const;

    bool debug() const;

    int authenticatedCount() const
    {
        return m_authenticatedCount;
    }

signals:
    void listenPortChanged(int arg);

    void debugChanged(bool arg);

    void authenticatedCountChanged(int arg);

public slots:
    void setListenPort(int arg);

    void setDebug(bool arg);

    void setAuthenticatedCount(int arg)
    {
        if (m_authenticatedCount != arg) {
            m_authenticatedCount = arg;
            emit authenticatedCountChanged(arg);
        }
    }

private:
    int m_listenPort;
    bool m_debug;
    int m_authenticatedCount;
};

#endif // FLOWNODESERVICE_H
