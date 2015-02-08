#ifndef QMLFLOW_PLUGIN_H
#define QMLFLOW_PLUGIN_H

#include <QQmlExtensionPlugin>

class QmlFlowPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "Flow")

public:
    void registerTypes(const char *uri);
};

#endif // QMLFLOW_PLUGIN_H

