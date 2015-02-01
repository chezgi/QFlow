#include "FlowService.h"
#include "FlowNode.h"
//#include "FlowCommunicationUtils.h"
#include <QJsonDocument>
#include <QJsonObject>


FlowService::FlowService(QObject *parent) :
    QObject(parent),
    m_serverSocket(new QWebSocketServer("FlowService",QWebSocketServer::NonSecureMode,this))
{
    m_listenPort = 8888;
    m_debug = false;
    m_authenticatedCount = 0;

    serviceApiMap["api"] = "c2s: { api:{} }  s2c:{ api:{} }";
    serviceApiMap["ping"] = "c2s: { ping:{} }  s2c:{ pong:{} }";
//    serviceApiMap["events"] = "c2s: { events:{register:[]} }  s2c:{events:{registered:true}}";

    internalServices << "api" << "ping";

    connect(m_serverSocket,&QWebSocketServer::newConnection,this,&FlowService::newConnectionEvent);
}

void FlowService::start()
{
    if(listenPort() > 0)
    {
        if(!m_serverSocket->listen(QHostAddress::Any,listenPort()))
        {
            qDebug() << "[FlowService] Start Error:" << m_serverSocket->errorString() << listenPort();
        }else
        {
            if(debug())
                qDebug() << "[FlowService] Started:" << listenPort();
        }
    }
    else
        qDebug() << "[FlowService]: Listen port Invalid:" << listenPort();
}

void FlowService::stop()
{
    m_serverSocket->close();
    // close all connections
    QMap<QString,FlowServiceConnection*> tmpConnectionMap = connectionMap;
    connectionMap.clear();
    for(FlowServiceConnection *connection:tmpConnectionMap)
    {
        connection->deleteLater();
    }
    tmpConnectionMap.clear();
}

void FlowService::registerServiceProvider(FlowNode *node, QString serviceName,QString serviceApi,bool annonymousAllowed)
{
    if(serviceProvicerMap.contains(serviceName))
        qDebug() << "Service Provider Conflict for service:"<< serviceName;
    if(debug())
        qDebug() << "[FlowService] Service Registered: " << serviceName;
    serviceApiMap[serviceName] = serviceApi;
    serviceProvicerMap[serviceName] = node;
    if(annonymousAllowed)
        anonServices << serviceName;


}

void FlowService::unregisterServiceProvider(QString serviceName)
{
    if(debug())
        qDebug() << "[FlowService] Service UnRegistered: " << serviceName;

    serviceProvicerMap.remove(serviceName);
    anonServices.removeAll(serviceName);
    serviceApiMap.remove(serviceName);
}

FlowServiceConnection *FlowService::getConnection(QString clientId)
{
    return connectionMap.value(clientId,nullptr);
}

QList<FlowServiceConnection *> FlowService::allConnections()
{
    return connectionMap.values();
}


void FlowService::handleInternalService(QString clientId,QString serviceName,QVariantMap requestData)
{
    Q_UNUSED(requestData)
    if(serviceName == "api")
    {
        sendTo(clientId,serviceName,serviceApiMap);
        return;
    }
    if(serviceName == "ping")
    {
        QVariantMap resultMap;
        resultMap["pong"] = QVariantMap();
        sendTo(clientId,"ping",resultMap);
        return;
    }

//    if(serviceName == "events")
//    {
//        FlowServiceConnection *connection = getConnection(clientId);
//        if(!connection)
//            return;
//        if(requestData.contains("register"))
//        {
//            QStringList regServices = requestData["register"].toStringList();
//            registerClientForEvents(clientId,regServices);
//            QVariantMap resultData;
//            resultData["registered"]= true;
//            sendTo(clientId,serviceName,resultData);
//        }
//    }
}

void FlowService::broadcast(QString serviceName, QVariantMap data)
{
    QVariantMap sendMap;
    sendMap[serviceName] = data;

    if(debug())
        qDebug() << "[FlowService] sending to all: " << serviceName;

    QJsonDocument jsonDoc = QJsonDocument::fromVariant(sendMap);
    QByteArray msg = jsonDoc.toJson();
    for(FlowServiceConnection *connection:connectionMap.values())
    {
        if(!connection->authenticated())
            continue;
        if(connection->registeredEvents().contains(serviceName))
            connection->sendMessageToClient(msg);
    }
}

void FlowService::sendTo(QString clientId, QString serviceName, QVariantMap data)
{
    if(debug())
        qDebug() << "[FlowService] sending to one: " << serviceName;

    QVariantMap sendMap;
    sendMap[serviceName] = data;
    FlowServiceConnection *connection = getConnection(clientId);
    if(!connection)
        return;
    QJsonDocument jsonDoc = QJsonDocument::fromVariant(sendMap);
    connection->sendMessageToClient(jsonDoc.toJson());
}

void FlowService::sendErrorTo(QString clientId, QString errorMessage,QString serviceName,QVariant requestData)
{
    QVariantMap sendMap;
    QVariantMap errorMap;

    if(debug())
        qDebug() << "[FlowService] sending error: " << serviceName << " Error:"<<errorMessage;

    errorMap["errorString"] = errorMessage;
    errorMap["service"] = serviceName;
    errorMap["request"] = requestData;
    sendMap["error"] = errorMap;


    FlowServiceConnection *connection = getConnection(clientId);
    if(!connection)
        return;
    QJsonDocument jsonDoc = QJsonDocument::fromVariant(sendMap);
    connection->sendMessageToClient(jsonDoc.toJson());
}

bool FlowService::hasRegisteredClient(QString serviceName)
{
    if(registeredMap.contains(serviceName) && !registeredMap[serviceName].isEmpty())
        return true;
    return false;
}

