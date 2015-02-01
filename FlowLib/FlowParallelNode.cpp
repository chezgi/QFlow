#include "FlowParallelNode.h"

FlowParallelNode::FlowParallelNode(QObject *parent) :
    FlowNode(parent),
    m_requestRouter(new FlowRequestRouter(this))
{
    setName("parallel");
    m_requestRouter->setName("ParallelNode:RR");
    m_replyWhenCompleted = true;
    requestRouter()->setTtl(5);
}

void FlowParallelNode::processRequest(FlowInPort *inPort, QVariantMap flowPacket)
{
    requestRouter()->add(flowPacket,inPort);
    int sendDone = 0;
    for(FlowPort *port:ports())
    {
        FlowOutPort *outPort = qobject_cast<FlowOutPort*>(port);
        if(!outPort)
            continue;
        sendRequestToPort(outPort,flowPacket);
        sendDone++;
    }
    if(sendDone>0)
    {
        if(replyWhenCompleted())
            requestRouter()->setCount(flowPacket,sendDone);
        else
            requestRouter()->setCount(flowPacket,1);
    }
}

void FlowParallelNode::processResponse(FlowOutPort *outPort, QVariantMap flowPacket)
{
    Q_UNUSED(outPort);
    FlowInPort *inPort = (FlowInPort*)requestRouter()->get(flowPacket);
    if(!inPort)
        return;
    int count = requestRouter()->getCount(flowPacket);
    if(count > 0)
        requestRouter()->setCount(flowPacket,count-1);
    if(count == 1)
    {
        requestRouter()->take(flowPacket);
        sendResponseToPort(inPort,flowPacket);
    }
}

//------------------------------------------------ Q_PROPERTY
FlowRequestRouter *FlowParallelNode::requestRouter() const
{
    return m_requestRouter;
}

bool FlowParallelNode::replyWhenCompleted() const
{
    return m_replyWhenCompleted;
}

void FlowParallelNode::setReplyWhenCompleted(bool arg)
{
    if (m_replyWhenCompleted != arg) {
        m_replyWhenCompleted = arg;
        emit replyWhenCompletedChanged(arg);
    }
}
