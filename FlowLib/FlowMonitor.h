#ifndef FLOWMONITOR_H
#define FLOWMONITOR_H

#include <QObject>
#include <QTcpSocket>
#include <QList>
#include <QTcpServer>

/*
 * status: property status
 * alerts: custom messages
 * events: signals emited
 *
 * total:
 *     eventHistory: [ newEvent,....]
 *     alertHistory: [ newAlert,....]
 *     currentStatus: {"cat.name":{...},....}
 *
 *  changes:
 *     newEvent: {"who":cat.name,"when":date,"what":msg}
 *     newAlert: {"who":cat.name,"when":date,"what":msg;"severity":10}
 *     newNodeStatus: { "cat.name":{ "propName":"propValue",...}}
 */
class FlowNode;
class FlowNodeMonitorInfo;
class FlowMonitor : public QObject
{
    Q_OBJECT
//    Q_PROPERTY(int maxEventHistory READ maxEventHistory WRITE setMaxEventHistory NOTIFY maxEventHistoryChanged)
    Q_PROPERTY(int maxAlertHistory READ maxAlertHistory WRITE setMaxAlertHistory NOTIFY maxAlertHistoryChanged)
    Q_PROPERTY(bool debug READ debug WRITE setDebug NOTIFY debugChanged)

//    Q_PROPERTY(QVariantList eventHistory READ eventHistory NOTIFY eventHistoryChanged)
    Q_PROPERTY(QVariantList alertHistory READ alertHistory NOTIFY alertHistoryChanged)
    Q_PROPERTY(QVariantMap currentStatus READ currentStatus NOTIFY currentStatusChanged)

public:
    explicit FlowMonitor(QObject *parent = 0);

    Q_INVOKABLE void registerNode(FlowNode *node);
    Q_INVOKABLE void unregisterNode(FlowNode *node);

signals:
//    void newEvent(QVariantMap);  // only one event
    void newAlert(QVariantMap);  // only one alert
    void newNodeStatus(QVariantMap); // only one status

private slots:
    void nodeMonitorInfoRemoved();
    void nodeCurrentStatusChanged();
//    void nodeNewEventReceived(QString eventName);
    void nodeNewAlertReceived(int severity, QString alert);

private:
    QList<FlowNodeMonitorInfo *> nodeMonitorInfos;

    //---------------------------------------- Q_PROPERTY
public:
    bool debug() const
    {
        return m_debug;
    }

//    int maxEventHistory() const;
//
//    QVariantList eventHistory() const;

    QVariantMap currentStatus() const
    {
        return m_currentStatus;
    }

    QVariantList alertHistory() const
    {
        return m_alertHistory;
    }

    int maxAlertHistory() const
    {
        return m_maxAlertHistory;
    }

signals:
    void debugChanged(bool arg);

//    void maxEventHistoryChanged(int arg);

//    void eventHistoryChanged(QVariantList arg);

    void currentStatusChanged(QVariantMap arg);

    void alertHistoryChanged(QVariantList arg);

    void maxAlertHistoryChanged(int arg);

public slots:
    void setDebug(bool arg)
    {
        if (m_debug != arg) {
            m_debug = arg;
            emit debugChanged(arg);
        }
    }

//    void setMaxEventHistory(int arg);

    void setMaxAlertHistory(int arg)
    {
        if (m_maxAlertHistory != arg) {
            m_maxAlertHistory = arg;
            while(m_alertHistory.count() > m_maxAlertHistory)
                m_alertHistory.pop_front();
            emit maxAlertHistoryChanged(arg);
        }
    }

private:
    bool m_debug;
//    int m_maxEventHistory;
//    QVariantList m_eventHistory;
    QVariantMap m_currentStatus;
    QVariantList m_alertHistory;
    int m_maxAlertHistory;
};

#endif // FLOWMONITOR_H
