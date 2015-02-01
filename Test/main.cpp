#include <QCoreApplication>
#include <QtQml>
#include "FlowDslEngine.h"
#include "CppNode.h"

int main(int argc,char **argv)
{
    QCoreApplication app(argc,argv);
    QObject::connect(FlowDslEngine::instance()->getEngine(), SIGNAL(quit()), &app, SLOT(quit()));

    qmlRegisterType<CppNode>("App", 1, 0, "CppNode");

    FlowDslEngine::instance()->loadUrl("qml/Main.qml");
    return app.exec();
}

