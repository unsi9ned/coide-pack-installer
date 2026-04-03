#include "database.h"
#include "services/logger.h"
#include "services/paths.h"

DataBase* DataBase::_m_instance = nullptr;
QString DataBase::m_errorString;
QMutex DataBase::m_mutex;

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
        logInfo("Closing the database connection");
        db.close();
    }

    delete _m_instance;
}

//------------------------------------------------------------------------------
// Создает единственный экземпляр класса
//------------------------------------------------------------------------------
DataBase *DataBase::instance()
{
    QMutexLocker locker(&m_mutex);

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

    logInfo("Attempting to connect to the database");

    if(!db.isOpen())
    {
        QFile dbFile(Paths::instance()->coIdeDatabaseFile());

        if(dbFile.exists())
        {
            db.setDatabaseName(Paths::instance()->coIdeDatabaseFile());
            status = db.open();

            if(status)
            {
                logInfo("The database is open");
                emit dbConnected();
            }
            else
            {
                m_errorString = db.lastError().databaseText();
                logError(QString("Database opening error: %1").arg(db.lastError().databaseText()));
                logError(QString("Database driver error: %1").arg(db.lastError().driverText()));
            }
        }
        else
        {
            m_errorString = "The database was not found";
            logError(m_errorString);
            status = false;
        }
    }
    else
        logInfo("The database is already open");

    return status;
}

//------------------------------------------------------------------------------
// Послать запрос базе
//------------------------------------------------------------------------------
QSqlQuery DataBase::sendQuery(QString queryString, bool * result)
{
    QSqlQuery query;

    QMutexLocker locker(&m_mutex);

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
