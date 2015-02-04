#include "FlowNode.h"
#include "FlowPort.h"
#include "FlowGraph.h"
#include "FlowNodeMonitorInfo.h"
#include "FlowMonitor.h"

#include <QtCore/QDebug>
#include <QtCore/QChildEvent>
#include <QMetaProperty>
#include <QUuid>

#include <QtQml/QtQml>

//-------------------------------------  flowPacket Tags
#define FLOW_OPERATION_TAG "_FOp"
#define FLOW_SOURCE_TAG "_FSrc"
#define FLOW_DESTINATION_TAG "_FDst"
#define FLOW_DESTINATIODEVICE_TAG "_FSharedD"

FlowNode::FlowNode(QObject *parent) :
    QObject(parent),
    m_cachedFlowGraph(nullptr),
    m_monitorInfo(new FlowNodeMonitorInfo(this)),
    m_serviceInfo(new FlowNodeServiceInfo(this))
{
    m_enabled = true;
    m_debug = false;
    m_uuid = QUuid::createUuid().toString();
    m_nodeDropCount = 0;
    m_nodeRecvRequestCount = 0 ;
    m_nodeRecvResponseCount = 0;
    m_nodeSendRequestCount = 0 ;
    m_nodeSendResponseCount = 0;

    connect(this,&FlowNode::requestReceived,this,&FlowNode::processRequest);
    connect(this,&FlowNode::responseReceived,this,&FlowNode::processResponse);
    connect(this,&FlowNode::serviceRequestReceived,this,&FlowNode::processServiceRequest);
    QTimer::singleShot(0,this,SLOT(autoCheckPorts()));
}

FlowNode::~FlowNode()
{
    m_monitorInfo->setEnabled(false);
    m_monitorInfo->deleteLater();
}

/*
 * NOTE:
 * all calls to this function will be failed when called from  CONSTRUCTOR
 * calls from constructor must be deleayed for later
 */
FlowGraph *FlowNode::flowGraph()
{
    if(m_cachedFlowGraph)
        return m_cachedFlowGraph;
    QObject *current = this;
    FlowGraph *graph;
    while(current)
    {
        graph = qobject_cast<FlowGraph*>(current);
        if(graph)
        {
            m_cachedFlowGraph = graph;
            return m_cachedFlowGraph;
        }
        Q_ASSERT_X(current->parent(),name().toLatin1(),"FlowGraph for Node not found!!!!");
        current = current->parent();
    }
    Q_ASSERT_X(nullptr,name().toLatin1(),"FlowGraph for Node not found!!!!");
    return nullptr;
}

QList<FlowPort *> FlowNode::ports()
{
    QList<FlowPort *> result;
    for(QObject *child:children())
    {
        FlowPort *port = qobject_cast<FlowPort*>(child);
        if(port)
            result.append(port);
    }
    return result;
}

bool FlowNode::sendResponseToPort(FlowInPort *port, QVariantMap flowPacket)
{
    if(!enabled())
    {
        addDropCount();
        return false;
    }
    if(debug())
        qDebug() << "<--Response" << "[" << graphName()+":"+name() +":"+port->name() << "]" <<  flowPacket; //prettyPrint(flowPacket);

//    if(!flowPacket.contains(FLOW_ROUTING_TAG))
//        flowPacket[FLOW_ROUTING_TAG] = QUuid::createUuid().toString() + ".Routing";

    m_nodeSendResponseCount ++; emit nodeSendResponseCountChanged(m_nodeSendResponseCount);
    return port->_responseToWire(flowPacket);
}

bool FlowNode::sendRequestToPort(FlowOutPort *port, QVariantMap flowPacket)
{
    // push stack
//    pushFlowIdStack(flowPacket);
    if(!enabled())
    {
        addDropCount();
        return false;
    }
    if(debug())
        qDebug() << "<--Request" << "[" << graphName()+":"+name() +":"+port->name() << "]" <<  flowPacket; //prettyPrint(flowPacket);

//    if(!flowPacket.contains(FLOW_ROUTING_TAG))
//        flowPacket[FLOW_ROUTING_TAG] = QUuid::createUuid().toString() + ".Routing";

    m_nodeSendRequestCount ++; emit nodeSendRequestCountChanged(m_nodeSendRequestCount);
    return port->_requestToWire(flowPacket);
}

FlowInPort* FlowNode::findPort(QString portName)
{
    if(portName.isEmpty())
        return nullptr;

    for(FlowPort *port:ports())
    {
        FlowInPort* inPort = qobject_cast<FlowInPort*>(port);
        if(!inPort)
            continue;
        if(inPort->name() == portName)
            return inPort;
    }
    return nullptr;
}


