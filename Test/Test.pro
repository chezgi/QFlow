TEMPLATE = app
QT += qml network websockets
QT -= gui
CONFIG += c++11 staticlib

INCLUDEPATH += ../FlowLib
LIBS += -L../FlowLib -lFlowLib

SOURCES += \
    main.cpp \
    CppNode.cpp

HEADERS += \
    CppNode.h

DISTFILES += \
    qml/Main.qml

