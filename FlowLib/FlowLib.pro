TEMPLATE = lib
QT += network websockets
QT -= gui
CONFIG += c++11 staticlib
QMAKE_CXXFLAGS += -Wall -Wextra

HEADERS += \
    FlowDslEngine.h \
    FlowNode.h \
    FlowPort.h \
    FlowRequestRouter.h \
    FlowGraph.h \
    FlowObject.h \
    FlowParallelNode.h \
    FlowSequentialNode.h \
    FlowPacket.h \
    FlowRelayNode.h \
    FlowInOutNode.h \
    FlowSinkNode.h \
    FlowSourceNode.h \
    FlowTimerSourceNode.h \
    FlowMonitor.h \
    FlowNodeMonitorInfo.h \
    FlowCommunicationUtils.h \
    FlowService.h \
    FlowServiceConnection.h \
    FlowObjectStorage.h \
    FlowJsonStorage.h \    
    FlowNodeServiceInfo.h

SOURCES += \
    FlowDslEngine.cpp \
    FlowNode.cpp \
    FlowPort.cpp \
    FlowRequestRouter.cpp \
    FlowGraph.cpp \
    FlowObject.cpp \
    FlowParallelNode.cpp \
    FlowSequentialNode.cpp \
    FlowPacket.cpp \
    FlowRelayNode.cpp \
    FlowInOutNode.cpp \
    FlowSinkNode.cpp \
    FlowSourceNode.cpp \
    FlowTimerSourceNode.cpp \
    FlowMonitor.cpp \
    FlowNodeMonitorInfo.cpp \
    FlowCommunicationUtils.cpp \
    FlowService.cpp \
    FlowServiceConnection.cpp \
    FlowObjectStorage.cpp \
    FlowJsonStorage.cpp \
    FlowNodeServiceInfo.cpp
