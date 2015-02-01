#include "FlowRelayNode.h"

FlowRelayNode::FlowRelayNode(QObject *parent) :
    FlowNode(parent)
{
    setName("relay");
}

void FlowRelayNode::addRelay(FlowInPort *in, FlowOutPort *out)
{
    Q_ASSERT(in);
    Q_ASSERT(out);
    inToOut.insert(in,out);
    outToIn.insert(out,in);
}

void FlowRelayNode::removeRelay(FlowInPort *in, FlowOutPort *out)
{
    inToOut.remove(in);
    outToIn.remove(out);
}

void FlowRelayNode::processRequest(FlowInPort *inPort, QVariantMap flowPacket)
{
    if(inToOut.contains(inPort))
        sendRequestToPort(inToOut.value(inPort),flowPacket);
}

void FlowRelayNode::processResponse(FlowOutPort *outPort, QVariantMap flowPacket)
{
    if(outToIn.contains(outPort))
        sendResponseToPort(outToIn.value(outPort),flowPacket);
}
