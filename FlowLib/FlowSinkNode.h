#ifndef FLOWSINKNODE_H
#define FLOWSINKNODE_H

#include "FlowNode.h"

class FlowSinkNode : public FlowNode
{
    Q_OBJECT
    Q_PROPERTY(FlowInPort* inputPort READ inputPort CONSTANT)
public:
    explicit FlowSinkNode(QObject *parent = 0);

public:
    FlowInPort *inputPort() const;

signals:
public slots:
private:
    FlowInPort* m_inputPort;
};


#endif // FLOWSINKNODE_H
