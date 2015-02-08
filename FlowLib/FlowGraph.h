#ifndef FLOWGRAPH_H
#define FLOWGRAPH_H

#include <QtCore/QObject>
#include <QtQml/QQmlListProperty>
#include <QtQml/QQmlParserStatus>
#include <QTimer>
#include <QDateTime>

/*
   TODO: tcpserver service: register for new tcpserver socket with portname, and implement functions like: newClient,closedClient,newRequest,...
   TODO: can register for dataproviders and search for specific data with datapath,datafilter,datasearch,... like AMI DataGet API

 */



class FlowNode;
class FlowInPort;
class FlowObject;
class FlowObjectStorage;
class FlowMonitor;
class FlowRestService;
class FlowJsonStorage;

class FlowGraph : public QObject, public QQmlParserStatus
{
    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString uuid READ uuid CONSTANT)
    Q_PROPERTY(QString description READ description WRITE setDescription NOTIFY descriptionChanged)
    Q_PROPERTY(bool debug READ debug WRITE setDebug NOTIFY debugChanged)
    Q_PROPERTY(QDateTime creationTime READ creationTime CONSTANT)
    Q_PROPERTY(FlowMonitor* monitor READ monitor WRITE setMonitor NOTIFY monitorChanged)
    Q_PROPERTY(FlowObjectStorage* objectStorage READ objectStorage CONSTANT)
    Q_PROPERTY(FlowRestService* restService READ restService  CONSTANT)
    Q_PROPERTY(FlowJsonStorage* jsonStorage READ jsonStorage CONSTANT)
    Q_PROPERTY(QObject* config READ config WRITE setConfig NOTIFY configChanged)
    Q_PROPERTY(bool traceEnabled READ traceEnabled WRITE setTraceEnabled NOTIFY traceEnabledChanged)

    Q_PROPERTY(QQmlListProperty<QObject> data READ data)
    Q_CLASSINFO("DefaultProperty", "data")

public:
    explicit FlowGraph(QObject *parent = 0);
    QList<FlowNode *> nodes();

    Q_INVOKABLE FlowNode * findNode(QString nodeName);
    Q_INVOKABLE FlowInPort * findPort(QString nodeName,QString portName);

    Q_INVOKABLE QVariantMap topologyMap();

    // QQmlParserStatus interface
public:
    void classBegin(){}
    void componentComplete();

protected slots:
    virtual void initializeGraph();

public slots:
    void traceRequest(QString node, QString port, QVariantMap flowPacket);
    void traceResponse(QString node, QString port, QVariantMap flowPacket);

signals:
    void newTrace(QVariantMap trace);

private:
    bool m_defaultLoggerIsUsed;
    bool m_defaultMonitorIsUsed;

    QVariantMap createTrace(bool isRequest,QString node, QString port,QVariantMap flowPacket);
    static quint64 traceId;

    //------------------------------------- properties
public:
    QQmlListProperty<QObject> data(){return QQmlListProperty<QObject>(this, m_data);}

    QString name() const
    {
        return m_name;
    }

    QString description() const
    {
        return m_description;
    }

    FlowObjectStorage* objectStorage() const
    {
        return m_objectStorage;
    }

    bool debug() const
    {
        return m_debug;
    }

    FlowMonitor* monitor() const
    {
        return m_monitor;
    }

    FlowRestService* restService() const
    {
        return m_restService;
    }

    QDateTime creationTime() const
    {
        return m_creationTime;
    }

    QString uuid() const
    {
        return m_uuid;
    }

    FlowJsonStorage* jsonStorage() const
    {
        return m_jsonStorage;
    }

    QObject* config() const
    {
        return m_config;
    }

    bool traceEnabled() const
    {
        return m_traceEnabled;
    }

signals:
    void nameChanged(QString arg);

    void descriptionChanged(QString arg);

    void debugChanged(bool arg);

    void monitorChanged(FlowMonitor* arg);

    void configChanged(QObject* arg);

    void traceEnabledChanged(bool arg);

public slots:
    void setName(QString arg)
    {
        if (m_name != arg) {
            m_name = arg;
            emit nameChanged(arg);
        }
    }

    void setDescription(QString arg)
    {
        if (m_description != arg) {
            m_description = arg;
            emit descriptionChanged(arg);
        }
    }

    void setDebug(bool arg)
    {
        if (m_debug != arg) {
            m_debug = arg;
            emit debugChanged(arg);
        }
    }


    void setMonitor(FlowMonitor* arg)
    {
        if (m_monitor != arg) {
            if(m_defaultMonitorIsUsed)
            {
                m_defaultMonitorIsUsed = false;
            }
            m_monitor = arg;
            emit monitorChanged(arg);
        }
    }

    void setConfig(QObject* arg)
    {
        if (m_config != arg) {
            m_config = arg;
            emit configChanged(arg);
        }
    }

    void setTraceEnabled(bool arg)
    {
        if (m_traceEnabled != arg) {
            m_traceEnabled = arg;
            emit traceEnabledChanged(arg);
        }
    }

private:
    QList<QObject *> m_data;
    QString m_name;
    QString m_description;
    FlowObjectStorage *m_objectStorage;
    bool m_debug;
    FlowMonitor* m_monitor;
    FlowRestService* m_restService;
    QDateTime m_creationTime;
    QString m_uuid;
    FlowJsonStorage* m_jsonStorage;
    QObject* m_config;
    bool m_traceEnabled;
};


//template<class T>
//T *FlowGraph::createFlowObject()
//{
//    T *flowObject = new T(this);
//    addFlowObject(flowObject);
//    return flowObject;
//}

#endif // FLOWGRAPH_H
