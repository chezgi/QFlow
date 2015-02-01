#include "FlowPort.h"
#include "FlowNode.h"
#include "FlowTracer.h"

#include <QtCore/QMetaMethod>
#include <QtCore/QDebug>
#include <QtCore/QUuid>
#include <QEvent>

#define FLOW_ROUTING_TAG "_FROUTE"
#define FLOW_STACKEDID_TAG "_FTSTACK"
//#define FLOW_ROUTING_TAG "_FId"
//#define FLOW_STACKEDID_TAG "_FIdStack"


FlowPort::FlowPort(QString portName, Direction dir, QObject *parent) :
    QObject(parent),
    m_name(portName),
    m_direction(dir)
{    
    m_cachedGraph = nullptr;
    m_portSendCount = 0;
    m_portRecvCount = 0;
    m_portDropCount = 0;    
    m_enabled = true;
    m_debug = false;
    m_uuid = QUuid::createUuid().toString();
}

QString FlowPort::getFlowId(QVariantMap flowPacket)
{
    return flowPacket[FLOW_ROUTING_TAG].toString();
}

QVariantList FlowPort::getFlowIdStack(QVariantMap flowPacket)
{
    return flowPacket[FLOW_STACKEDID_TAG].toList();
}

FlowGraph *FlowPort::flowGraph()
{
    if(m_cachedGraph)
        return m_cachedGraph;
    FlowNode * node = qobject_cast<FlowNode*>(parent());
    if(!node)
        return nullptr;
    m_cachedGraph = node->flowGraph();
    return m_cachedGraph;
}

QString FlowPort::nodeName()
{
    if(!m_cachedNodeName.isEmpty())
        return m_cachedNodeName;
    if(!parent())
        return "";
    FlowNode * node = qobject_cast<FlowNode*>(parent());
    if(!node)
        return "";
    m_cachedNodeName = node->name();
    return m_cachedNodeName;
}

void FlowPort::addSendCount()
{
    m_portSendCount ++;
    emit portSendCountChanged(m_portSendCount);
}

void FlowPort::addRecvCount()
{
    m_portRecvCount ++ ;
    emit portRecvCountChanged(m_portRecvCount);
}

void FlowPort::addDropCount()
{
    m_portDropCount ++;
    emit portDropCountChanged(m_portDropCount);
}


FlowPort::Direction FlowPort::direction() const
{
    return m_direction;
}


bool FlowPort::enabled() const
{
    return m_enabled;
}

QString FlowPort::name() const
{
    return m_name;
}

QString FlowPort::description() const
{
    return m_description;
}

void FlowPort::setEnabled(bool arg)
{
    if (m_enabled != arg) {
        m_enabled = arg;
        emit enabledChanged(arg);
    }
}

void FlowPort::setName(QString arg)
{
    if (m_name != arg) {
        m_name = arg;
        emit nameChanged(arg);
    }
}

void FlowPort::setDescription(QString arg)
{
    if (m_description != arg) {
        m_description = arg;
        emit descriptionChanged(arg);
    }
}

void FlowPort::setDebug(bool arg)
{
    if (m_debug != arg) {
        m_debug = arg;
        emit debugChanged(arg);
    }
}

bool FlowPort::debug() const
{
    return m_debug;
}

quint64 FlowPort::portSendCount() const
{
    return m_portSendCount;
}

quint64 FlowPort::portRecvCount() const
{
    return m_portRecvCount;
}

quint64 FlowPort::portDropCount() const
{
    return m_portDropCount;
}

QString FlowPort::uuid() const
{
    return m_uuid;
}


//-------------------------------------------------------------------- IN
FlowInPort::FlowInPort(QString portName, QObject *parent):
    FlowPort(portName,FlowPort::Input,parent),
    m_requestRouter(new FlowRequestRouter(this))
{
    m_requestRouter->setName(portName + ":RR");
}

FlowRequestRouter *FlowInPort::requestRouter() const
{
    return m_requestRouter;
}

bool FlowInPort::hasPreviousRoute(QVariantMap flowPacket)
{
    return requestRouter()->contains(flowPacket);
}

void FlowInPort::_requestFromWire(FlowOutPort *senderPort, QVariantMap flowPacket)
{
    if(debug())
        qDebug()  << "FlowInPort[" << nodeName() +":" + name()  << "]" << "fromWire Called:" << flowPacket;
    addRecvCount();
    if(!enabled())
    {
        addDropCount();
        return;
    }

    if(senderPort)
    {
        //            Q_ASSERT( flowPacket.contains(FlowNode::flowRoutingTag()) );
        if(debug())
            qDebug() << "FlowInPort[" << nodeName() +":" + name()  << "]"  << "Adding Request Info For Future Routings:" << flowPacket;
        requestRouter()->add(flowPacket,senderPort);
    }else
    {
        qDebug()  << "FlowInPort[" << nodeName() +":" + name()  << "]" << "Unknown sender port !!!!";
    }

    if(debug())
        qDebug()  << "FlowInPort[" << nodeName() +":" + name()  << "]" << "sending to Node:" << flowPacket;


    if(flowGraph() && flowGraph()->traceEnabled())
    {
        flowGraph()->traceRequest(nodeName(),name(),flowPacket);
    }

    if(!isSignalConnected(QMetaMethod::fromSignal(&FlowPort::_toNode)))
    {
        if(parent())
        {
            FlowNode * p = qobject_cast<FlowNode*>(parent());
            if(p)
            {
                connect(this,&FlowPort::_toNode,p,&FlowNode::_fromPort, Qt::QueuedConnection);
            }else
            {
                addDropCount();
                qDebug()  << "FlowInPort[" << nodeName() +":" + name()  << "]" << "Port Has Incorrect Parent. Port must be a child of Node.";
            }
        }else
        {
            addDropCount();
            qDebug()  << "FlowInPort[" << nodeName() +":" + name()  << "]" << "Port Has No Parent!!!!";
        }
    }
    emit _toNode(flowPacket);
}

