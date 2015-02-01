#ifndef FLOWRELAYNODE_H
#define FLOWRELAYNODE_H

#include "FlowNode.h"
class FlowRelayNode : public FlowNode
{
    Q_OBJECT
public:
    explicit FlowRelayNode(QObject *parent = 0);

    Q_INVOKABLE void addRelay(FlowInPort *in,FlowOutPort*out);
    Q_INVOKABLE void removeRelay(FlowInPort *in,FlowOutPort*out);

private:
    QMap<FlowInPort*,FlowOutPort*> inToOut;
    QMap<FlowOutPort*,FlowInPort*> outToIn;

    // FlowNode interface
protected slots:
    void processRequest(FlowInPort *inPort, QVariantMap flowPacket);
    void processResponse(FlowOutPort *outPort, QVariantMap flowPacket);
};

#endif // FLOWRELAYNODE_H
