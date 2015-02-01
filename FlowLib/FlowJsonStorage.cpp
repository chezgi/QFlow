#include "FlowJsonStorage.h"
#include <QDebug>
#include <QFile>
#include <QDir>
#include <QJsonDocument>

FlowJsonStorage::FlowJsonStorage(QObject *parent) :
    QObject(parent)
{
    m_debug = false;
    m_dbpath = "jsonStoarage.json";
    m_inMemory = false;
}

bool FlowJsonStorage::store(QString dbname, QString tablename, QVariant data)
{
    QString indexname = dbname + "." + tablename;
    QString dbfilename = dbpath() + "/" + indexname + ".json";
    if(debug())
        qDebug() << "storing data:" << dbfilename;

    if(inMemory())
    {
        inMemoryMap[indexname] = data;
    }else
    {
        if(!QDir().mkpath(dbpath()))
        {
            qDebug() << "given directory for FlowJsonStorage is invalid. check it. " << dbpath();
            return false;
        }
        QFile dbfile(dbfilename);
        if(!dbfile.open(QIODevice::WriteOnly|QIODevice::Unbuffered))
        {
            qDebug() << "given dbfile for FlowJsonStorage is invalid. check it. " << dbfilename;
            return false;
        }
        dbfile.write(QJsonDocument::fromVariant(data).toJson());
    }
    return true;
}

QVariant FlowJsonStorage::fetch(QString dbname, QString tablename)
{
    QString indexname = dbname + "." + tablename;
    QString dbfilename = dbpath() + "/" + indexname + ".json";
    if(debug())
        qDebug() << "fetch data:" << dbfilename;

    if(inMemory())
    {
        return inMemoryMap.value(indexname);
    }else
    {
        if(!QDir().mkpath(dbpath()))
        {
            qDebug() << "given directory for FlowJsonStorage is invalid. check it. " << dbpath();
            return false;
        }
        QFile dbfile(dbfilename);
        if(!dbfile.open(QIODevice::ReadOnly))
        {
            qDebug() << "given dbfile for FlowJsonStorage is invalid. check it. " << dbfilename;
            return QVariant();
        }
        QJsonParseError parseError;
        QJsonDocument doc = QJsonDocument::fromJson(dbfile.readAll(),&parseError);
        if(parseError.error != QJsonParseError::NoError)
        {
            qDebug() << "parsing json error in " << dbfilename << " :" << parseError.errorString();
            return QVariant();
        }
        return doc.toVariant();
    }
}
