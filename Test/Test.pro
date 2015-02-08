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

!equals(_PRO_FILE_PWD_, $$OUT_PWD) {
    copydata.commands = $(COPY_DIR) $$PWD/qml $$OUT_PWD
    first.depends = $(first) copydata
    export(first.depends)
    export(copydata.commands)
    QMAKE_EXTRA_TARGETS += first copydata
}

