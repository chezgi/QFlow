#include "FlowNodeMonitorInfo.h"
#include "FlowNode.h"
#include <QDebug>
#include <QDateTime>

FlowNodeMonitorInfo::FlowNodeMonitorInfo(FlowNode *parent) :
    QObject((QObject *)parent)
{
    m_debug = false;
    m_enabled = false;
    connect(&counterResetTimer,SIGNAL(timeout()),SLOT(resetCounterProperties()));
    QDateTime midnight;
    midnight = QDateTime::currentDateTime().addDays(1);
    midnight.setTime(QTime(0,5));
    counterResetTimer.setInterval(QDateTime::currentDateTime().msecsTo(midnight));
    counterResetTimer.start();
}

void FlowNodeMonitorInfo::alert(int severity, QString msg)
{
    emit newAlert(severity,msg);
}

void FlowNodeMonitorInfo::monitorPropertyChanged()
{
    int signalIndex = senderSignalIndex();
    if(signalIndex<0)
    {
        qDebug() << "Invalid signal of property change emitted!!!!" << category() << name() << ((FlowNode *)parent())->name();
        return;
    }
    if(!m_cachedProperties.contains(signalIndex))
    {
        QMetaProperty metaProperty;
        for(QString propname:monitoredProperties() + monitoredCounters())
        {
            metaProperty = parent()->metaObject()->property(parent()->metaObject()->indexOfProperty(propname.toLatin1()));
            if(metaProperty.notifySignalIndex() == signalIndex)
            {
                break;
            }
        }
        if(!metaProperty.isValid())
        {
            qDebug() << "No Such Property Change Signal!!!! : " << category() << name() << ((FlowNode *)parent())->name();
            return;
        }
        m_cachedProperties[signalIndex] = metaProperty;
    }
    QString propname = m_cachedProperties[signalIndex].name();


    QVariant value;
    if(m_cachedProperties[signalIndex].isEnumType())
    {
        QMetaEnum metaEnum = m_cachedProperties[signalIndex].enumerator();
        value = metaEnum.valueToKey(m_cachedProperties[signalIndex].read(parent()).toInt());
    }else
    {
        value = m_cachedProperties[signalIndex].read(parent());
    }
    m_currentStatus[propname] = value;
    if(debug())
    {
        qDebug() << "FlowNode[" << ((FlowNode *)parent())->name() << "] Property Changed:" << propname << " : "<< m_currentStatus[propname];
    }
    emit currentStatusChanged(m_currentStatus);
}

void FlowNodeMonitorInfo::monitorSignalEmited()
{
    int signalIndex = senderSignalIndex();
    if(signalIndex<0)
    {
        qDebug() << "Invalid signal emitted!!!!";
        return;
    }
    if(!m_cachedSignals.contains(signalIndex))
    {
        QMetaMethod calledSignal = parent()->metaObject()->method(signalIndex);
        m_cachedSignals[signalIndex] = calledSignal.name();
    }

    emit newEvent(m_cachedSignals[signalIndex]);
}

void FlowNodeMonitorInfo::initializeStatus(QMetaProperty metaProperty)
{
    QVariant value;
    if(metaProperty.isEnumType())
    {
        QMetaEnum metaEnum = metaProperty.enumerator();
        value = metaEnum.valueToKey(metaProperty.read(parent()).toInt());
    }else
    {
        value = metaProperty.read(parent());
    }

    m_currentStatus[metaProperty.name()] = value;
    emit currentStatusChanged(m_currentStatus);
}

void FlowNodeMonitorInfo::addMonitoredProperty(QString propname)
{
    QMetaMethod slotMethod = metaObject()->method(metaObject()->indexOfMethod(QMetaObject::normalizedSignature("monitorPropertyChanged()")));
    Q_ASSERT(slotMethod.isValid());
    QMetaProperty metaProperty = parent()->metaObject()->property(parent()->metaObject()->indexOfProperty(propname.toLatin1()));
    if(!metaProperty.hasNotifySignal())
    {
        qDebug() << "Node["<< ((FlowNode *)parent())->name() <<"] Monitor Property has no notify signal:" << propname;
        return;
    }
    connect(parent(),metaProperty.notifySignal(),this,slotMethod);
    initializeStatus(metaProperty);
}

void FlowNodeMonitorInfo::removeMonitoredProperty(QString propname)
{
    QMetaMethod slotMethod = metaObject()->method(metaObject()->indexOfMethod(QMetaObject::normalizedSignature("monitorPropertyChanged()")));
    Q_ASSERT(slotMethod.isValid());
    QMetaProperty metaProperty = parent()->metaObject()->property(metaObject()->indexOfProperty(propname.toLatin1()));
    if(!metaProperty.hasNotifySignal())
    {
        qDebug() << "Node["<< ((FlowNode *)parent())->name() <<"] Monitor Property has no notify signal:" << propname;
        return;
    }
    disconnect(parent(),metaProperty.notifySignal(),this,slotMethod);
}

void FlowNodeMonitorInfo::resetCounterProperties()
{
    counterResetTimer.setInterval(24 * 60 * 60 * 1000);
    for(QString propname:m_monitoredCounters)
    {
        QMetaProperty metaProperty = parent()->metaObject()->property(parent()->metaObject()->indexOfProperty(propname.toLatin1()));
        metaProperty.write(parent(),0);
    }
}

//---------------------------------------------- Q_PROPERTY

