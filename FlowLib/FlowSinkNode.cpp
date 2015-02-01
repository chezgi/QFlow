#include "FlowSinkNode.h"

FlowSinkNode::FlowSinkNode(QObject *parent) :
    FlowNode(parent),
    m_inputPort(new FlowInPort("input",this))
{
    setName("sink");
}

FlowInPort* FlowSinkNode::inputPort() const
{
    return m_inputPort;
}
