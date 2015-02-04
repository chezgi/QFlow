#ifndef FLOWOBJECTSTORAGE_H
#define FLOWOBJECTSTORAGE_H

#include <QObject>
#include <QTimer>
#include <QDateTime>
#include <QString>
#include <QMap>

class FlowObject;
class FlowObjectStorage : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool debug READ debug WRITE setDebug NOTIFY debugChanged)
public:
    explicit FlowObjectStorage(QObject *parent = 0);

    //------------------------------ flow storage
    //    template<class T> T *createFlowObject();
    Q_INVOKABLE FlowObject *get(const QString &uuid);
    Q_INVOKABLE FlowObject *getByPacket(const QVariantMap &flowPacket);
    Q_INVOKABLE void add(FlowObject *flowObject);
    Q_INVOKABLE void remove(const QString &uuid);
    Q_INVOKABLE void removeByPacket(const QVariantMap &flowPacket);


signals:
    void objectAdded(QString objectId);
    void objectRemoved(QString objectId);

private slots:
    void cleanupPackets();
    void objectExpiredEvent();
    void objectRemovedEvent();    

private:
    QHash<QString,FlowObject *> flowObjectsByHash;
    QMap<QDateTime,QObject *> waitingForRemoveList;
    QTimer cleanupTimer;   

    //------------------------------------------- Q_PROPERTY
public:
    bool debug() const
    {
        return m_debug;
    }

signals:
    void debugChanged(bool arg);

public slots:
    void setDebug(bool arg)
    {
        if (m_debug != arg) {
            m_debug = arg;
            emit debugChanged(arg);
        }
    }
private:
    bool m_debug;

};

#endif // FLOWOBJECTSTORAGE_H
