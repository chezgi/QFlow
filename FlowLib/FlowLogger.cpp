#include "FlowLogger.h"

#include <QDebug>

FlowLogger::FlowLogger(QObject *parent) :
    QObject(parent)
{
    m_debug = false;
}

void FlowLogger::add(const QString &table, QString recordid, QByteArray jsondata)
{
    Q_UNUSED(table);
    Q_UNUSED(recordid);
    Q_UNUSED(jsondata);
    qDebug() << "Dummy FlowLogger Add!!!!!";
}

void FlowLogger::add(const QString &table, QString recordid, QVariantMap jsonmap)
{
    Q_UNUSED(table);
    Q_UNUSED(recordid);
    Q_UNUSED(jsonmap);
    qDebug() << "Dummy FlowLogger Add!!!!!";
}

void FlowLogger::remove(const QString &table, QString recordid)
{
    Q_UNUSED(table);
    Q_UNUSED(recordid);
    qDebug() << "Dummy FlowLogger Remove!!!!!";
}

void FlowLogger::merge(const QString &table, QString recordid, QByteArray jsondata)
{
    Q_UNUSED(table);
    Q_UNUSED(recordid);
    Q_UNUSED(jsondata);
    qDebug() << "Dummy FlowLogger Merge!!!!!";
}

void FlowLogger::merge(const QString &table, QString recordid, QVariantMap jsonmap)
{
    Q_UNUSED(table);
    Q_UNUSED(recordid);
    Q_UNUSED(jsonmap);
    qDebug() << "Dummy FlowLogger Merge!!!!!";
}

void FlowLogger::update(const QString &table, QString recordid, QByteArray jsondata)
{
    Q_UNUSED(table);
    Q_UNUSED(recordid);
    Q_UNUSED(jsondata);
    qDebug() << "Dummy FlowLogger Update!!!!!";
}

void FlowLogger::updateScripted(const QString &scriptFile, const QString &table,const QString &recordid,const QVariantList &args)
{
    Q_UNUSED(scriptFile);
    Q_UNUSED(table);
    Q_UNUSED(recordid);
    Q_UNUSED(args);
    qDebug() << "Dummy FlowLogger Update!!!!!";
}

//----------------------------------------- Q_PROPERTY
bool FlowLogger::debug() const
{
    return m_debug;
}

void FlowLogger::setDebug(bool arg)
{
    if (m_debug != arg) {
        m_debug = arg;
        emit debugChanged(arg);
    }
}
