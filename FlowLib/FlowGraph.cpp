#include "FlowGraph.h"
#include "FlowLogger.h"
#include "FlowMonitor.h"
#include "FlowNode.h"
#include "FlowService.h"
#include "FlowJsonStorage.h"
#include "FlowObjectStorage.h"
#include "FlowTracer.h"
#include <QDebug>
#include <QUuid>

quint64 FlowGraph::traceId = 0;

FlowGraph::FlowGraph(QObject *parent) :
    QObject(parent),
    m_objectStorage(new FlowObjectStorage(this)),
    m_logger(new FlowLogger(this)),
    m_monitor(new FlowMonitor(this)),
    m_service(new FlowService(this)),
    m_jsonStorage(new FlowJsonStorage(this))
{
    m_traceEnabled = false;
    m_config = nullptr;
    m_defaultLoggerIsUsed = true;
    m_defaultMonitorIsUsed = true;
    m_debug = false;
    m_creationTime = QDateTime::currentDateTime();
    m_uuid = QUuid::createUuid().toString();
}

QList<FlowNode *> findSubNodes(QObject *root)
{
    QList<FlowNode *> result;
    FlowNode *fnode = qobject_cast<FlowNode*>(root);
#if 0
    FlowGraph *fgraph = qobject_cast<FlowGraph*>(root);
    FlowSubgraph *fcomponent = qobject_cast<FlowSubgraph*>(root);
    if(!fnode && !fcomponent && !fgraph)
        return result;
#endif
    if(fnode)
        result.append(fnode);

    for(QObject *child:root->children())
    {
        result += findSubNodes(child);
    }
    return result;
}

QList<FlowNode *> FlowGraph::nodes()
{    
    return findSubNodes(this);
}

FlowNode *FlowGraph::findNode(QString nodeName)
{
    if(nodeName.isEmpty())
        return nullptr;
    for(FlowNode *node:nodes())
    {
        if(node->name() == nodeName)
            return node;
    }
    return nullptr;
}

FlowInPort *FlowGraph::findPort(QString nodeName, QString portName)
{
    if(nodeName.isEmpty() || portName.isEmpty())
        return nullptr;

    for(FlowNode *node:nodes())
    {
        if(node->name() == nodeName)
            return node->findPort(portName);
    }
    return nullptr;
}

/*
gather all topology info. format is:
graph:{
   uuid,name,creationTime
   nodes:[
        {
           uuid,name,monitored,enabled,
           monitorInfo:{ name,category,monitoredProperties,currentStatus}
           statistics:{sendRequestCount,sendResponseCount,recvRequestCount,recvResponseCount,dropCount}
           inPorts:[
              {
                  uuid,name,enabled
                  statistics:{sendCount,recvCount,dropCount}
               },....
           ],
           outPorts:[
              {
                  uuid,name,enabled,next,multicast
                  statistics:{sendCount,recvCount,dropCount}
               },....
           ]
        },....
   ]
}
*/
QVariantMap FlowGraph::topologyMap()
{
    QVariantMap graphMap;
    graphMap["uuid"] = uuid();
    graphMap["name"] = name();
    graphMap["creationTime"] = creationTime().toString(Qt::ISODate);
    QVariantList nodesList;
    for(FlowNode *node:nodes())
    {
        QVariantMap nodeMap;
        nodeMap["uuid"] = node->uuid();
        nodeMap["name"] = node->name();
//        nodeMap["monitored"] = node->monitorInfo()->;
        nodeMap["enabled"] = node->enabled();
        QVariantMap nodeStatistics;
        {
            nodeStatistics["sendRequestCount"] = node->nodeSendRequestCount();
            nodeStatistics["sendResponseCount"] = node->nodeSendResponseCount();
            nodeStatistics["recvRequestCount"] = node->nodeRecvRequestCount();
            nodeStatistics["recvResponseCount"] = node->nodeRecvResponseCount();
            nodeStatistics["dropCount"] = node->nodeDropCount();
        }
        nodeMap["statistics"] = nodeStatistics;
        QVariantMap monitorMap;
        {
             monitorMap["name"] = node->monitorInfo()->name();
             monitorMap["category"] = node->monitorInfo()->category();
             monitorMap["monitoredProperties"] = node->monitorInfo()->monitoredProperties();
             monitorMap["monitoredCounters"] = node->monitorInfo()->monitoredCounters();
//             monitorMap["monitoredSignals"] = node->monitorInfo()->monitoredSignals();
        }
        nodeMap["monitorInfo"] = monitorMap;
        QVariantList inportsList,outportsList;
        for(FlowPort *port:node->ports())
        {
            FlowInPort *inport = qobject_cast<FlowInPort*>(port);
            FlowOutPort *outport = qobject_cast<FlowOutPort*>(port);
            if(inport)
            {
                QVariantMap portMap;
                portMap["uuid"] = inport->uuid();
                portMap["name"] = inport->name();
                portMap["enabled"] = inport->enabled();
                QVariantMap portStatistics;
                {
                    portStatistics["sendCount"] = inport->portSendCount();
                    portStatistics["recvCount"] = inport->portRecvCount();
                    portStatistics["dropCount"] = inport->portDropCount();
                }
                portMap["statistics"] = portStatistics;
                inportsList << portMap;
            }
            if(outport)
            {
                QVariantMap portMap;
                portMap["uuid"] = outport->uuid();
                portMap["name"] = outport->name();
                portMap["enabled"] = outport->enabled();
                if(outport->next())
                    portMap["next"] = outport->next()->uuid();
                QStringList multicastUuids;
                for(FlowInPort *mport:outport->multicast())
                {
                    multicastUuids << mport->uuid();
                }
                portMap["multicast"] = multicastUuids;
                QVariantMap portStatistics;
                {
                    portStatistics["sendCount"] = outport->portSendCount();
                    portStatistics["recvCount"] = outport->portRecvCount();
                    portStatistics["dropCount"] = outport->portDropCount();
                }
                portMap["statistics"] = portStatistics;
                outportsList << portMap;
            }
        }
        nodeMap["inPorts"] = inportsList;
        nodeMap["outPorts"] = outportsList;
        nodesList << nodeMap;
    }
    graphMap["nodes"] = nodesList;
    return graphMap;
}

