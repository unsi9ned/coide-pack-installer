#include "database.h"
#include "logger.h"
#include "paths.h"

//------------------------------------------------------------------------------
// Открыть соединение
//------------------------------------------------------------------------------
DataBase::DataBase() : QObject()
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    tryOpen(Paths::instance()->coIdeDatabaseFile());
}

//------------------------------------------------------------------------------
// Закрыть соединение с БД
//------------------------------------------------------------------------------
DataBase::~DataBase()
{
    if(db.isOpen())
    {
        db.close();
    }
}

//------------------------------------------------------------------------------
// Открыть соединение
//------------------------------------------------------------------------------
bool DataBase::tryOpen(QString path_to_db)
{
    bool status = true;

    Logger::instance()->addEvent("Attempting to connect to the database");

    if(!db.isOpen())
    {
        pathToDb = path_to_db;
        QFile dbFile(path_to_db);

        if(dbFile.exists())
        {
            db.setDatabaseName(path_to_db);
            status = db.open();

            if(status)
            {
                Logger::instance()->addEvent("The database is open");
                emit dbConnected();
            }
            else
            {
                Logger::instance()->addEvent(QString("Database opening error: %1").arg(db.lastError().databaseText()));
                Logger::instance()->addEvent(QString("Database driver error: %1").arg(db.lastError().driverText()));
            }
        }
        else
        {
            Logger::instance()->addEvent("The database was not found");
            emit errorOccured(tr("База данных не найдена"));
            status = false;
        }
    }
    else
        Logger::instance()->addEvent("The database is already open");

    return status;
}

//------------------------------------------------------------------------------
// Послать запрос базе
//------------------------------------------------------------------------------
QSqlQuery DataBase::sendQuery(QString queryString, bool * result)
{
    QSqlQuery query;

    if(db.isOpen())
    {
        bool status;
        status = query.exec(queryString);
        if(result != NULL)
        {
            *result = status;
        }
    }
    else
    {
        this->tryOpen(pathToDb);
    }

    return query;
}

//------------------------------------------------------------------------------
// Задать путь к базе данных
//------------------------------------------------------------------------------
void DataBase::setDbPath(QString path)
{
    if(db.isOpen())
    {
        db.close();
    }

    this->pathToDb = path;
    this->tryOpen(path);
}
