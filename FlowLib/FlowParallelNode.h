#ifndef FLOWPARALLELNODE_H
#define FLOWPARALLELNODE_H

#include "FlowNode.h"

class FlowParallelNode : public FlowNode
{
    Q_OBJECT
    // 0 means all reply must be come in to do reply
    Q_PROPERTY(bool replyWhenCompleted READ replyWhenCompleted WRITE setReplyWhenCompleted NOTIFY replyWhenCompletedChanged)
    Q_PROPERTY(FlowRequestRouter* requestRouter READ requestRouter)

public:    
    explicit FlowParallelNode(QObject *parent = 0);

private slots:
    void processRequest(FlowInPort *inPort, QVariantMap flowPacket);
    void processResponse(FlowOutPort *outPort, QVariantMap flowPacket);
    void cleanup();

//------------------------------------------------ Q_PROPERTY
public:
    FlowRequestRouter* requestRouter() const;


    bool replyWhenCompleted() const;

signals:

    void replyWhenCompletedChanged(bool arg);

public slots:

void setReplyWhenCompleted(bool arg);

private:
    FlowRequestRouter* m_requestRouter;
    bool m_replyWhenCompleted;

};

#endif // FLOWPARALLELNODE_H
