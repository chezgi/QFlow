#include "FlowDslEngine.h"

#include <QtCore/QUrl>
#include <QtCore/QDebug>

#include "FlowGraph.h"
#include "FlowNode.h"
#include "FlowPort.h"
#include "FlowObject.h"
#include "FlowObjectStorage.h"
#include "FlowJsonStorage.h"
#include "FlowRequestRouter.h"

#include "FlowInOutNode.h"
#include "FlowSequentialNode.h"
#include "FlowParallelNode.h"
#include "FlowSourceNode.h"
#include "FlowSinkNode.h"
#include "FlowRelayNode.h"
#include "FlowTimerSourceNode.h"
#include "FlowLogger.h"
#include "FlowTracer.h"
#include "FlowMonitor.h"
#include "FlowNodeMonitorInfo.h"
#include "FlowService.h"
#include "FlowServiceConnection.h"
#include "FlowNodeServiceInfo.h"

FlowDslEngine* FlowDslEngine::self = nullptr;
FlowDslEngine *FlowDslEngine::instance()
{
    if(!self)
        self = new FlowDslEngine;
    return self;
}

FlowDslEngine::FlowDslEngine(QObject *parent) :
    QObject(parent)
{
    qmlRegisterUncreatableType<FlowObject>("Flow", 1, 0, "Object","Flow.Object is pure Virtual. use subclasses.");
    qmlRegisterUncreatableType<FlowPort>("Flow", 1, 0, "Port","Use Directioned Ports.");
    qmlRegisterUncreatableType<FlowNodeMonitorInfo>("Flow", 1, 0, "NodeMonitorInfo","Only usable as flownode.");
    qmlRegisterUncreatableType<FlowNodeServiceInfo>("Flow", 1, 0, "NodeServiceInfo","Only usable as flownode.");
    qmlRegisterUncreatableType<FlowServiceConnection>("Flow", 1, 0, "ServiceConnection","Only for FlowService");

    qmlRegisterType<FlowGraph>("Flow", 1, 0, "Graph");
    qmlRegisterType<FlowObjectStorage>("Flow", 1, 0, "ObjectStorage");
    qmlRegisterType<FlowJsonStorage>("Flow", 1, 0, "JsonStorage");
    qmlRegisterType<FlowLogger>("Flow", 1, 0, "Logger");
    qmlRegisterType<FlowMonitor>("Flow", 1, 0, "Monitor");

    qmlRegisterType<FlowService>("Flow", 1, 0, "Service");

    qmlRegisterType<FlowNode>("Flow", 1, 0, "Node");

    qmlRegisterType<FlowRequestRouter>("Flow", 1, 0, "RequestRouter");
    qmlRegisterType<FlowInPort>("Flow", 1, 0, "InPort");
    qmlRegisterType<FlowOutPort>("Flow", 1, 0, "OutPort");


    qmlRegisterType<FlowInOutNode>("Flow", 1, 0, "InOutNode");
    qmlRegisterType<FlowInNode>("Flow", 1, 0, "InNode");
    qmlRegisterType<FlowOutNode>("Flow", 1, 0, "OutNode");
    qmlRegisterType<FlowSequentialNode>("Flow", 1, 0, "SequentialNode");
    qmlRegisterType<FlowRelayNode>("Flow", 1, 0, "RelayNode");
    qmlRegisterType<FlowSinkNode>("Flow", 1, 0, "SinkNode");
    qmlRegisterType<FlowSourceNode>("Flow", 1, 0, "SourceNode");
    qmlRegisterType<FlowTimerSourceNode>("Flow", 1, 0, "TimerSourceNode");


    engine = new QQmlEngine(this);
//    engine->rootContext()->setContextProperty(name,value);
}

void FlowDslEngine::loadUrl(const QString &url)
{
    QQmlComponent *rootComponent = new  QQmlComponent(engine);
    rootComponent->loadUrl(QUrl(url));

    if(rootComponent->status() != QQmlComponent::Ready)
    {
        qDebug() << "Not Ready!!!"  << rootComponent->errorString();
    }
    rootComponent->create();
    if(rootComponent->isError())
        qDebug() << rootComponent->errorString();
}




