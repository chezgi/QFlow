#ifndef FLOWSOURCENODE_H
#define FLOWSOURCENODE_H

#include "FlowNode.h"

class FlowSourceNode : public FlowNode
{
    Q_OBJECT
    Q_PROPERTY(FlowOutPort* outputPort READ outputPort)

public:
    explicit FlowSourceNode(QObject *parent = 0);

public:
    FlowOutPort* outputPort() const;

signals:
public slots:
private:
    FlowOutPort* m_outputPort;
};

#endif // FLOWSOURCENODE_H
