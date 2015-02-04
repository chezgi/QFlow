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
