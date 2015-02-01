#ifndef FLOWSEQUENTIALNODE_H
#define FLOWSEQUENTIALNODE_H

#include "FlowNode.h"
class FlowSequentialNode : public FlowNode
{
    Q_OBJECT
    Q_PROPERTY(FlowRequestRouter* requestRouter READ requestRouter)
public:
    explicit FlowSequentialNode(QObject *parent = 0);

private slots:
    void processRequest(FlowInPort *inPort, QVariantMap flowPacket);
    void processResponse(FlowOutPort *outPort, QVariantMap flowPacket);

    //------------------------------------------------ Q_PROPERTY
public:
    FlowRequestRouter* requestRouter() const;

signals:
public slots:
private:
    FlowRequestRouter* m_requestRouter;

};
#endif // FLOWSEQUENTIALNODE_H
