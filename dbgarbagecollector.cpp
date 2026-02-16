#include <QFile>
#include <QDebug>
#include <QDir>
#include "dbgarbagecollector.h"
#include "database.h"
#include "paths.h"
#include "requestmanager.h"

//------------------------------------------------------------------------------
// Класс очистки БД от ненужных данных и таблиц
//------------------------------------------------------------------------------
DBGarbageCollector::DBGarbageCollector() : QObject()
{
#if 1
    // На время отладки создаем бекап базы данных
    QFile dbFile(Paths::instance()->coIdeDatabaseFile());
    QFile dbBackup(Paths::instance()->coIdeDatabaseFile() + ".bak");

    if(dbFile.exists() && !dbBackup.exists())
    {
        QFile::copy(dbFile.fileName(), dbBackup.fileName());
    }
#endif

    connect(this, SIGNAL(eventOccured(QString)), SLOT(printEvents(QString)));
    connect(this, SIGNAL(errorOccured(QString)), SLOT(printEvents(QString)));
}

//------------------------------------------------------------------------------
// Очистка БД от ненужных мусорных данных
//------------------------------------------------------------------------------
bool DBGarbageCollector::deleteObsoleteData()
{
    if(cleanComponents())
        if(cleanUsers())
            return true;

    return false;
}

//------------------------------------------------------------------------------
// Удаление из БД не используемых таблиц
//------------------------------------------------------------------------------
bool DBGarbageCollector::deleteUnnecessaryTables()
{
    QStringList tables =
    {
        "Education", "Group_has_User", "Groups", "Industry",
        "Nationality", "Position", "comment",
        "component_not_supports_mcu", "component_not_supports_mcufamily",
        "component_not_supports_mcumanufacturer", "component_not_supports_mcuseries",
        "solution", "solution_has_component",
        "solution_has_example", "solution_has_keyword", "solution_has_category",
        "solution_has_subcategory", "solutioncategory", "solutionsubcategory",
        "user_glorification"
    };

    foreach (QString table, tables)
    {
        bool status = false;
        QString sql = QString("DROP TABLE IF EXISTS `%1`;").arg(table);
        QSqlQuery result = DataBase::instance()->sendQuery(sql, &status);

        emit eventOccured(QString("Delete table %1: %2").arg(table).arg(sql));

        if(!status)
        {
            _errorString = result.lastError().text();
            emit errorOccured(QString("Error: %1").arg(_errorString));
            return false;
        }
    }

    emit eventOccured(QString("Delete Unnecessary Tables DONE"));

    return false;
}

//------------------------------------------------------------------------------
// Вывод текста последней ошибки
//------------------------------------------------------------------------------
QString DBGarbageCollector::errorString()
{
    return _errorString;
}

