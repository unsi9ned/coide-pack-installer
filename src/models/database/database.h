#ifndef DATABASE_H
#define DATABASE_H

#include <QObject>
#include <QtSql/QSqlDatabase>
#include <QSqlQuery>
#include <QSqlResult>
#include <QSqlError>
#include <QFile>
#include <QDebug>
#include <QMutex>

#include "common/loggable.h"

class DataBase : public QObject, public Loggable
{
    Q_OBJECT

private:
    QSqlDatabase db;
    static DataBase* _m_instance;
    static QString m_errorString;
    static QMutex m_mutex;

    explicit DataBase();
    ~DataBase();

protected:
    QString logSource() const override { return "DataBase"; }

public:
    DataBase(const DataBase&) = delete;
    DataBase& operator=(const DataBase&) = delete;

    static DataBase* instance();

    QSqlQuery sendQuery(QString queryString, bool * result = nullptr);
    QString lastError() const { return DataBase::m_errorString; }
    QStringList tables() const { return db.tables(); }

private:
    bool isOpen();
    bool tryOpen();

signals:

    void dbConnected();

public slots:
};

#endif // DATABASE_H