void FlowGraph::componentComplete()
{
    initializeGraph();
}

void FlowGraph::initializeGraph()
{
}

void FlowGraph::traceRequest(QString node, QString port, QVariantMap flowPacket)
{
    if(!traceEnabled())
        return;    
    QVariantMap trace = createTrace(true,node,port,flowPacket);
    if(debug())
        qDebug() << "[trace] added:" << trace;
    emit newTrace(trace);
}

void FlowGraph::traceResponse(QString node, QString port, QVariantMap flowPacket)
{
    if(!traceEnabled())
        return;

    QVariantMap trace = createTrace(false,node,port,flowPacket);
    if(debug())
        qDebug() << "[trace] added:" << trace;
    emit newTrace(trace);
}

QVariantMap FlowGraph::createTrace(bool isRequest, QString node, QString port, QVariantMap flowPacket)
{
    QVariantMap trace;
//    if(FlowPort::getFlowId(flowPacket).isEmpty())
//    {
//        qDebug() << "request routeId unknown:" << node << port;
//    }else
//    {
//        qDebug() << "RouteId: " << node << port<< FlowPort::getFlowId(flowPacket);
//    }
    FlowObject *flowObject = objectStorage()->getByPacket(flowPacket);
    QVariantMap data;
    if(flowObject)
    {
        data = flowObject->toMap();

    }
    trace["data"] = data;    
    trace["traceId"] = ++traceId;
    if(isRequest)
        trace["type"] = "request";
    else
        trace["type"] = "response";
    trace["graph"] =  name();
    trace["node"] = node;
    trace["port"] = port;
    if(data.contains("uuid"))
        trace["uuid"] = data["uuid"];
    else
        trace["uuid"] = "";
    trace["routeId"] = FlowPort::getFlowId(flowPacket);
    trace["routeStack"] = FlowPort::getFlowIdStack(flowPacket);
    return trace;
}


//------------------------------------- properties
QString FlowGraph::name() const
{
    return m_name;
}

QString FlowGraph::description() const
{
    return m_description;
}

FlowObjectStorage *FlowGraph::objectStorage() const
{
    return m_objectStorage;
}

bool FlowGraph::debug() const
{
    return m_debug;
}

FlowLogger *FlowGraph::logger() const
{
    return m_logger;
}

FlowMonitor *FlowGraph::monitor() const
{
    return m_monitor;
}

FlowService *FlowGraph::service() const
{
    return m_service;
}

QDateTime FlowGraph::creationTime() const
{
    return m_creationTime;
}

QString FlowGraph::uuid() const
{
    return m_uuid;
}

FlowJsonStorage *FlowGraph::jsonStorage() const
{
    return m_jsonStorage;
}



void FlowGraph::setName(QString arg)
{
    if (m_name != arg) {
        m_name = arg;
        emit nameChanged(arg);
    }
}

void FlowGraph::setDescription(QString arg)
{
    if (m_description != arg) {
        m_description = arg;
        emit descriptionChanged(arg);
    }
}

void FlowGraph::setDebug(bool arg)
{
    if (m_debug != arg) {
        m_debug = arg;
        emit debugChanged(arg);
    }
}

void FlowGraph::setLogger(FlowLogger *arg)
{
    if (m_logger != arg) {
        if(m_defaultLoggerIsUsed)
        {
            m_defaultLoggerIsUsed = false;
        }
        m_logger = arg;
        emit loggerChanged(arg);
    }
}

void FlowGraph::setMonitor(FlowMonitor *arg)
{
    if (m_monitor != arg) {
        if(m_defaultMonitorIsUsed)
        {
            m_defaultMonitorIsUsed = false;
        }
        m_monitor = arg;
        emit monitorChanged(arg);
    }
}

void FlowGraph::setService(FlowService *arg)
{
    if (m_service != arg) {
        m_service = arg;
        emit serviceChanged(arg);
    }
}
