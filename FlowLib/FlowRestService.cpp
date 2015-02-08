#include "FlowRestService.h"
#include "FlowNode.h"
//#include "FlowCommunicationUtils.h"
#include <QJsonDocument>
#include <QJsonObject>


FlowRestService::FlowRestService(QObject *parent) :
    QObject(parent),
    m_serverSocket(new QWebSocketServer("FlowRestService",QWebSocketServer::NonSecureMode,this))
{
    m_listenPort = 8888;
    m_debug = false;
    m_authenticatedCount = 0;

    serviceApiMap["api"] = "c2s: { api:{} }  s2c:{ api:{} }";
//    serviceApiMap["ping"] = "c2s: { ping:{} }  s2c:{ pong:{} }";
//    serviceApiMap["topology"] = "c2s:{topology:{} s2c:{topology:{} }";

//    serviceApiMap["events"] = "c2s: { events:{register:[]} }  s2c:{events:{registered:true}}";

    internalServices << "api";

    connect(m_serverSocket,&QWebSocketServer::newConnection,this,&FlowRestService::newConnectionEvent);
}

void FlowRestService::start()
{
    if(listenPort() > 0)
    {
        if(!m_serverSocket->listen(QHostAddress::Any,listenPort()))
        {
            qDebug() << "[FlowRestService] Start Error:" << m_serverSocket->errorString() << listenPort();
        }else
        {
            if(debug())
                qDebug() << "[FlowRestService] Started:" << listenPort();
        }
    }
    else
        qDebug() << "[FlowRestService]: Listen port Invalid:" << listenPort();
}

void FlowRestService::stop()
{
    m_serverSocket->close();
    // close all connections
    QMap<QString,FlowRestConnection*> tmpConnectionMap = connectionMap;
    connectionMap.clear();
    for(FlowRestConnection *connection:tmpConnectionMap)
    {
        connection->deleteLater();
    }
    tmpConnectionMap.clear();
}

void FlowRestService::registerServiceProvider(FlowNode *node, QString serviceName,QString serviceApi,bool annonymousAllowed)
{
    if(serviceProvicerMap.contains(serviceName))
        qDebug() << "Service Provider Conflict for service:"<< serviceName;
    if(debug())
        qDebug() << "[FlowRestService] Service Registered: " << serviceName;
    serviceApiMap[serviceName] = serviceApi;
    serviceProvicerMap[serviceName] = node;
    if(annonymousAllowed)
        anonServices << serviceName;


}

void FlowRestService::unregisterServiceProvider(QString serviceName)
{
    if(debug())
        qDebug() << "[FlowRestService] Service UnRegistered: " << serviceName;

    serviceProvicerMap.remove(serviceName);
    anonServices.removeAll(serviceName);
    serviceApiMap.remove(serviceName);
}

FlowRestConnection *FlowRestService::getConnection(QString clientId)
{
    return connectionMap.value(clientId,nullptr);
}

QList<FlowRestConnection *> FlowRestService::allConnections()
{
    return connectionMap.values();
}