bool FlowInPort::_responseToWire(QVariantMap flowPacket)
{    
    if(debug())
        qDebug() << "FlowInPort[" << nodeName() +":" + name()  << "]" << "sendToWire Called:" << flowPacket;
    if(!enabled())
    {
        addDropCount();
        return false;
    }
//    Q_ASSERT( flowPacket.contains(FlowNode::flowRoutingTag()) );

    if(flowGraph() && flowGraph()->traceEnabled())
    {
        flowGraph()->traceResponse(nodeName(),name(),flowPacket);
    }

    FlowOutPort *reqPort = qobject_cast<FlowOutPort*>(requestRouter()->take(flowPacket));
    if(reqPort)
    {
        if(debug())
            qDebug() << "FlowInPort[" << nodeName() +":" + name()  << "]"  << "Sending Response To Other Side:" << flowPacket;
        addSendCount();
        reqPort->_responseFromWire(flowPacket);
        return true;
    }
    qDebug() << "FlowInPort[" << nodeName() +":" + name()  << "]"  << "Sending Response To Other Side Failed [No otherSide Found]:" << flowPacket;
    addDropCount();
    return false;

//    if(!isSignalConnected(QMetaMethod::fromSignal(&FlowPort::_toWire)))
//    {
//        qDebug()  << "FlowInPort[" << nodeName() +":" + name()  << "]" << "toWire Not Connected!!!!:";
//        return false;
//    }
//    if(debug())
//        qDebug()  << "FlowInPort[" << nodeName() +":" + name()  << "]" << "Sending Request To Wire:" << flowPacket;
//    emit _toWire(flowPacket);
//    return true;
}

//-------------------------------------------------------------------- OUT
FlowOutPort::FlowOutPort(QString portName, QObject *parent):FlowPort(portName,FlowPort::Output,parent)
{
    m_next = nullptr;
}


void FlowOutPort::pushFlowIdStack(QVariantMap &flowPacket)
{
    QVariantList stack;
    QString newTag = QUuid::createUuid().toString() + ".Routing";
    QString oldTag = flowPacket[FLOW_ROUTING_TAG].toString();
    if(oldTag.isEmpty())
    {
        flowPacket[FLOW_ROUTING_TAG] =  newTag;
        flowPacket[FLOW_STACKEDID_TAG] = stack;
        return;
    }
    if(flowPacket.contains(FLOW_STACKEDID_TAG))
         stack = flowPacket[FLOW_STACKEDID_TAG].toList();
    stack.prepend(oldTag);
    flowPacket[FLOW_STACKEDID_TAG] = stack;
    flowPacket[FLOW_ROUTING_TAG] = newTag;
}

void FlowOutPort::popFlowIdStack(QVariantMap &flowPacket)
{
    if(!flowPacket.contains(FLOW_STACKEDID_TAG))
    {
        qDebug() << "No Flow Tag Stack !!!!";
        if(flowPacket.contains(FLOW_ROUTING_TAG))
            flowPacket.remove(FLOW_ROUTING_TAG);
        return ;
    }
    QVariantList stack = flowPacket[FLOW_STACKEDID_TAG].toList();
    QString oldTag;
    if(stack.isEmpty())
    {
        flowPacket.remove(FLOW_STACKEDID_TAG);
        flowPacket.remove(FLOW_ROUTING_TAG);
    }else
    {
        oldTag = stack.takeFirst().toString();
        flowPacket[FLOW_STACKEDID_TAG] = stack;
        flowPacket[FLOW_ROUTING_TAG]  = oldTag;
    }
}


