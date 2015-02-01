#include "CppNode.h"

static QString api = R"(
c2s:{
        ping:""
}

s2c:{
        pong:""
}
)";

CppNode::CppNode(QObject *parent) : FlowNode(parent),
  m_inDataPort(new FlowInPort("testInPort",this)),
  m_outDataPort(new FlowOutPort("testOutPort",this))
{
    setName("TestNode");
}

void CppNode::initializeNode()
{
    serviceInfo()->setName("testService");
    serviceInfo()->setApi(api);
    serviceInfo()->setEnabled(true);
}

void CppNode::processRequest(FlowInPort *inPort, QVariantMap flowPacket)
{
    qDebug() << "Request Received";
    sendResponseToPort(inPort,flowPacket);
}

void CppNode::processResponse(FlowOutPort *outPort, QVariantMap flowPacket)
{
    Q_UNUSED(outPort)
    Q_UNUSED(flowPacket)

    qDebug() << "Response Received";
}

void CppNode::processServiceRequest(QString clientId, QVariantMap requestData)
{
    Q_UNUSED(requestData)

    QVariantMap result;
    result["pong"] = "" ;
    serviceInfo()->sendTo(clientId,result);
}

