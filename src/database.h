#ifndef DATABASE_H
#define DATABASE_H

#include <QObject>
#include <QtSql/QSqlDatabase>
#include <QSqlQuery>
#include <QSqlResult>
#include <QSqlError>
#include <QFile>
#include <QDebug>

class DataBase : public QObject
{
    Q_OBJECT

private:
    QSqlDatabase db;
    static DataBase* _m_instance;

    explicit DataBase();
    ~DataBase();

public:
    DataBase(const DataBase&) = delete;
    DataBase& operator=(const DataBase&) = delete;

    static DataBase* instance();

    bool isOpen();
    bool tryOpen();
    QSqlQuery sendQuery(QString queryString, bool * result = nullptr);

signals:

    void errorOccured(QString e);
    void dbConnected();

public slots:
};

#endif // DATABASE_H
