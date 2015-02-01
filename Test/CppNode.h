#ifndef CPPNODE_H
#define CPPNODE_H

#include <QObject>
#include "FlowNode.h"

class CppNode : public FlowNode
{
    Q_OBJECT
    Q_PROPERTY(FlowInPort* inDataPort READ inDataPort CONSTANT)
    Q_PROPERTY(FlowOutPort* outDataPort READ outDataPort CONSTANT)

public:
    explicit CppNode(QObject *parent = 0);

    // FlowNode interface
protected slots:
    void initializeNode();
    void processRequest(FlowInPort *inPort, QVariantMap flowPacket);
    void processResponse(FlowOutPort *outPort, QVariantMap flowPacket);
    void processServiceRequest(QString clientId, QVariantMap requestData);

    //-------------------------------------------
public:
    FlowInPort* inDataPort() const
    {
        return m_inDataPort;
    }

    FlowOutPort* outDataPort() const
    {
        return m_outDataPort;
    }

signals:
public slots:
private:
    FlowInPort* m_inDataPort;
    FlowOutPort* m_outDataPort;

};

#endif // CPPNODE_H
