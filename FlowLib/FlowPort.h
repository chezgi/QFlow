#ifndef FLOWPORT_H
#define FLOWPORT_H

#include <QtCore/QObject>
#include <QtCore/QVariant>
#include <QtCore/QDateTime>
#include <QtQml/QQmlListProperty>

#include "FlowRequestRouter.h"
#include "FlowObject.h"

class FlowNode;
class FlowGraph;
class FlowPort : public QObject
{
    friend class FlowNode;
//    friend class FlowInPort;
    friend class FlowOutPort;
    Q_OBJECT
    Q_ENUMS(Direction)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString uuid READ uuid CONSTANT)
    Q_PROPERTY(QString description READ description WRITE setDescription NOTIFY descriptionChanged)
    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged)
    Q_PROPERTY(bool debug READ debug WRITE setDebug NOTIFY debugChanged)
    Q_PROPERTY(Direction direction READ direction)

    // statistics
    Q_PROPERTY(quint64 portSendCount READ portSendCount NOTIFY portSendCountChanged)
    Q_PROPERTY(quint64 portRecvCount READ portRecvCount NOTIFY portRecvCountChanged)
    Q_PROPERTY(quint64 portDropCount READ portDropCount NOTIFY portDropCountChanged)
public:
    enum Direction{Output,Input};
    explicit FlowPort(QString portName="",Direction dir = Output,QObject *parent = 0);

    Q_INVOKABLE static QString getFlowId(QVariantMap flowPacket);
    Q_INVOKABLE static QVariantList getFlowIdStack(QVariantMap flowPacket);

    FlowGraph *flowGraph();

protected:
    QString nodeName();
    QString m_cachedNodeName;
    FlowGraph *m_cachedGraph;

    void addSendCount();
    void addRecvCount();
    void addDropCount();

signals:
//    void _toWire(QVariantMap flowPacket);
    void _toNode(QVariantMap flowPacket);

    //------------------------------------- properties
public:
    Direction direction() const;

    bool enabled() const;

    QString name() const;

    QString description() const;

    bool debug() const;

    quint64 portSendCount() const;

    quint64 portRecvCount() const;

    quint64 portDropCount() const;

    QString uuid() const;

signals:
    void enabledChanged(bool arg);

    void nameChanged(QString arg);

    void descriptionChanged(QString arg);

    void debugChanged(bool arg);

    void portSendCountChanged(quint64 arg);

    void portRecvCountChanged(quint64 arg);

    void portDropCountChanged(quint64 arg);

public slots:
    void setEnabled(bool arg);

    void setName(QString arg);

    void setDescription(QString arg);

    void setDebug(bool arg);

private:
    QString m_name;
    bool m_enabled;
    Direction m_direction;
    QString m_description;
    bool m_debug;
    quint64 m_portSendCount;
    quint64 m_portRecvCount;
    quint64 m_portDropCount;
    QString m_uuid;
};

class FlowOutPort;
//------------------------------------- FlowInPort
class FlowInPort : public FlowPort
{
    friend class FlowOutPort;
    friend class FlowNode;
    Q_OBJECT
    Q_PROPERTY(FlowRequestRouter* requestRouter READ requestRouter)
public:
    explicit FlowInPort(QString portName="",QObject *parent = 0);

    FlowRequestRouter* requestRouter() const;
    Q_INVOKABLE bool hasPreviousRoute(QVariantMap flowPacket);

private:
    void _requestFromWire(FlowOutPort *senderPort,QVariantMap flowPacket);
    bool _responseToWire(QVariantMap flowPacket);  // can be called from FlowNode

private:
    FlowRequestRouter* m_requestRouter;
};


//------------------------------------- FlowOutPort
class FlowOutPort : public FlowPort
{
    friend class FlowInPort;
    friend class FlowNode;
    Q_OBJECT    
    Q_PROPERTY(FlowInPort* next READ next WRITE setNext NOTIFY nextChanged)
    Q_PROPERTY(QList<FlowInPort*> multicast READ multicast WRITE setMulticast NOTIFY multicastChanged)
public:
    explicit FlowOutPort(QString portName="",QObject *parent = 0);

private slots:
    void _inportDeleted();

private:
    void _responseFromWire(QVariantMap flowPacket);
    bool _requestToWire(QVariantMap flowPacket);  // can be called from FlowNode

    void popFlowIdStack(QVariantMap &flowPacket);
    void pushFlowIdStack(QVariantMap &flowPacket);

    //------------------------------------- properties
public:
    QList<FlowInPort*> multicast() const;

    FlowInPort* next() const;

public slots:
    void setMulticast(QList<FlowInPort*> arg);

    void setNext(FlowInPort* arg);

signals:
    void multicastChanged(QList<FlowInPort*> arg);

    void nextChanged(FlowInPort* arg);

private:
    QList<FlowInPort *> m_multicast;

    FlowInPort* m_next;
};

#endif // FLOWPORT_H
