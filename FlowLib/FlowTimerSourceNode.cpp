#include "FlowTimerSourceNode.h"

FlowTimerSourceNode::FlowTimerSourceNode(QObject *parent) :
    FlowSourceNode(parent),
    m_timer(new QTimer(this))
{
    setName("timerSource");
    m_interval = 1000;
    m_repeat = false;
    m_running = false;
    m_triggeredOnStart = false;
    m_timer->setInterval(1000);
    connect(m_timer,&QTimer::timeout,this,&FlowTimerSourceNode::timedout);
}

// The running property will be true following a call to restart()
void FlowTimerSourceNode::restart()
{
    if(m_timer->isActive())
        m_timer->stop();
    if(!running())
        setRunning(true);
    else
        m_timer->start();
}

void FlowTimerSourceNode::start()
{
    if(triggeredOnStart())
        emit triggered();
    setRunning(true);
}

void FlowTimerSourceNode::stop()
{
    setRunning(false);
}

void FlowTimerSourceNode::timedout()
{
    emit triggered();
    if(!repeat())
    {
        m_timer->stop();
    }
}

//-------------------------------------------------------- Q_PROPERTY
void FlowTimerSourceNode::setRunning(bool arg)
{
    if (m_running != arg) {
        m_running = arg;
        if(m_running)
            m_timer->start();
        else
            m_timer->stop();
        emit runningChanged(arg);
    }
}

void FlowTimerSourceNode::setInterval(int arg)
{
    if (m_interval != arg) {
        m_interval = arg;
        m_timer->setInterval(1000);
        emit intervalChanged(arg);
    }
}

void FlowTimerSourceNode::setRepeat(bool arg)
{
    if (m_repeat != arg) {
        m_repeat = arg;
        emit repeatChanged(arg);
    }
}

void FlowTimerSourceNode::setTriggeredOnStart(bool arg)
{
    if (m_triggeredOnStart != arg) {
        m_triggeredOnStart = arg;
        emit triggeredOnStartChanged(arg);
    }
}
