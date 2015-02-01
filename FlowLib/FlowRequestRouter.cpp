#include "FlowRequestRouter.h"
#include "FlowPort.h"
#include <QDebug>
#include <QVariantMap>

FlowRequestRouter::FlowRequestRouter(QObject *parent) :
    QObject(parent)
{
    m_ttl = 10 * 60; // 10 minutes
    m_debug = false;
    cleanupTimer = new QTimer(this);
    cleanupTimer->setInterval( qMax(m_ttl/10,1) * 1000);
    connect(cleanupTimer,&QTimer::timeout,this,&FlowRequestRouter::doCleaup);
    cleanupTimer->start();
}

FlowRequestRouter::~FlowRequestRouter()
{
    clear();
}

void FlowRequestRouter::add(QVariantMap flowPacket, QObject *object)
{
    if(!object)
        return;
    QString flowId  = FlowPort::getFlowId(flowPacket);
    if(flowId.isEmpty())
        return;
    if(debug())
        qDebug() << "RR["+name()+"]: Adding " + flowId ;
    RequestInfo *info;
    if(m_requestMap.contains(flowId))
    {        
        if(debug())
            qDebug() << "RR["+name()+"]: Old Value " + flowId ;

        info = m_requestMap.value(flowId);
        if(info->requester != object)
            qDebug() << "Internal Error: same flowId from two diffrent objects.!!!!";
        else
            qDebug()<< "Internal Error: flowId must be Unique per send!!!";
        info->requester = object;
        info->updateTime = QDateTime::currentDateTime();
    }else
    {
        info = new RequestInfo;
        info->flowId = flowId;
        info->requester = object;
        connect(object,&QObject::destroyed,this,&FlowRequestRouter::requestPortObjectDeleted,Qt::UniqueConnection);
        m_requestMap.insert(flowId,info);
    }
}

bool FlowRequestRouter::contains(QVariantMap flowPacket)
{
    QString flowId  = FlowPort::getFlowId(flowPacket);
    return m_requestMap.contains(flowId);
}

void FlowRequestRouter::setCount(QVariantMap flowPacket, int count)
{
    QString flowId  = FlowPort::getFlowId(flowPacket);
    if(flowId.isEmpty())
        return;
    RequestInfo *info;
    if(m_requestMap.contains(flowId))
    {
        info = m_requestMap.value(flowId);
        info->count = count;
    }
}

int FlowRequestRouter::getCount(QVariantMap flowPacket)
{
    QString flowId  = FlowPort::getFlowId(flowPacket);
    if(flowId.isEmpty())
        return 0;
    RequestInfo *info;
    if(m_requestMap.contains(flowId))
    {
        info = m_requestMap.value(flowId);
        return info->count;
    }
    return 0;
}

void FlowRequestRouter::setOutPortId(QVariantMap flowPacket, int port)
{
    QString flowId  = FlowPort::getFlowId(flowPacket);
    if(flowId.isEmpty())
        return;
    RequestInfo *info;
    if(m_requestMap.contains(flowId))
    {
        info = m_requestMap.value(flowId);
        info->outPortId = port;
    }
}

int FlowRequestRouter::getOutPortId(QVariantMap flowPacket)
{
    QString flowId  = FlowPort::getFlowId(flowPacket);
    if(flowId.isEmpty())
        return -1;
    RequestInfo *info;
    if(m_requestMap.contains(flowId))
    {
        info = m_requestMap.value(flowId);
        return info->outPortId;
    }
    return -1;
}

QObject *FlowRequestRouter::get(QVariantMap flowPacket)
{
    RequestInfo *info;
    QString flowId  = FlowPort::getFlowId(flowPacket);
    if(!m_requestMap.contains(flowId))
        return nullptr;
    if(debug())
        qDebug() << "RR["+name()+"]: get " + flowId ;

    info = m_requestMap.value(flowId);
    return info->requester;
}

QObject *FlowRequestRouter::take(QVariantMap flowPacket)
{

    QObject * result= nullptr;
    RequestInfo *info;
    QString flowId  = FlowPort::getFlowId(flowPacket);
    if(debug())
        qDebug() << "RR["+name()+"]: Take " << m_requestMap.contains(flowId) <<  flowId ;
    if(!m_requestMap.contains(flowId))
        return result;
    info = m_requestMap.take(flowId);
    result = info->requester;
    delete info;
    return result;
}

void FlowRequestRouter::clear()
{
    QMutableMapIterator<QString,RequestInfo *> mit(m_requestMap);
    while(mit.hasNext())
    {
        mit.next();
        RequestInfo *info = mit.value();
        info->requester->disconnect(this);
        mit.remove();
        delete info;
    }
}

void FlowRequestRouter::doCleaup()
{
    if(debug())
        qDebug() << "RR Before Cleanup Count[" + name() + "]:" << m_requestMap.count();
    QMutableMapIterator<QString,RequestInfo *> mit(m_requestMap);
    while(mit.hasNext())
    {
        mit.next();
        RequestInfo *info = mit.value();
        if(info->updateTime.secsTo(QDateTime::currentDateTime()) > ttl())
        {
            mit.remove();
            info->requester->disconnect(this);
            delete info;
        }
    }
    if(debug())
        qDebug() << "RR After Cleanup Count[" + name() + "]:" << m_requestMap.count();
}

void FlowRequestRouter::requestPortObjectDeleted()
{
    QObject * object = sender();
    if(!object)
        return;
    QMutableMapIterator<QString,RequestInfo *> mit(m_requestMap);
    while(mit.hasNext())
    {
        mit.next();
        RequestInfo *info = mit.value();
        if(info->requester == object)
        {
            mit.remove();
            delete info;
        }
    }
}

//----------------------------------------------- properties
int FlowRequestRouter::ttl() const
{
    return m_ttl;
}

QString FlowRequestRouter::name() const
{
    return m_name;
}

bool FlowRequestRouter::debug() const
{
    return m_debug;
}

void FlowRequestRouter::setTtl(int arg)
{
    if (m_ttl != arg) {
        m_ttl = arg;
        cleanupTimer->setInterval( qMax(m_ttl/2,1) * 1000);
        emit ttlChanged(arg);
    }
}

void FlowRequestRouter::setName(QString arg)
{
    if (m_name != arg) {
        m_name = arg;
        emit nameChanged(arg);
    }
}

void FlowRequestRouter::setDebug(bool arg)
{
    if (m_debug != arg) {
        m_debug = arg;
        emit debugChanged(arg);
    }
}