void FlowOutPort::_responseFromWire(QVariantMap flowPacket)
{
    popFlowIdStack(flowPacket);
    addRecvCount();

    if(debug())
        qDebug()  << "FlowOutPort[" << nodeName() +":" + name()  << "]" << "fromWire Called:" << flowPacket;

    if(!enabled())
    {
        qDebug() << "_responseFromWire for disabled port!!!!!";
        addDropCount();
        return;
    }

    if(debug())
        qDebug()  << "FlowOutPort[" << nodeName() +":" + name()  << "]" << "sending to Node:" << flowPacket;

    if(flowGraph() && flowGraph()->traceEnabled())
    {
        flowGraph()->traceResponse(nodeName(),name(),flowPacket);
    }

    if(!isSignalConnected(QMetaMethod::fromSignal(&FlowPort::_toNode)))
    {
        if(parent())
        {
            FlowNode * p = qobject_cast<FlowNode*>(parent());
            if(p)
            {
                connect(this,&FlowPort::_toNode,p,&FlowNode::_fromPort, Qt::QueuedConnection);
            }else
            {
                addDropCount();
                qDebug()  << "FlowOutPort[" << nodeName() +":" + name()  << "]" << "Port Has Incorrect Parent. Port must be a child of Node.";
            }
        }else
        {
            addDropCount();
            qDebug()  << "FlowOutPort[" << nodeName() +":" + name()  << "]" << "Port Has No Parent!!!!";
        }
    }
    emit _toNode(flowPacket);
}

void FlowOutPort::_inportDeleted()
{
    QObject *object = sender();
    if(object == (QObject*)next())
    {
        m_next = nullptr;
        emit nextChanged(m_next);
    }
    QList<FlowInPort *> new_multicast = multicast();
    bool changed = false;
    for(FlowInPort *inport:multicast())
    {
        if(inport == object)
        {
            new_multicast.removeAll(inport);
            changed  = true;
        }
    }
    if(changed)
    {
        m_multicast = new_multicast;
        emit multicastChanged(m_multicast);
    }
}

bool FlowOutPort::_requestToWire(QVariantMap flowPacket)
{
    if(debug())
        qDebug() << "FlowOutPort[" << nodeName() +":" + name()  << "]" << "sendToWire Called:" << flowPacket;
    if(!enabled())
    {
        addDropCount();
        qDebug() << "_requestToWire for disabled port!!!!!";
        return false;
    }
//    Q_ASSERT( flowPacket.contains(FlowNode::flowRoutingTag()) );

//    if(!isSignalConnected(QMetaMethod::fromSignal(&FlowPort::_toWire)))
//    {
//        qDebug()  << "FlowOutPort[" << nodeName() +":" + name()  << "]" << "toWire Not Connected!!!!:";
//        return false;
//    }

    if(flowGraph() && flowGraph()->traceEnabled())
    {
        flowGraph()->traceRequest(nodeName(),name(),flowPacket);
    }

    if(!next() && multicast().isEmpty())
    {
        addDropCount();
        qDebug()  << "FlowOutPort[" << nodeName() +":" + name()  << "]" << "Please set next property!!!!:";
        return false;
    }

    if(debug())
        qDebug()  << "FlowOutPort[" << nodeName() +":" + name()  << "]" << "Sending Request To Wire:" << flowPacket;

    if(next())
    {
        QVariantMap newFlowPacket = flowPacket;
        pushFlowIdStack(newFlowPacket);
        addSendCount();
        next()->_requestFromWire(this,newFlowPacket);
    }
    for(FlowInPort *inport:multicast())
    {
        QVariantMap newFlowPacket = flowPacket;
        pushFlowIdStack(newFlowPacket);
        addSendCount();
        inport->_requestFromWire(this,newFlowPacket);
    }
//    emit _toWire(flowPacket);
    return true;

}

QList<FlowInPort *> FlowOutPort::multicast() const
{
    return m_multicast;
}

FlowInPort *FlowOutPort::next() const
{
    return m_next;
}

void FlowOutPort::setMulticast(QList<FlowInPort *> arg)
{
    if (m_multicast != arg) {
//        for(FlowInPort *port:m_multicast)
//        {
//            if(port == m_next) continue;
//            disconnect(this,&FlowPort::_toWire,port,&FlowInPort::_requestFromWire);
//        }
        for(FlowInPort *port:m_multicast)
            disconnect(port,&QObject::destroyed,this,&FlowOutPort::_inportDeleted);
        m_multicast = arg;
        for(FlowInPort *port:m_multicast)
            connect(port,&QObject::destroyed,this,&FlowOutPort::_inportDeleted,Qt::UniqueConnection);

//        for(FlowInPort *port:m_multicast)
//        {
//            if(port == m_next) continue;
//            Q_ASSERT_X(port,"FlowOutPort:next","next must be a list of FlowInPorts");
//            connect(this,&FlowPort::_toWire,port,&FlowInPort::_requestFromWire,Qt::UniqueConnection);
//        }
        emit multicastChanged(arg);
    }
}

void FlowOutPort::setNext(FlowInPort *arg)
{
    if (m_next != arg) {
//        if(m_next && !m_multicast.contains(m_next))
//            disconnect(this,&FlowPort::_toWire,m_next,&FlowInPort::_requestFromWire);
        if(m_next)
            disconnect(m_next,&QObject::destroyed,this,&FlowOutPort::_inportDeleted);
        m_next = arg;
        if(m_next)
            connect(m_next,&QObject::destroyed,this,&FlowOutPort::_inportDeleted,Qt::UniqueConnection);

//        if(m_next)
//            connect(this,&FlowPort::_toWire,m_next,&FlowInPort::_requestFromWire,Qt::UniqueConnection);
        emit nextChanged(arg);
    }
}