void FlowRestService::handleInternalService(QString clientId,QString serviceName,QVariantMap requestData)
{
    Q_UNUSED(requestData)
    if(serviceName == "api")
    {
        sendTo(clientId,serviceName,serviceApiMap);
        return;
    }
//    if(serviceName == "ping")
//    {
//        QVariantMap resultMap;
//        resultMap["pong"] = QVariantMap();
//        sendTo(clientId,"ping",resultMap);
//        return;
//    }
//    if(serviceName == "topology")
//    {
//        FlowGraph *graph=qobject_cast<FlowGraph*>(parent());
//        if(!graph)
//            return;
//        QVariantMap sendMap;
//        sendMap[command] = graph->topologyMap();
//        sendTo(clientId,serviceName,sendMap);
//        return;
//    }

//    if(serviceName == "events")
//    {
//        FlowRestServiceConnection *connection = getConnection(clientId);
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

void FlowRestService::broadcast(QString serviceName, QVariantMap data)
{
    QVariantMap sendMap;
    sendMap[serviceName] = data;

    if(debug())
        qDebug() << "[FlowRestService] sending to all: " << serviceName;

    QJsonDocument jsonDoc = QJsonDocument::fromVariant(sendMap);
    QByteArray msg = jsonDoc.toJson();
    for(FlowRestConnection *connection:connectionMap.values())
    {
        if(!connection->authenticated())
            continue;
        if(connection->registeredEvents().contains(serviceName))
            connection->sendMessageToClient(msg);
    }
}

void FlowRestService::sendTo(QString clientId, QString serviceName, QVariantMap data)
{
    if(debug())
        qDebug() << "[FlowRestService] sending to one: " << serviceName;

    QVariantMap sendMap;
    sendMap[serviceName] = data;
    FlowRestConnection *connection = getConnection(clientId);
    if(!connection)
        return;
    QJsonDocument jsonDoc = QJsonDocument::fromVariant(sendMap);
    connection->sendMessageToClient(jsonDoc.toJson());
}

void FlowRestService::sendErrorTo(QString clientId, QString errorMessage,QString serviceName,QVariant requestData)
{
    QVariantMap sendMap;
    QVariantMap errorMap;

    if(debug())
        qDebug() << "[FlowRestService] sending error: " << serviceName << " Error:"<<errorMessage;

    errorMap["errorString"] = errorMessage;
    errorMap["service"] = serviceName;
    errorMap["request"] = requestData;
    sendMap["error"] = errorMap;


    FlowRestConnection *connection = getConnection(clientId);
    if(!connection)
        return;
    QJsonDocument jsonDoc = QJsonDocument::fromVariant(sendMap);
    connection->sendMessageToClient(jsonDoc.toJson());
}

bool FlowRestService::hasRegisteredClient(QString serviceName)
{
    if(registeredMap.contains(serviceName) && !registeredMap[serviceName].isEmpty())
        return true;
    return false;
}

void FlowRestService::newConnectionEvent()
{
    QWebSocket *socket = m_serverSocket->nextPendingConnection();
    if(!socket)
        return;

    if(debug())
        qDebug() << "[FlowRestService] New Connection";

    FlowRestConnection *connection = new FlowRestConnection(socket,this);
    connect(connection,SIGNAL(messageFromClient(QString)),SLOT(connectionMessageReceivedEvent(QString)));
    connect(connection,SIGNAL(clientDisconnected()),SLOT(connectinDisconnectedEvent()));
    connect(connection,SIGNAL(authenticatedChanged(bool)),SLOT(connectionAuthenticationChanged()));
    connect(connection,SIGNAL(registeredEventsChanged(QStringList)),SLOT(connectionRegisteredEventsChanged()));
    connectionMap[connection->uuid()] = connection;

    emit connectionAdded(connection->uuid());
}

void FlowRestService::connectionMessageReceivedEvent(QString message)
{
    FlowRestConnection *connection = qobject_cast<FlowRestConnection*>(sender());
    if(!connection)
        return;
    if(debug())
        qDebug() << "[FlowRestService]: Connection Read[" << connection->description()  << "] " << message;

    QJsonParseError jsonError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(message.toLatin1(),&jsonError);
    if(jsonError.error != QJsonParseError::NoError)
    {
        qDebug() << "[FlowRestService]:Error Accoured in  FlowRestService Received Message Parsing:" << jsonError.errorString() << " At Offset:" << jsonError.offset;
        qDebug() << "  Received Message is:" << message;
        sendErrorTo(connection->uuid(),"Message Parsing Error:" +jsonError.errorString(),"",message);
        return;
    }
    if(!jsonDoc.isObject() || jsonDoc.isEmpty())
    {
        qDebug() << "[FlowRestService]: Received Message is not object or empty";
        qDebug() << "  Received Message is:" << message;
        sendErrorTo(connection->uuid(),"FlowRestService Received Message is not object or empty","",message);
        return;
    }
    bool showReceivedMessage = false;
    for( QString serviceName: jsonDoc.object().keys())
    {
        if(!jsonDoc.object()[serviceName].isObject())
        {
            qDebug() << "[FlowRestService]: Received Message service format error for service:" << serviceName;
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
            qDebug() << "[FlowRestService]: Data From Connection Without Any ServiceProvider:" << serviceName;
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


void FlowRestService::connectinDisconnectedEvent()
{
    FlowRestConnection *connection = qobject_cast<FlowRestConnection*>(sender());;
    if(!connection)
        return;
    if(debug())
        qDebug() << "[FlowRestService] Connection Closed:" << connection->description();

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

void FlowRestService::connectionAuthenticationChanged()
{
    FlowRestConnection *connection = qobject_cast<FlowRestConnection*>(sender());
    if(connection->authenticated())
    {
        setAuthenticatedCount(authenticatedCount() + 1);
    }else
    {
        connection->setRegisteredEvents(QStringList());
        setAuthenticatedCount(authenticatedCount() - 1);
    }
}

void FlowRestService::connectionRegisteredEventsChanged()
{
    FlowRestConnection *connection = qobject_cast<FlowRestConnection*>(sender());

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

