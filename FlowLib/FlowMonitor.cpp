#include "FlowMonitor.h"
#include "FlowNode.h"
#include "FlowNodeMonitorInfo.h"
#include "FlowCommunicationUtils.h"

FlowMonitor::FlowMonitor(QObject *parent) :
    QObject(parent)
{
//    m_maxEventHistory = 100;
    m_maxAlertHistory = 100;
    m_debug = false;
}

void FlowMonitor::registerNode(FlowNode *node)
{
    if(!node)
        return;
    if(node->monitorInfo()->name().isEmpty() || node->monitorInfo()->category().isEmpty())
        qDebug() << "Alert: registered monitoring node without name and or catagory:" << node->name();
    connect(node->monitorInfo(),SIGNAL(currentStatusChanged(QVariantMap)),SLOT(nodeCurrentStatusChanged()));
//    connect(node->monitorInfo(),SIGNAL(newEvent(QString)),SLOT(nodeNewEventReceived(QString)));
    connect(node->monitorInfo(),SIGNAL(newAlert(int,QString)),SLOT(nodeNewAlertReceived(int,QString)));
    connect(node->monitorInfo(),SIGNAL(destroyed()),SLOT(nodeMonitorInfoRemoved()));
    nodeMonitorInfos.append(node->monitorInfo());

    //1- we must append current status.
    m_currentStatus[node->monitorInfo()->category() +"."+ node->monitorInfo()->name()] = node->monitorInfo()->currentStatus();
    emit currentStatusChanged(m_currentStatus);
    QVariantMap varStatus;
    varStatus[node->monitorInfo()->category() +"."+ node->monitorInfo()->name()] = node->monitorInfo()->currentStatus();
    emit newNodeStatus(varStatus);

    if(debug())
    {
        qDebug() << "Flow Monitor: node registered[" << node->name() << "]" << node->monitorInfo()->category() + "." + node->monitorInfo()->name();
    }
}

void FlowMonitor::unregisterNode(FlowNode *node)
{
    if(!node)
        return;
    node->monitorInfo()->disconnect(this);
    if(nodeMonitorInfos.contains(node->monitorInfo()))
    {
        nodeMonitorInfos.removeAll((node->monitorInfo()));
        if(debug())
        {
            qDebug() << "Flow Monitor: node unregistered[" << node->name() << "]" << node->monitorInfo()->category() + "." + node->monitorInfo()->name();
        }
    }
}

void FlowMonitor::nodeCurrentStatusChanged()
{
    FlowNodeMonitorInfo *flowNodeMonitorInfo = qobject_cast<FlowNodeMonitorInfo*>(sender());
    if(!flowNodeMonitorInfo)
        return;
    m_currentStatus[flowNodeMonitorInfo->category() +"."+ flowNodeMonitorInfo->name()] = flowNodeMonitorInfo->currentStatus();
    if(debug())
    {
        qDebug() << "Flow Monitor: new Status" << flowNodeMonitorInfo->category() +"."+ flowNodeMonitorInfo->name()
                 << m_currentStatus[flowNodeMonitorInfo->category() +"."+ flowNodeMonitorInfo->name()];

        qDebug() << "Flow Monitor: All Status" << m_currentStatus;
    }

    emit currentStatusChanged(m_currentStatus);

    QVariantMap varStatus;
    varStatus[flowNodeMonitorInfo->category() +"."+ flowNodeMonitorInfo->name()] = flowNodeMonitorInfo->currentStatus();
    emit newNodeStatus(varStatus);
}

//void FlowMonitor::nodeNewEventReceived(QString eventName)
//{
//    FlowNodeMonitorInfo *flowNodeMonitorInfo = qobject_cast<FlowNodeMonitorInfo*>(sender());
//    if(!flowNodeMonitorInfo)
//        return;

//    QVariantMap varEvent;
//    varEvent["who"] = flowNodeMonitorInfo->category() +"."+ flowNodeMonitorInfo->name();
//    varEvent["when"] = QDateTime::currentDateTime() ;
//    varEvent["what"] = eventName;

//    m_eventHistory.push_back(((QVariant)varEvent));
//    while(m_eventHistory.count() > maxEventHistory())
//        m_eventHistory.pop_front();
//    if(debug())
//    {
//        qDebug() << "Flow Monitor: new Event" << varEvent;
//        qDebug() << "All Events:" << m_eventHistory;
//    }
//    emit eventHistoryChanged(m_eventHistory);
//    emit newEvent(varEvent);
//}

void FlowMonitor::nodeNewAlertReceived(int severity,QString alert)
{
    FlowNodeMonitorInfo *flowNodeMonitorInfo = qobject_cast<FlowNodeMonitorInfo*>(sender());
    if(!flowNodeMonitorInfo)
        return;

    QVariantMap varAlert;
    varAlert["who"] = flowNodeMonitorInfo->category() +"."+ flowNodeMonitorInfo->name();
    varAlert["when"] = QDateTime::currentDateTime() ;
    varAlert["what"] = alert;
    varAlert["severity"] = severity;

    m_alertHistory.push_back(((QVariant)varAlert));
    while(m_alertHistory.count() > maxAlertHistory())
        m_alertHistory.pop_front();
    if(debug())
    {
        qDebug() << "Flow Monitor: new Alert" << varAlert;
        qDebug() << "All Alerts:" << m_alertHistory;
    }
    emit alertHistoryChanged(m_alertHistory);
    emit newAlert(varAlert);
}

void FlowMonitor::nodeMonitorInfoRemoved()
{
    QObject *object = sender();
    if(!object)
        return;
    nodeMonitorInfos.removeAll((FlowNodeMonitorInfo *) object);
}



//void FlowMonitor::externalSocketDisconnectedEvent()
//{
//    QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender());
//    if(!socket)
//        return;
//    if(debug())
//        qDebug() << "Flow Monitor: External Socket Disconnected:" << socket->peerAddress() << ":" << socket->peerPort();
//    m_externalSockets.removeAll(socket);
////    socket->deleteLater();
//}

//void FlowMonitor::externalSocketDeletedEVent()
//{
//    m_externalSockets.removeAll((QTcpSocket*)sender());
//}


