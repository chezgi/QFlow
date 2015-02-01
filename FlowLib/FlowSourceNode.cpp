#include "FlowSourceNode.h"

FlowSourceNode::FlowSourceNode(QObject *parent) :
    FlowNode(parent),
    m_outputPort(new FlowOutPort("output",this))
{
    setName("source");
}

FlowOutPort *FlowSourceNode::outputPort() const
{
    return m_outputPort;
}
