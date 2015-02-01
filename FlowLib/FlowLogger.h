#ifndef FLOWLOGGER_H
#define FLOWLOGGER_H

#include <QObject>
#include <QVariant>

class FlowLogger : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool debug READ debug WRITE setDebug NOTIFY debugChanged)
public:
    explicit FlowLogger(QObject *parent = 0);

    virtual void add(const QString &table, QString recordid,QByteArray jsondata);
    virtual void add(const QString &table, QString recordid, QVariantMap jsonmap);
    virtual void remove(const QString &table, QString recordid);
    virtual void merge(const QString &table, QString recordid,QByteArray jsondata);
    virtual void merge(const QString &table, QString recordid,QVariantMap jsonmap);
    virtual void update(const QString &table,QString recordid, QByteArray jsondata);
    virtual void updateScripted(const QString &scriptFile, const QString &table, const QString &recordid, const QVariantList &args=QVariantList());

//----------------------------------------- Q_PROPERTY
public:
    bool debug() const;

signals:
    void debugChanged(bool arg);

public slots:
    void setDebug(bool arg);

private:
    bool m_debug;
};

#endif // FLOWLOGGER_H
