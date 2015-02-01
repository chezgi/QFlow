#include "FlowObjectStorage.h"
#include "FlowObject.h"

#include <QDebug>

FlowObjectStorage::FlowObjectStorage(QObject *parent) :
    QObject(parent)
{
    m_debug = false;
    cleanupTimer.setInterval(5 * 1000); // check every 5 seconds
    connect(&cleanupTimer,&QTimer::timeout,this,&FlowObjectStorage::cleanupPackets);
    cleanupTimer.start();
}

FlowObject *FlowObjectStorage::getByPacket(const QVariantMap &flowPacket)
{
    return get(flowPacket.value(FlowObject::flowObjectIdTag()).toString());
}

FlowObject *FlowObjectStorage::get(const QString &uuid)
{
    if(debug())
    {
        if(flowObjectsByHash.contains(uuid))
            qDebug() << "[Storage] Get Object Success: " << uuid;
        else
            qDebug() << "[Storage] Get Object Failure: " << uuid;
    }
    return flowObjectsByHash.value(uuid,nullptr);
}

void FlowObjectStorage::add(FlowObject *flowObject)
{
    if(!flowObject)
        return;
    flowObject->setFlowGraph((FlowGraph*)parent());
    flowObject->setParent(this);
    connect(flowObject,&FlowObject::flowObjectExpired,this,&FlowObjectStorage::objectExpiredEvent);
    connect(flowObject,&QObject::destroyed,this,&FlowObjectStorage::objectRemovedEvent);
    flowObjectsByHash[flowObject->uuid()] = flowObject;
    flowObject->storageAddEvent();
    emit objectAdded(flowObject->uuid());
    if(debug())
        qDebug() << "[Storage] Object Added: " << flowObject->uuid();

}

void FlowObjectStorage::removeByPacket(const QVariantMap &flowPacket)
{
    return remove(flowPacket.value(FlowObject::flowObjectIdTag()).toString());
}

bool FlowObjectStorage::debug() const
{
    return m_debug;
}

void FlowObjectStorage::remove(const QString &uuid)
{    
    FlowObject *flowObject = flowObjectsByHash.take(uuid);
    if(!flowObject)
        return;

    emit objectRemoved(flowObject->uuid());

    flowObject->storageRemoveEvent();
    if(flowObject->flowObjectDeleteDelay() <= 0)
    {
        flowObject->deleteLater();
        if(debug())
            qDebug() << "[Storage] Remove Object InTime: " << uuid;
    }else
    {
        if(debug())
            qDebug() << "[Storage] Remove Object Later: " << uuid;
        waitingForRemoveList.insert(QDateTime::currentDateTime().addSecs(flowObject->flowObjectDeleteDelay()),flowObject);
    }
}

void FlowObjectStorage::cleanupPackets()
{
    // do cleanup old packets.
    int removedCount = 0;
    QDateTime now =QDateTime::currentDateTime();
    QMutableMapIterator<QDateTime,QObject *> mit(waitingForRemoveList);
    while(mit.hasNext())
    {
        mit.next();
        if(now > mit.key())
        {
            mit.value()->deleteLater();
            mit.remove();
            removedCount++;
        }
    }
    if(debug())
        qDebug() << "flow objects after cleanup:" <<  flowObjectsByHash.count() << waitingForRemoveList.count() << removedCount ;
}

void FlowObjectStorage::objectExpiredEvent()
{
    if(debug())
        qDebug() << "[Storage] Object Expire Event";

    QObject *robject = sender();
    {
        QMutableHashIterator<QString,FlowObject *> hit(flowObjectsByHash);
        while(hit.hasNext())
        {
            hit.next();
            if(hit.value() == robject)
            {
                if(debug())
                    qDebug() << "[Storage] Object Expired";
                FlowObject *flowObject = hit.value();
                emit objectRemoved(flowObject->uuid());
                flowObject->storageRemoveEvent();
                hit.remove();                
            }
        }
    }
    {
        QMutableMapIterator<QDateTime,QObject *> mit(waitingForRemoveList);
        while(mit.hasNext())
        {
            mit.next();
            if(mit.value() == robject)
                mit.remove();
        }
    }
    robject->deleteLater();
}

void FlowObjectStorage::objectRemovedEvent()
{
//    if(debug())
    QObject *robject = sender();
    {
        QMutableHashIterator<QString,FlowObject *> hit(flowObjectsByHash);
        while(hit.hasNext())
        {
            hit.next();
            FlowObject *flowObject = hit.value();
            if(flowObject == robject)
            {
                qDebug() << "[Storage] Object Remove Event outside of storage!!!";
                emit objectRemoved(flowObject->uuid());
                hit.remove();
            }
        }
    }
    {
        QMutableMapIterator<QDateTime,QObject *> mit(waitingForRemoveList);
        while(mit.hasNext())
        {
            mit.next();
            if(mit.value() == robject)
                mit.remove();
        }
    }
}

void FlowObjectStorage::setDebug(bool arg)
{
    if (m_debug != arg) {
        m_debug = arg;
        emit debugChanged(arg);
    }
}