//------------------------------------------------------------------------------
// Удаление пользователей, которые не задействованы в других таблицах
//------------------------------------------------------------------------------
bool DBGarbageCollector::cleanUsers()
{
    struct User
    {
        int id;
        QString name;
        int level;
        int componentsCount;
        int examplesCount;
        int mcuCount;
    };

    QList<User> users;

    bool status = false;
    QString sql = QString("SELECT `id`, `name`, `level` FROM user;");
    QSqlQuery result = DataBase::instance()->sendQuery(sql, &status);

    if(!status)
    {
        _errorString = result.lastError().text();
        return false;
    }

    while(result.next())
    {
        User u;
        u.id = result.value(0).toInt();
        u.name = result.value(1).toString();
        u.level = result.value(2).toInt();

        users.append(u);
    }

    foreach (User u, users)
    {
        sql = QString("SELECT COUNT(*) FROM `component` WHERE authorId = '%1';").arg(u.id);
        result = DataBase::instance()->sendQuery(sql, &status);

        if(!status || !result.next())
        {
            _errorString = result.lastError().text();
            return false;
        }
        else
        {
            u.componentsCount = result.value(0).toInt();
        }

        sql = QString("SELECT COUNT(*) FROM `example` WHERE userId = '%1';").arg(u.id);
        result = DataBase::instance()->sendQuery(sql, &status);

        if(!status || !result.next())
        {
            _errorString = result.lastError().text();
            return false;
        }
        else
        {
            u.examplesCount = result.value(0).toInt();
        }

        sql = QString("SELECT COUNT(*) FROM `mcu` WHERE userId = '%1';").arg(u.id);
        result = DataBase::instance()->sendQuery(sql, &status);

        if(!status || !result.next())
        {
            _errorString = result.lastError().text();
            return false;
        }
        else
        {
            u.mcuCount = result.value(0).toInt();
        }

        if(u.componentsCount == 0 && u.examplesCount == 0 && u.mcuCount == 0 && u.level == 0)
        {
            sql = QString("DELETE FROM `user` WHERE id = '%1';").arg(u.id);
            result = DataBase::instance()->sendQuery(sql, &status);

            emit eventOccured(QString("Remove User %1 with %2 components, %3 examples, %4 MCUs: %5").
                              arg(u.name).
                              arg(u.componentsCount).
                              arg(u.examplesCount).
                              arg(u.mcuCount).
                              arg(sql));

            if(!status)
            {
                _errorString = result.lastError().text();
                emit errorOccured(QString("Error: %1").arg(_errorString));
                return false;
            }
        }
    }

    emit eventOccured(QString("Clean `user` table DONE"));

    return true;
}

//------------------------------------------------------------------------------
// Удаление из базы несуществующих в локальном репозитории компонентов
//------------------------------------------------------------------------------
bool DBGarbageCollector::cleanComponents()
{
    RequestManager * reqManager = RequestManager::instance();

    emit eventOccured("Remove phantom relations in `component_depends_component`");

    if(!reqManager->removeComponentPhantomRelations(&_errorString))
        return false;

    QMap<int, Component> components = reqManager->requestComponentMap();

    for(auto it = components.begin(); it != components.end(); ++it)
    {
        Component c = it.value();
        QString componentPath;
        QDir componentDir;

        if(c.isDriver())
        {
            componentPath = Paths::instance()->coIdeDriverDir(c.getId(), c.getName());
        }
        else
        {
            componentPath = Paths::instance()->coIdeComponentDir(c.getId(), c.getName());
        }

        componentDir.setPath(componentPath);

        QStringList componentEntryList = componentDir.entryList(QDir::AllEntries | QDir::NoDotAndDotDot);
        bool componentExists = componentDir.exists();

        if(componentEntryList.isEmpty() || ((componentEntryList.count() == 1 &&
                                             componentEntryList.first().toLower() == "doc")))
            componentExists = false;

        // Статус компонента прочитан неверно либо отсутствует в базе данных
        if(c.getStatus().isNull())
        {
            emit eventOccured(QString("Component %1 is NULL").arg(it.key()));
        }
        // Каталог существует, но в базе помечен как не скачанный
        else if(componentExists && !c.isDownloaded())
        {
            qInfo() << "Wrong status" << componentPath;
        }
        // Каталог не существует, но в базе помечен как скачанный
        else if(!componentExists && c.isDownloaded())
        {
            qInfo() << "Not found" << componentPath;
        }
        // И каталог не существует на диске, и в базе помечен как не скачанный
        else if(!componentExists && !c.isDownloaded())
        {
            emit eventOccured(QString("Deleting a non-existent component '%1_%2'").
                              arg(c.getId()).
                              arg(c.getName()));

            if(!reqManager->removeComponent(c.getId(), &_errorString))
                return false;
        }
        // Каталог существует и статус верный
        else if(componentExists && c.isDownloaded())
        {
            //qInfo() << "Status OK" << componentPath;
        }
    }

    return true;
}

//------------------------------------------------------------------------------
// Вывод сообщений об ошибках
//------------------------------------------------------------------------------
void DBGarbageCollector::printEvents(QString e)
{
    qInfo() << e;
}

