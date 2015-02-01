#ifndef FLOWTIMERSOURCENODE_H
#define FLOWTIMERSOURCENODE_H

#include "FlowSourceNode.h"

class FlowTimerSourceNode : public FlowSourceNode
{
    Q_OBJECT
    Q_PROPERTY(bool running READ running WRITE setRunning NOTIFY runningChanged)
    Q_PROPERTY(int interval READ interval WRITE setInterval NOTIFY intervalChanged)
    Q_PROPERTY(bool repeat READ repeat WRITE setRepeat NOTIFY repeatChanged)
    Q_PROPERTY(bool triggeredOnStart READ triggeredOnStart WRITE setTriggeredOnStart NOTIFY triggeredOnStartChanged)

public:
    explicit FlowTimerSourceNode(QObject *parent = 0);

    Q_INVOKABLE void restart();
    Q_INVOKABLE void start();
    Q_INVOKABLE void stop();

signals:
    void triggered();

private slots:
    void timedout();

private:
    QTimer *m_timer;

//-------------------------------------------------------- Q_PROPERTY
public:
    bool running() const
    {
        return m_running;
    }

    int interval() const
    {
        return m_interval;
    }

    bool repeat() const
    {
        return m_repeat;
    }

    bool triggeredOnStart() const
    {
        return m_triggeredOnStart;
    }

signals:
    void runningChanged(bool arg);

    void intervalChanged(int arg);

    void repeatChanged(bool arg);

    void triggeredOnStartChanged(bool arg);

public slots:
    void setRunning(bool arg);

    void setInterval(int arg);

    void setRepeat(bool arg);

    void setTriggeredOnStart(bool arg);

private:
    bool m_running;
    int m_interval;
    bool m_repeat;
    bool m_triggeredOnStart;
};

#endif // FLOWTIMERSOURCENODE_H
