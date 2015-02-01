#ifndef FLOWNODE_H
#define FLOWNODE_H

#include <QtCore/QObject>
#include <QtCore/QList>
#include <QtQml/QQmlListProperty>
#include <QStringList>

#include "FlowPort.h"
#include "FlowObject.h"
#include "FlowGraph.h"
#include "FlowObjectStorage.h"
#include "FlowJsonStorage.h"
#include "FlowLogger.h"
#include "FlowMonitor.h"
#include "FlowService.h"
#include "FlowNodeMonitorInfo.h"
#include "FlowCommunicationUtils.h"
#include "FlowNodeServiceInfo.h"

class FlowNode : public QObject, public QQmlParserStatus
{
    friend class FlowPort;
    friend class FlowInPort;
    friend class FlowOutPort;
    friend class FlowService;
    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString uuid READ uuid CONSTANT)
    Q_PROPERTY(QString description READ description WRITE setDescription NOTIFY descriptionChanged)

//    Q_PROPERTY(bool dontTrackSubNodes READ dontTrackSubNodes WRITE setDontTrackSubNodes NOTIFY dontTrackSubNodesChanged) //used for nodes have some temporary subnodes
    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged)
    Q_PROPERTY(bool debug READ debug WRITE setDebug NOTIFY debugChanged)

    Q_PROPERTY(FlowNodeMonitorInfo* monitorInfo READ monitorInfo CONSTANT)
    Q_PROPERTY(FlowNodeServiceInfo* serviceInfo READ serviceInfo CONSTANT)

    Q_PROPERTY(quint64 nodeSendRequestCount READ nodeSendRequestCount NOTIFY nodeSendRequestCountChanged)
    Q_PROPERTY(quint64 nodeSendResponseCount READ nodeSendResponseCount NOTIFY nodeSendResponseCountChanged)
    Q_PROPERTY(quint64 nodeRecvRequestCount READ nodeRecvRequestCount NOTIFY nodeRecvRequestCountChanged)
    Q_PROPERTY(quint64 nodeRecvResponseCount READ nodeRecvResponseCount NOTIFY noedRecvResponseCountChanged)
    Q_PROPERTY(quint64 nodeDropCount READ nodeDropCount NOTIFY nodeDropCountChanged)

    Q_PROPERTY(QQmlListProperty<QObject> data READ data)
    Q_CLASSINFO("DefaultProperty", "data")

public:
    explicit FlowNode(QObject *parent = 0);
    ~FlowNode();

    // NOTE: all calls to this function will be failed when called from  CONSTRUCTOR. calls from constructor must be deleayed for later. or it must be done at initializer.
    Q_INVOKABLE FlowGraph * flowGraph();
    QList<FlowPort *> ports();
    Q_INVOKABLE FlowInPort *findPort(QString portName);

    Q_INVOKABLE bool sendResponseToPort(FlowInPort *port,QVariantMap flowPacket);
    Q_INVOKABLE bool sendRequestToPort(FlowOutPort *port,QVariantMap flowPacket);

    static void setupRemotePacket(QVariantMap &flowPacket,QString source,QString destination);// function with referenced param(&) can't be Q_INVOKABLE
    static void setupRemoteDevicePacket(QVariantMap &flowPacket,QString destinatinDevice);
//    Q_INVOKABLE static QString getRemoteOperation(QVariantMap flowPacket);
    Q_INVOKABLE static QString getRemoteSource(QVariantMap flowPacket);
    Q_INVOKABLE static QString getRemoteDestination(QVariantMap flowPacket);
    Q_INVOKABLE static QString getRemoteDestinationDevice(QVariantMap flowPacket);

    // QQmlParserStatus interface
public:
    void classBegin();
    void componentComplete();

private:
    QString graphName();
    FlowGraph *m_cachedFlowGraph;
    void addDropCount();

private slots:
    void _fromPort(QVariantMap flowPacket);
    void _fromServiceCenter(QString clientId,QVariantMap requestData);
    void autoCheckPorts();  // for qml ports and also check for parents

signals:
    void requestReceived(FlowInPort *port,QVariantMap flowPacket);
    void responseReceived(FlowOutPort *port,QVariantMap flowPacket);
    void serviceRequestReceived(QString clientId, QVariantMap requestData);

protected slots:
    virtual void initializeNode();
    virtual void processRequest(FlowInPort *inPort, QVariantMap flowPacket);
    virtual void processResponse(FlowOutPort *outPort, QVariantMap flowPacket);
    virtual void processServiceRequest(QString clientId,QVariantMap requestData);

private:
    static void append_data(QQmlListProperty<QObject> *list, QObject *d);

    //------------------------------------- properties
public:
    QQmlListProperty<QObject> data(){ return QQmlListProperty<QObject>(this, 0, &FlowNode::append_data, nullptr, nullptr, nullptr);}

    bool enabled() const;

    bool debug() const;

    QString name() const;

    QString description() const;

    FlowNodeMonitorInfo* monitorInfo() const;

    quint64 nodeSendRequestCount() const;

    quint64 nodeSendResponseCount() const;

    quint64 nodeRecvRequestCount() const;

    quint64 nodeRecvResponseCount() const;

    quint64 nodeDropCount() const;

    QString uuid() const;

    FlowNodeServiceInfo* serviceInfo() const
    {
        return m_serviceInfo;
    }

signals:
    void enabledChanged(bool arg);

    void debugChanged(bool arg);

    void nameChanged(QString arg);

    void descriptionChanged(QString arg);

    void nodeSendRequestCountChanged(quint64 arg);

    void nodeSendResponseCountChanged(quint64 arg);

    void nodeRecvRequestCountChanged(quint64 arg);

    void noedRecvResponseCountChanged(quint64 arg);

    void nodeDropCountChanged(quint64 arg);

public slots:
    void setEnabled(bool arg);

    void setDebug(bool arg);

    void setName(QString arg);

    void setDescription(QString arg);

private:
    QList<QObject *> m_data;
    bool m_enabled;
    bool m_debug;
    QString m_name;
    QString m_description;
    FlowNodeMonitorInfo* m_monitorInfo;
    quint64 m_nodeSendRequestCount;
    quint64 m_nodeSendResponseCount;
    quint64 m_nodeRecvRequestCount;
    quint64 m_nodeRecvResponseCount;
    quint64 m_nodeDropCount;

    QString m_uuid;
    FlowNodeServiceInfo* m_serviceInfo;
};


#endif // FLOWNODE_H
