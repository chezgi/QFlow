#include "FlowInOutNode.h"

FlowInOutNode::FlowInOutNode(QObject *parent) :
    FlowNode(parent),
    m_inputPort(new FlowInPort("input",this)),
    m_outputPort(new FlowOutPort("output",this))
{
    setName("inout");
}

FlowInPort *FlowInOutNode::inputPort() const
{
    return m_inputPort;
}

FlowOutPort *FlowInOutNode::outputPort() const
{
    return m_outputPort;
}


FlowInNode::FlowInNode(QObject *parent) :
    FlowNode(parent),
    m_inputPort(new FlowInPort("input",this))
{
    setName("in");
}

FlowInPort *FlowInNode::inputPort() const
{
    return m_inputPort;
}


FlowOutNode::FlowOutNode(QObject *parent) :
    FlowNode(parent),
    m_outputPort(new FlowOutPort("output",this))
{
    setName("out");
}

FlowOutPort *FlowOutNode::outputPort() const
{
    return m_outputPort;
}
