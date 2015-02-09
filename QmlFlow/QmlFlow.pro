TEMPLATE = lib
TARGET = QmlFlow
QT += qml quick websockets
CONFIG += qt plugin c++11

TARGET = $$qtLibraryTarget($$TARGET)

INCLUDEPATH += ../FlowLib

# Input
SOURCES += \
    QmlFlowPlugin.cpp

HEADERS += \
    QmlFlowPlugin.h

DISTFILES = qmldir

#------------------------------------- deploy additional files to snapshot build dir
!equals(_PRO_FILE_PWD_, $$OUT_PWD) {
    #  copy qmldir
    copy_qmldir.target = $$OUT_PWD/qmldir
    copy_qmldir.depends = $$_PRO_FILE_PWD_/qmldir
    copy_qmldir.commands = $(COPY_FILE) \"$$replace(copy_qmldir.depends, /, $$QMAKE_DIR_SEP)\" \"$$replace(copy_qmldir.target, /, $$QMAKE_DIR_SEP)\"
    QMAKE_EXTRA_TARGETS += copy_qmldir
    PRE_TARGETDEPS += $$copy_qmldir.target
}

#------------------------------------- install module to Qt
qmldir.files = qmldir
unix {
    installPath = $$[QT_INSTALL_QML]/Flow
    qmldir.path = $$installPath
    target.path = $$installPath
    INSTALLS += target qmldir
}
