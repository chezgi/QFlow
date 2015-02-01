#ifndef FLOWDSLENGINE_H
#define FLOWDSLENGINE_H

#include <QObject>
#include <QtQml/QQmlContext>
#include <QtQml/QQmlEngine>
#include <QtQml/QQmlComponent>

class QQmlEngine;
class FlowDslEngine : public QObject
{
    Q_OBJECT
public:
    static FlowDslEngine *instance();
    void loadUrl(const QString &url);
    QQmlEngine *getEngine(){return engine;}

private:
    explicit FlowDslEngine(QObject *parent = 0);

private:
    static FlowDslEngine *self;
    QQmlEngine *engine;
};
#endif // FLOWDSLENGINE_H