void FlowNode::setupRemotePacket(QVariantMap &flowPacket, QString source, QString destination)
{
//    if(!operation.isEmpty())
//        flowPacket[FLOW_OPERATION_TAG] = operation;
    flowPacket[FLOW_SOURCE_TAG] = source;
    flowPacket[FLOW_DESTINATION_TAG] = destination;
}

void FlowNode::setupRemoteDevicePacket(QVariantMap &flowPacket, QString destinatinDevice)
{
    if(!destinatinDevice.isEmpty())
        flowPacket[FLOW_DESTINATIODEVICE_TAG] = destinatinDevice;
}

//QString FlowNode::getRemoteOperation(QVariantMap flowPacket)
//{
//    return flowPacket[FLOW_OPERATION_TAG].toString();
//}

QString FlowNode::getRemoteSource(QVariantMap flowPacket)
{
    return flowPacket[FLOW_SOURCE_TAG].toString();
}

QString FlowNode::getRemoteDestination(QVariantMap flowPacket)
{
    return flowPacket[FLOW_DESTINATION_TAG].toString();
}

QString FlowNode::getRemoteDestinationDevice(QVariantMap flowPacket)
{
    return flowPacket[FLOW_DESTINATIODEVICE_TAG].toString();
}

void FlowNode::classBegin()
{
}

void FlowNode::componentComplete()
{
    initializeNode();
}

QString FlowNode::graphName()
{
    FlowGraph *graph = flowGraph();
    if(!graph)
        return "";    
    return flowGraph()->name();
}

void FlowNode::addDropCount()
{
    m_nodeDropCount ++;
    emit nodeDropCountChanged(m_nodeDropCount);
}


//-------------------------------------

void FlowNode::autoCheckPorts()
{
    if(!qmlEngine(this))
        initializeNode();

    for(int i=0;i<metaObject()->propertyCount();i++)
    {
        QMetaProperty mprop = metaObject()->property(i);
        QVariant portv = mprop.read(this);
        if(portv.isValid() && portv.canConvert<FlowPort*>())
        {
            FlowPort *port = portv.value<FlowPort*>();
            if(port)
                Q_ASSERT_X(port->parent(),"FlowNode","parent not set Correctly!!!");
        }
    }
}


void FlowNode::processRequest(FlowInPort *inPort, QVariantMap flowPacket)
{    
    Q_UNUSED(inPort)
    Q_UNUSED(flowPacket)
    // do echo
    //inPort->sendToWire(flowPacket);
}

void FlowNode::processResponse(FlowOutPort *outPort, QVariantMap flowPacket)
{
    Q_UNUSED(outPort)
    Q_UNUSED(flowPacket)
}

void FlowNode::processServiceRequest(QString clientId, QVariantMap requestData)
{
    Q_UNUSED(clientId)
    Q_UNUSED(requestData)
    qDebug() << "processServiceRequest not implemented:" << graphName()+":"+name() ;
}

void FlowNode::initializeNode()
{
    // all initialization codes goes here.
}

void FlowNode::append_data(QQmlListProperty<QObject> *list, QObject *d)
{
    FlowNode *me = qobject_cast<FlowNode *>(list->object);
    if (me) {
        me->m_data.append(d);
    }
}

void FlowNode::_fromPort(QVariantMap flowPacket)
{
    if(!enabled())
    {
        if(debug())
            qDebug() << "FlowNode[" << graphName()+":"+name() << "]" << "nod used in not enabled mode!!!";
        addDropCount();
        return;
    }

    FlowPort *port = qobject_cast<FlowPort*>(sender());
    if(!port)
    {
        addDropCount();
        return;
    }
    FlowInPort *inPort = qobject_cast<FlowInPort*>(port);
    FlowOutPort *outPort = qobject_cast<FlowOutPort*>(port);
    if(inPort)
    {
        if(debug())
            qDebug() << "-->Request" << "[" << graphName()+":"+name() +":"+port->name() << "]" <<  flowPacket; //prettyPrint(flowPacket);
        m_nodeRecvRequestCount++ ; emit nodeRecvRequestCountChanged(m_nodeRecvRequestCount);
        emit requestReceived(inPort,flowPacket);
    }else if(outPort)
    {
        if(debug())
            qDebug() << "-->Response" << "[" << graphName()+":"+name() +":"+port->name() << "]" <<  flowPacket; //prettyPrint(flowPacket);
//        popFlowIdStack(flowPacket);
        m_nodeRecvResponseCount++ ; emit noedRecvResponseCountChanged(m_nodeRecvResponseCount);
        emit responseReceived(outPort,flowPacket);
    }else
    {
        addDropCount();
        qDebug() << "ERROR!!!!!!!!!!!!!!!! P[" << port->name() << "]!!!!!=>" << "N[" << name() << "]" <<  flowPacket; //prettyPrint(flowPacket);
    }
}

void FlowNode::_fromServiceCenter(QString clientId, QVariantMap requestData)
{
    emit serviceRequestReceived(clientId,requestData);
}

