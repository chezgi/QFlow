#ifndef FLOWINOUTNODE_H
#define FLOWINOUTNODE_H

#include "FlowNode.h"

class FlowInOutNode : public FlowNode
{
    Q_OBJECT
    Q_PROPERTY(FlowInPort* inputPort READ inputPort CONSTANT)
    Q_PROPERTY(FlowOutPort* outputPort READ outputPort CONSTANT)
public:
    explicit FlowInOutNode(QObject *parent = 0);

public:
    FlowInPort* inputPort() const;

    FlowOutPort* outputPort() const;

signals:
public slots:
private:
    FlowInPort* m_inputPort;
    FlowOutPort* m_outputPort;
};

class FlowInNode: public FlowNode
{
    Q_OBJECT
    Q_PROPERTY(FlowInPort* inputPort READ inputPort)
public:
    explicit FlowInNode(QObject *parent = 0);

public:
    FlowInPort* inputPort() const;
signals:
public slots:
private:
    FlowInPort* m_inputPort;
};


class FlowOutNode: public FlowNode
{
    Q_OBJECT
    Q_PROPERTY(FlowOutPort* outputPort READ outputPort)
public:
    explicit FlowOutNode(QObject *parent = 0);

public:
    FlowOutPort* outputPort() const;

signals:
public slots:
private:
    FlowOutPort* m_outputPort;
};


#endif // FLOWINOUTNODE_H
