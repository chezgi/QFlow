#ifndef FLOWOBJECT_H
#define FLOWOBJECT_H

#include <QObject>
#include <QString>
#include <QDateTime>
#include <QTimer>
#include <QVariantMap>
#include <QUuid>


/*
 *
 *
 */

typedef QVariantMap FlowPacket;
class FlowGraph;
class FlowObject : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString uuid READ uuid CONSTANT FINAL)
    Q_PROPERTY(QDateTime creationTime READ creationTime CONSTANT FINAL)
    Q_PROPERTY(int flowObjectLifeTime READ flowObjectLifeTime WRITE setFlowObjectLifeTime NOTIFY flowObjectLifeTimeChanged)
    Q_PROPERTY(int flowObjectDeleteDelay READ flowObjectDeleteDelay WRITE setFlowObjectDeleteDelay NOTIFY flowObjectDeleteDelayChanged)
    Q_PROPERTY(FlowGraph* flowGraph READ flowGraph WRITE setFlowGraph NOTIFY flowGraphChanged)

public:
    explicit FlowObject(QObject *parent = 0);
    Q_INVOKABLE static QString flowObjectIdTag(){return "FlowObjectUuid";}
    Q_INVOKABLE static QString flowObjectSerializedTag(){return "FlowObjectSerialized";}
    Q_INVOKABLE QVariantMap createFlowPacket();

    virtual QVariantMap toMap();
    virtual void storageAddEvent();
    virtual void storageRemoveEvent();

    Q_INVOKABLE QString objectTypeName();

signals:
    void flowObjectExpired();

private slots:
    void lifetimeCheckTimedout();

private:
    QTimer lifetimeChecker;

//----------------------------------------- Q_PROPERTY
public:
    QString uuid() const;

    QDateTime creationTime() const;

    int flowObjectLifeTime() const;

    int flowObjectDeleteDelay() const;

    FlowGraph* flowGraph() const
    {
        return m_flowGraph;
    }

public slots:
    void setCreationTime(QDateTime arg);

    void setFlowObjectLifeTime(int arg);

    void setFlowObjectDeleteDelay(int arg);

    void setFlowGraph(FlowGraph* arg)
    {
        if (m_flowGraph != arg) {
            m_flowGraph = arg;
            emit flowGraphChanged(arg);
        }
    }

signals:
    void creationTimeChanged(QDateTime arg);

    void flowObjectLifeTimeChanged(int arg);

    void flowObjectDeleteDelayChanged(int arg);

    void flowGraphChanged(FlowGraph* arg);

protected:
    QString m_uuid;    

private:
    QDateTime m_creationTime;
    int m_flowObjectLifeTime;
    int m_flowObjectDeleteDelay;
    FlowGraph* m_flowGraph;
};
#endif // FLOWOBJECT_H
