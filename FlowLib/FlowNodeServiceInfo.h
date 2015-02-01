#ifndef FLOWNODESERVICEINFO_H
#define FLOWNODESERVICEINFO_H

#include <QObject>
#include <QtQml/QQmlParserStatus>

#include "FlowServiceConnection.h"
class FlowNode;
class FlowNodeServiceInfo : public QObject, public QQmlParserStatus
{
    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString api READ api WRITE setApi NOTIFY apiChanged)
    Q_PROPERTY(bool anonymousAllowed READ anonymousAllowed WRITE setAnonymousAllowed NOTIFY anonymousAllowedChanged)

    Q_PROPERTY(bool debug READ debug WRITE setDebug NOTIFY debugChanged)
    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged)

public:
    explicit FlowNodeServiceInfo(FlowNode *parent = 0);

    Q_INVOKABLE FlowServiceConnection *getConnection(QString clientId);
    Q_INVOKABLE void broadcast(QVariantMap data);
    Q_INVOKABLE void sendTo(QString clientId, QVariantMap data);
    Q_INVOKABLE void sendErrorTo(QString clientId, QString errorMessage, QVariant request);
    Q_INVOKABLE bool hasRegisteredClient();

    // QQmlParserStatus interface
public:
    void classBegin(){}
    void componentComplete();

private slots:
    void checkInitialization();

private:
    bool load_completed;

    //---------------------------------------- Q_PROPERTY
public:
    QString name() const
    {
        return m_name;
    }

    QString api() const
    {
        return m_api;
    }

    bool debug() const
    {
        return m_debug;
    }

    bool enabled() const
    {
        return m_enabled;
    }

    bool anonymousAllowed() const
    {
        return m_anonymousAllowed;
    }

signals:

    void nameChanged(QString arg);

    void apiChanged(QString arg);

    void debugChanged(bool arg);

    void enabledChanged(bool arg);

    void anonymousAllowedChanged(bool arg);

public slots:

    void setName(QString arg)
    {
        if (m_name != arg) {
            m_name = arg;
            emit nameChanged(arg);
        }
    }
    void setApi(QString arg)
    {
        if (m_api != arg) {
            m_api = arg;
            emit apiChanged(arg);
        }
    }
    void setDebug(bool arg)
    {
        if (m_debug != arg) {
            m_debug = arg;
            emit debugChanged(arg);
        }
    }
    void setEnabled(bool arg);

    void setAnonymousAllowed(bool arg)
    {
        if (m_anonymousAllowed != arg) {
            m_anonymousAllowed = arg;
            emit anonymousAllowedChanged(arg);
        }
    }

private:
    QString m_name;

    QString m_api;

    bool m_debug;

    bool m_enabled;

    bool m_anonymousAllowed;

};

#endif // FLOWNODESERVICEINFO_H
