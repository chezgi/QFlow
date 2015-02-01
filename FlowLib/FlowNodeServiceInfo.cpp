#include "FlowNodeServiceInfo.h"
#include "FlowGraph.h"
#include "FlowNode.h"

#include <QtQml/QtQml>

FlowNodeServiceInfo::FlowNodeServiceInfo(FlowNode *parent) :
    QObject((QObject *)parent)
{    
    load_completed = false;
    m_enabled = false;
    m_debug = false;
    m_anonymousAllowed = false;
    QTimer::singleShot(0,this,SLOT(checkInitialization()));
}

FlowServiceConnection *FlowNodeServiceInfo::getConnection(QString clientId)
{
    FlowNode * node = (FlowNode *)parent();
    return node->flowGraph()->service()->getConnection(clientId);
}

void FlowNodeServiceInfo::broadcast(QVariantMap data)
{
    FlowNode * node = (FlowNode *)parent();
    return node->flowGraph()->service()->broadcast(name(),data);
}

void FlowNodeServiceInfo::sendTo(QString clientId, QVariantMap data)
{
    FlowNode * node = (FlowNode *)parent();
    return node->flowGraph()->service()->sendTo(clientId,name(),data);
}

void FlowNodeServiceInfo::sendErrorTo(QString clientId, QString errorMessage, QVariant request)
{
    FlowNode * node = (FlowNode *)parent();
    return node->flowGraph()->service()->sendErrorTo(clientId,errorMessage,name(),request);
}

bool FlowNodeServiceInfo::hasRegisteredClient()
{
    FlowNode * node = (FlowNode *)parent();
    return node->flowGraph()->service()->hasRegisteredClient(name());
}

void FlowNodeServiceInfo::componentComplete()
{
    FlowNode * node = (FlowNode *)parent();
    Q_ASSERT_X(node,"serviceinfo","can't find parent node.");
    if(enabled())
    {
        if(debug())
            qDebug() << "registering service[" << node->name() << "]:" << name();
        if(node->flowGraph())
            node->flowGraph()->service()->registerServiceProvider(node,name(),api(),anonymousAllowed());
        else
            qDebug() << "Alert: flowGraph not valid at EnableService" << node->flowGraph()->name()+":"+node->name();
    }
    load_completed = true;
}

void FlowNodeServiceInfo::checkInitialization()
{
    if(!qmlEngine(this))
        componentComplete();
}

void FlowNodeServiceInfo::setEnabled(bool arg)
{
    if (m_enabled != arg) {
        m_enabled = arg;
        if(load_completed)
        {
            FlowNode * node = (FlowNode *)parent();
            Q_ASSERT_X(node,"serviceinfo","can't find parent node.");

            if(m_enabled)
            {
                if(debug())
                    qDebug() << "registering service[" << node->name() << "]:" << name();

                if(node->flowGraph())
                    node->flowGraph()->service()->registerServiceProvider(node,name(),api());
                else
                    qDebug() << "Alert: flowGraph not valid at EnableService" << node->flowGraph()->name()+":"+node->name();
            }else
            {
                if(debug())
                    qDebug() << "unregistering service[" << node->name() << "]:" << name();

                if(node->flowGraph())
                    node->flowGraph()->service()->unregisterServiceProvider(name());
            }
        }
        emit enabledChanged(arg);
    }
}
