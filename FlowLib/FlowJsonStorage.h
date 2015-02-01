#ifndef FLOWJSONSTORAGE_H
#define FLOWJSONSTORAGE_H

#include <QObject>
#include <QVariant>

class FlowJsonStorage : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool debug READ debug WRITE setDebug NOTIFY debugChanged)
    Q_PROPERTY(QString dbpath READ dbpath WRITE setDbpath NOTIFY dbpathChanged)
    Q_PROPERTY(bool inMemory READ inMemory WRITE setInMemory NOTIFY inMemoryChanged)

public:
    explicit FlowJsonStorage(QObject *parent = 0);

    Q_INVOKABLE bool store(QString dbname,QString tablename,QVariant data);
    Q_INVOKABLE QVariant fetch(QString dbname,QString tablename);

//    Q_INVOKABLE QStringList listDatabases();
//    Q_INVOKABLE QStringList listTables(QString dbname);
//    Q_INVOKABLE QVariantList fetchAllTables(QString dbname);

private:
    QMap<QString,QVariant> inMemoryMap;

//------------------------------------- Q_PROPERTY
public:
    bool debug() const
    {
        return m_debug;
    }

    QString dbpath() const
    {
        return m_dbpath;
    }

    bool inMemory() const
    {
        return m_inMemory;
    }

signals:
    void debugChanged(bool arg);

    void dbpathChanged(QString arg);

    void inMemoryChanged(bool arg);

public slots:
    void setDebug(bool arg)
    {
        if (m_debug != arg) {
            m_debug = arg;
            emit debugChanged(arg);
        }
    }

    void setDbpath(QString arg)
    {
        if (m_dbpath != arg) {
            m_dbpath = arg;
            emit dbpathChanged(arg);
        }
    }

    void setInMemory(bool arg)
    {
        if (m_inMemory != arg) {
            m_inMemory = arg;
            emit inMemoryChanged(arg);
        }
    }

private:
    bool m_debug;
    QString m_dbpath;
    bool m_inMemory;
};

#endif // FLOWJSONSTORAGE_H
