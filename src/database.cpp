#include "database.h"
#include "logger.h"
#include "paths.h"

DataBase* DataBase::_m_instance = nullptr;

//------------------------------------------------------------------------------
// Открыть соединение
//------------------------------------------------------------------------------
DataBase::DataBase() : QObject()
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    tryOpen();
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

    delete _m_instance;
}

//------------------------------------------------------------------------------
// Создает единственный экземпляр класса
//------------------------------------------------------------------------------
DataBase *DataBase::instance()
{
    if(!_m_instance)
        _m_instance = new DataBase();

    return _m_instance;
}

//------------------------------------------------------------------------------
// Открыть соединение
//------------------------------------------------------------------------------
bool DataBase::tryOpen()
{
    bool status = true;

    Logger::instance()->addEvent("Attempting to connect to the database");

    if(!db.isOpen())
    {
        QFile dbFile(Paths::instance()->coIdeDatabaseFile());

        if(dbFile.exists())
        {
            db.setDatabaseName(Paths::instance()->coIdeDatabaseFile());
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
            emit errorOccured(tr("The database was not found"));
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

    if(db.isOpen() || this->tryOpen())
    {
        bool status;
        status = query.exec(queryString);
        if(result != NULL)
        {
            *result = status;
        }
    }

    return query;
}

//------------------------------------------------------------------------------
// Проверка открыта ли база данных
//------------------------------------------------------------------------------
bool DataBase::isOpen()
{
    return db.isOpen();
}
