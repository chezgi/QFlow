#include "FlowSequentialNode.h"

FlowSequentialNode::FlowSequentialNode(QObject *parent) :
    FlowNode(parent),
    m_requestRouter(new FlowRequestRouter(this))
{
    setName("Sequential");
    m_requestRouter->setName("SequentialNode:RR");
    requestRouter()->setTtl(5);
}

void FlowSequentialNode::processRequest(FlowInPort *inPort, QVariantMap flowPacket)
{
    for(FlowPort *port:ports())
    {
        FlowOutPort *outPort = qobject_cast<FlowOutPort*>(port);
        if(!outPort)
            continue;
        requestRouter()->add(flowPacket,inPort);
        requestRouter()->setOutPortId(flowPacket,0);
        sendRequestToPort(outPort,flowPacket);
        return;
    }
}

void FlowSequentialNode::processResponse(FlowOutPort *outPort, QVariantMap flowPacket)
{
    Q_UNUSED(outPort);
//    FlowInPort *inPort = (FlowInPort*)requestRouter()->get(flowPacket);
//    if(!inPort)
//        return;
    int pastport = requestRouter()->getOutPortId(flowPacket);
    int portCounter = 0;
    for(FlowPort *port:ports())
    {
        FlowOutPort *outPort = qobject_cast<FlowOutPort*>(port);
        if(!outPort)
            continue;
        if(portCounter <= pastport)
        {
            portCounter ++;
            continue;
        }
        requestRouter()->setOutPortId(flowPacket,pastport+1);
        sendRequestToPort(outPort,flowPacket);
        return;
    }
    // the last node passed  we now reply to sender.
    FlowInPort *inPort = (FlowInPort*)requestRouter()->take(flowPacket);
    if(!inPort)
        return;
    sendResponseToPort(inPort,flowPacket);
}


