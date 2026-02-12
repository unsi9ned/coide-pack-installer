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
    QString pathToDb;

public:
    explicit DataBase();
    ~DataBase();

    bool tryOpen(QString path_to_db);
    QSqlQuery sendQuery(QString queryString, bool * result = NULL);
    void setDbPath(QString path);
    bool isOpen(){return db.isOpen();}

signals:

    void errorOccured(QString e);
    void dbConnected();

public slots:
};

#endif // DATABASE_H
