#ifndef FLOWNODEMONITORINFO_H
#define FLOWNODEMONITORINFO_H

#include <QObject>
#include <QList>
#include <QMap>
#include <QVariant>
#include <QVariantList>
#include <QStringList>
#include <QMetaProperty>
#include <QTimer>

/*
 *
 *  will be used inside of FlowNode and must be setup for working. [this must be done at initializeNode]
 * FIXME: monitoredCounters must be cleanuped
 *   node->setMonitored(true);
 *   node->monitorInfo()->setCategory("deviceQueue");
 *   node->monitorInfo()->setName("device1");
 *   node->monitorInfo()->setMonitoredProperties(...);
 *
 */


class FlowNode;
class FlowNodeMonitorInfo : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString category READ category WRITE setCategory NOTIFY categoryChanged)

    Q_PROPERTY(bool debug READ debug WRITE setDebug NOTIFY debugChanged)
    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged)

    // monitored counters are like monitored properties but will be reset in some interval [daily] and can be used for agreegation.
    Q_PROPERTY(QStringList monitoredCounters READ monitoredCounters WRITE setMonitoredCounters NOTIFY monitoredCountersChanged)
    Q_PROPERTY(QStringList monitoredProperties READ monitoredProperties WRITE setMonitoredProperties NOTIFY monitoredPropertiesChanged)
//    Q_PROPERTY(QStringList monitoredSignals READ monitoredSignals WRITE setMonitoredSignals NOTIFY monitoredSignalsChanged)
    // statistics property of node will be added to Monitored Properties and changes will be propagated to clients. NOTE: No need to this


    Q_PROPERTY(QVariantMap currentStatus READ currentStatus NOTIFY currentStatusChanged)  // propname,propvalue

public:    
    explicit FlowNodeMonitorInfo(FlowNode *parent);

    Q_INVOKABLE void alert(int severity,QString msg);

signals:
    void newEvent(QString eventName);
    void newAlert(int severity,QString alert);

private:
    QMap<int,QString> m_cachedSignals;
    QMap<int,QMetaProperty> m_cachedProperties;
    QTimer counterResetTimer;

private slots:
    void monitorPropertyChanged();
    void monitorSignalEmited();
    void initializeStatus(QMetaProperty metaProperty);

    void addMonitoredProperty(QString propname);
    void removeMonitoredProperty(QString propname);
    void resetCounterProperties();

//---------------------------------------------- Q_PROPERTY
public:
    QStringList monitoredProperties() const;

//    QStringList monitoredSignals() const;

    QVariantMap currentStatus() const;

    bool debug() const;

    QString name() const;

    QString category() const;

    QStringList monitoredCounters() const;

    bool enabled() const;

signals:
    void monitoredPropertiesChanged(QStringList arg);

//    void monitoredSignalsChanged(QStringList arg);

    void currentStatusChanged(QVariantMap arg);

    void debugChanged(bool arg);

    void nameChanged(QString arg);

    void categoryChanged(QString arg);

    void monitoredCountersChanged(QStringList arg);

    void enabledChanged(bool arg);

public slots:
    void setMonitoredProperties(QStringList arg);

//    void setMonitoredSignals(QStringList arg);

    void setDebug(bool arg);

    void setName(QString arg);

    void setCategory(QString arg);

    void setMonitoredCounters(QStringList arg);

    void setEnabled(bool arg);

private:
    QStringList m_monitoredProperties;
//    QStringList m_monitoredSignals;
    QVariantMap m_currentStatus;
    bool m_debug;
    QString m_name;
    QString m_category;
    QStringList m_monitoredCounters;
    bool m_enabled;
};

#endif // FLOWNODEMONITORINFO_H
