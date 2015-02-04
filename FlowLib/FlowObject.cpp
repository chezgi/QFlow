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