void FlowService::newConnectionEvent()
{
    QWebSocket *socket = m_serverSocket->nextPendingConnection();
    if(!socket)
        return;

    if(debug())
        qDebug() << "[FlowService] New Connection";

    FlowServiceConnection *connection = new FlowServiceConnection(socket,this);
    connect(connection,SIGNAL(messageFromClient(QString)),SLOT(connectionMessageReceivedEvent(QString)));
    connect(connection,SIGNAL(clientDisconnected()),SLOT(connectinDisconnectedEvent()));
    connect(connection,SIGNAL(authenticatedChanged(bool)),SLOT(connectionAuthenticationChanged()));
    connect(connection,SIGNAL(registeredEventsChanged(QStringList)),SLOT(connectionRegisteredEventsChanged()));
    connectionMap[connection->uuid()] = connection;

    emit connectionAdded(connection->uuid());
}

void FlowService::connectionMessageReceivedEvent(QString message)
{
    FlowServiceConnection *connection = qobject_cast<FlowServiceConnection*>(sender());
    if(!connection)
        return;
    if(debug())
        qDebug() << "[FlowService]: Connection Read[" << connection->description()  << "] " << message;

    QJsonParseError jsonError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(message.toLatin1(),&jsonError);
    if(jsonError.error != QJsonParseError::NoError)
    {
        qDebug() << "[FlowService]:Error Accoured in  FlowService Received Message Parsing:" << jsonError.errorString() << " At Offset:" << jsonError.offset;
        qDebug() << "  Received Message is:" << message;
        sendErrorTo(connection->uuid(),"Message Parsing Error:" +jsonError.errorString(),"",message);
        return;
    }
    if(!jsonDoc.isObject() || jsonDoc.isEmpty())
    {
        qDebug() << "[FlowService]: Received Message is not object or empty";
        qDebug() << "  Received Message is:" << message;
        sendErrorTo(connection->uuid(),"FlowService Received Message is not object or empty","",message);
        return;
    }
    bool showReceivedMessage = false;
    for( QString serviceName: jsonDoc.object().keys())
    {
        if(!jsonDoc.object()[serviceName].isObject())
        {
            qDebug() << "[FlowService]: Received Message service format error for service:" << serviceName;
            sendErrorTo(connection->uuid(),"Message Must Be Object",serviceName,message);
            showReceivedMessage = true;
            continue;
        }
        QJsonObject serviceObject = jsonDoc.object()[serviceName].toObject();

        if(connection->authenticated() && internalServices.contains(serviceName))
        {
            handleInternalService(connection->uuid(),serviceName,serviceObject.toVariantMap());
            continue;
        }

        if(serviceProvicerMap.contains(serviceName))
        {
            if(connection->authenticated() || anonServices.contains(serviceName))
            {
                serviceProvicerMap[serviceName]->_fromServiceCenter(connection->uuid(),serviceObject.toVariantMap());
            }else
            {
                sendErrorTo(connection->uuid(),"Not Authenticated you must logn first for service:" + serviceName,serviceName,message);
                continue;
            }
        }else
        {
            qDebug() << "[FlowService]: Data From Connection Without Any ServiceProvider:" << serviceName;
            sendErrorTo(connection->uuid(),"No Service Provider",serviceName,message);
            showReceivedMessage = true;
        }
    }
    if(showReceivedMessage)
    {
        qDebug() << "-----------------------------------------";
        qDebug() << "Received Message is:" ;
        qDebug() << message;
        qDebug() << "-----------------------------------------";
    }

}


void FlowService::connectinDisconnectedEvent()
{
    FlowServiceConnection *connection = qobject_cast<FlowServiceConnection*>(sender());;
    if(!connection)
        return;
    if(debug())
        qDebug() << "[FlowService] Connection Closed:" << connection->description();

    connection->disconnect(this);
    if(connection->authenticated())
    {
        setAuthenticatedCount(authenticatedCount() -1 );
        connection->setRegisteredEvents(QStringList());
    }

    connectionMap.remove(connection->uuid());
    emit connectionRemoved(connection->uuid());
    connection->deleteLater();
}

void FlowService::connectionAuthenticationChanged()
{
    FlowServiceConnection *connection = qobject_cast<FlowServiceConnection*>(sender());
    if(connection->authenticated())
    {
        setAuthenticatedCount(authenticatedCount() + 1);
    }else
    {
        connection->setRegisteredEvents(QStringList());
        setAuthenticatedCount(authenticatedCount() - 1);
    }
}

void FlowService::connectionRegisteredEventsChanged()
{
    FlowServiceConnection *connection = qobject_cast<FlowServiceConnection*>(sender());

    // remove old registered events
    for(QString regService:registeredMap.keys())
    {
        if(connection->registeredEvents().contains(regService))
            continue;
        if(registeredMap[regService].contains(connection->uuid()))
            registeredMap[regService].removeAll(connection->uuid());
    }

    // FIXME: check servicenames
    for(QString regService:connection->registeredEvents())
    {
        if(!registeredMap.contains(regService))
            registeredMap[regService] = QStringList();
        if(!registeredMap[regService].contains(connection->uuid()))
            registeredMap[regService].append(connection->uuid());
    }
}

//--------------------------------------------------------- Q_PROPERTY
int FlowService::listenPort() const
{
    return m_listenPort;
}

bool FlowService::debug() const
{
    return m_debug;
}

void FlowService::setListenPort(int arg)
{
    if (m_listenPort != arg) {
        m_listenPort = arg;
        emit listenPortChanged(arg);
    }
}

void FlowService::setDebug(bool arg)
{
    if (m_debug != arg) {
        m_debug = arg;
        emit debugChanged(arg);
    }
}