QStringList FlowNodeMonitorInfo::monitoredProperties() const
{
    return m_monitoredProperties;
}

//QStringList FlowNodeMonitorInfo::monitoredSignals() const
//{
//    return m_monitoredSignals;
//}

QVariantMap FlowNodeMonitorInfo::currentStatus() const
{
    return m_currentStatus;
}

bool FlowNodeMonitorInfo::debug() const
{
    return m_debug;
}

QString FlowNodeMonitorInfo::name() const
{
    return m_name;
}

QString FlowNodeMonitorInfo::category() const
{
    return m_category;
}

QStringList FlowNodeMonitorInfo::monitoredCounters() const
{
    return m_monitoredCounters;
}

bool FlowNodeMonitorInfo::enabled() const
{
    return m_enabled;
}


void FlowNodeMonitorInfo::setMonitoredProperties(QStringList arg)
{
    if (m_monitoredProperties != arg) {
        // cleanup old assigned properties.
        for(QString propname:m_monitoredProperties)
        {
            removeMonitoredProperty(propname);
        }
        m_monitoredProperties = arg;
        // setup new assigned properties.
        for(QString propname:m_monitoredProperties)
        {
            addMonitoredProperty(propname);
        }
        emit monitoredPropertiesChanged(arg);
    }
}

//void FlowNodeMonitorInfo::setMonitoredSignals(QStringList arg)
//{
//    QMetaMethod slotMethod = metaObject()->method(metaObject()->indexOfMethod(QMetaObject::normalizedSignature("monitorSignalEmited()")));
//    Q_ASSERT(slotMethod.isValid());
//    if (m_monitoredSignals != arg) {
//        // cleanup old signals.
//        if(!m_monitoredSignals.isEmpty())
//        {
//            for(int i=0;i< parent()->metaObject()->methodCount();i++)
//            {
//                QMetaMethod signalMethod = parent()->metaObject()->method(i);
//                if(signalMethod.methodType() == QMetaMethod::Signal && m_monitoredSignals.contains(signalMethod.name()))
//                {
//                    disconnect(parent(),signalMethod,this,slotMethod);
//                }
//            }
//        }
//        m_monitoredSignals = arg;
//        // setup new signals.
//        if(!m_monitoredSignals.isEmpty())
//        {
//            QStringList notFoundSignals = m_monitoredSignals;
//            for(int i=0;i< parent()->metaObject()->methodCount();i++)
//            {
//                QMetaMethod signalMethod = parent()->metaObject()->method(i);
//                if(signalMethod.methodType() == QMetaMethod::Signal && m_monitoredSignals.contains(signalMethod.name()))
//                {
//                    notFoundSignals.removeAll(signalMethod.name());
//                    connect(parent(),signalMethod,this,slotMethod);
//                    if(notFoundSignals.isEmpty())
//                        break;
//                }
//            }
//            if(!notFoundSignals.isEmpty())
//                qDebug() << "Node["<< ((FlowNode *)parent())->name() <<"] signals is not defined:" << notFoundSignals;
//        }
//        emit monitoredSignalsChanged(arg);
//    }
//}

void FlowNodeMonitorInfo::setDebug(bool arg)
{
    if (m_debug != arg) {
        m_debug = arg;
        emit debugChanged(arg);
    }
}

void FlowNodeMonitorInfo::setName(QString arg)
{
    if (m_name != arg) {
        m_name = arg;
        emit nameChanged(arg);
    }
}

void FlowNodeMonitorInfo::setCategory(QString arg)
{
    if (m_category != arg) {
        m_category = arg;
        emit categoryChanged(arg);
    }
}

void FlowNodeMonitorInfo::setMonitoredCounters(QStringList arg)
{
    if (m_monitoredCounters != arg) {
        // cleanup old assigned properties.
        for(QString propname:m_monitoredCounters)
        {
            removeMonitoredProperty(propname);
        }
        m_monitoredCounters = arg;
        // setup new assigned properties.
        for(QString propname:m_monitoredCounters)
        {
            addMonitoredProperty(propname);
        }
        emit monitoredCountersChanged(arg);
    }
}

void FlowNodeMonitorInfo::setEnabled(bool arg)
{
    FlowNode * node = (FlowNode *)parent();
    Q_ASSERT_X(node,"monitorinfo","can't find parent node.");
    if (m_enabled != arg) {
        m_enabled = arg;
        if(m_enabled)
        {
            if(node->flowGraph())
                node->flowGraph()->monitor()->registerNode(node);
            else
                qDebug() << "Alert: flowGraph not valid at EnableMonitor " << node->flowGraph()->name()+":"+node->name();
        }else
        {
            if(node->flowGraph())
                node->flowGraph()->monitor()->unregisterNode(node);
        }
        emit enabledChanged(arg);
    }
}

//void FlowNodeMonitorInfo::setMonitorNodeStatistics(bool arg)
//{
//    if (m_monitorNodeStatistics != arg) {
//        m_monitorNodeStatistics = arg;
//        QStringList statProperties;
//        statProperties << "sendRequestCount" << "sendResponseCount"
//                       << "recvRequestCount" << "recvResponseCount"
//                       << "dropCount" ;
//        if(m_monitorNodeStatistics)
//        {
//            for(QString propName:statProperties)
//                addMonitoredProperty(propName);
//        }else
//        {
//            for(QString propName:statProperties)
//                removeMonitoredProperty(propName);
//        }
//        emit monitorNodeStatisticsChanged(arg);
//    }
//}
