#include "FlowObject.h"

#include <QUuid>
#include <QMetaMethod>
#include <QDebug>

FlowObject::FlowObject(QObject *parent) :
    QObject(parent)
{
    m_flowGraph = nullptr;
    m_flowObjectDeleteDelay = 0;
    m_uuid =  QUuid::createUuid().toString() + "FlowObject";
    m_creationTime = QDateTime::currentDateTime();
    m_flowObjectLifeTime = 5*60; // 1 minute
    connect(&lifetimeChecker,&QTimer::timeout,this,&FlowObject::lifetimeCheckTimedout);
    lifetimeChecker.setInterval(m_flowObjectLifeTime * 1000);
    lifetimeChecker.start();
}

QVariantMap FlowObject::createFlowPacket()
{
        QVariantMap result;
        result[flowObjectIdTag()] = uuid();
        return result;
}

//static QVariantMap qpartialobject2qvariant(const QObject *object, const QStringList &includedProperties)
//{
//    QVariantMap result;
//    for(QString prop:includedProperties)
//    {
//        int pIdx = object->metaObject()->indexOfProperty(prop.toLatin1());
//        if( pIdx < 0 )
//        {
//            qDebug() << "Internal Error no such property:" << prop << " in class:" << object->metaObject()->className();
//            continue;
//        }
//        QVariant value = object->property(prop.toLatin1());
//        result[prop] = value;
//   }
//    return result;
//}

static QVariantMap qpartialobject2qvariantDescriptive(const QObject *object, const QStringList &includedProperties)
{
    QVariantMap result;
    for(QString prop:includedProperties)
    {
        int pIdx = object->metaObject()->indexOfProperty(prop.toLatin1());
        if( pIdx < 0 )
        {
            qDebug() << "Internal Error no such property:" << prop << " in class:" << object->metaObject()->className();
            continue;
        }
        QVariant value = object->property(prop.toLatin1());
        QMetaProperty metaprop = object->metaObject()->property(pIdx);
        if(metaprop.isEnumType())
        {
            QMetaEnum metaEnum = metaprop.enumerator();
            value = metaEnum.valueToKey(object->property(prop.toLatin1()).toInt());
        }
        result[prop] = value;
   }
    return result;
}


QVariantMap FlowObject::toMap()
{
    QStringList allProperties;
    {
        for (int i=0; i<metaObject()->propertyCount(); ++i)
            allProperties << metaObject()->property(i).name();
        for (int i=0; i<metaObject()->superClass()->propertyCount(); ++i)
            allProperties.removeAll(metaObject()->superClass()->property(i).name());
        allProperties.append("uuid");  // only uuid from FlowObject
    }
    QVariantMap variantMap = qpartialobject2qvariantDescriptive(this,allProperties);
    return variantMap;
    //    QJsonDocument jsonDoc = QJsonDocument::fromVariant(variantMap);
    //    flowPacket[flowObjectSerializedTag()] = jsonDoc.toJson();

}

void FlowObject::storageAddEvent()
{

}

void FlowObject::storageRemoveEvent()
{

}

QString FlowObject::objectTypeName()
{
    return metaObject()->className();
}

void FlowObject::lifetimeCheckTimedout()
{
    if(isSignalConnected(QMetaMethod::fromSignal(&FlowObject::flowObjectExpired)))
    {
        emit flowObjectExpired();
    }else
    {
        qDebug() << "Internal Error: FlowObject must be deleted by someone!!!!!!";
        deleteLater();  //
    }
}

//----------------------------------------- Q_PROPERTY
QString FlowObject::uuid() const
{
    return m_uuid;
}

QDateTime FlowObject::creationTime() const
{
    return m_creationTime;
}

int FlowObject::flowObjectLifeTime() const
{
    return m_flowObjectLifeTime;
}

int FlowObject::flowObjectDeleteDelay() const
{
    return m_flowObjectDeleteDelay;
}

void FlowObject::setCreationTime(QDateTime arg)
{
    if (m_creationTime != arg) {
        m_creationTime = arg;
        emit creationTimeChanged(arg);
    }
}

void FlowObject::setFlowObjectLifeTime(int arg)
{
    if (m_flowObjectLifeTime != arg) {
        m_flowObjectLifeTime = arg;
        if(m_flowObjectLifeTime == 0)
        {
            lifetimeChecker.stop();
        }
        else
        {
            lifetimeChecker.setInterval(m_flowObjectLifeTime * 1000);
            if(!lifetimeChecker.isActive())
                lifetimeChecker.start();
        }
        emit flowObjectLifeTimeChanged(arg);
    }
}

void FlowObject::setFlowObjectDeleteDelay(int arg)
{
    if (m_flowObjectDeleteDelay != arg) {
        m_flowObjectDeleteDelay = arg;
        emit flowObjectDeleteDelayChanged(arg);
    }
}
