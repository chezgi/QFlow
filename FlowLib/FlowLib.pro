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
    FlowMonitor.h \
    FlowNodeMonitorInfo.h \
    FlowCommunicationUtils.h \
    FlowObjectStorage.h \
    FlowJsonStorage.h \    
    FlowNodeServiceInfo.h \
    FlowRestService.h \
    FlowRestConnection.h

SOURCES += \
    FlowDslEngine.cpp \
    FlowNode.cpp \
    FlowPort.cpp \
    FlowRequestRouter.cpp \
    FlowGraph.cpp \
    FlowObject.cpp \
    FlowMonitor.cpp \
    FlowNodeMonitorInfo.cpp \
    FlowCommunicationUtils.cpp \
    FlowObjectStorage.cpp \
    FlowJsonStorage.cpp \
    FlowNodeServiceInfo.cpp \
    FlowRestService.cpp \
    FlowRestConnection.cpp
