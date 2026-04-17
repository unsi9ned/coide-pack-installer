#include "requestmanager.h"
#include "componentsinfo.h"
#include "services/logger.h"

#define USE_UNIQUE_ID   1
#define VERBOSE_DEBUG   0

RequestManager* RequestManager::_m_instance = nullptr;

RequestManager::RequestManager() : ComponentsInfo()
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
}

RequestManager::~RequestManager()
{
    delete _m_instance;
}

RequestManager *RequestManager::instance()
{
    if(!_m_instance)
        _m_instance = new RequestManager();

    return _m_instance;
}

//------------------------------------------------------------------------------
// Загрузка данных об MCU выбранного производителя
//------------------------------------------------------------------------------
void RequestManager::loadDataFromDb(const QString &vendorName, Manufacturer &vendor)
{
    Manufacturer requestedVendor;
    QList<Manufacturer> manufacturers = requestManufacturerList();

    for(auto m : manufacturers)
    {
        if(m.getName() == vendorName)
        {
            requestedVendor = m;
            break;
        }
    }

    if(!requestedVendor.isValid())
        return;
    else
    {
        vendor.setId(requestedVendor.getId());
        vendor.setName(requestedVendor.getName());
    }

    QList<Family> families = requestFamilyList(requestedVendor.toKeilId());

    for(int f = 0; f < families.length(); f++)
    {
        Family fam = families.at(f);
        QList<Series> series = requestSeriesList(fam.getId());

        Family& newFamily = vendor.addFamily(fam.getName());
        newFamily.setId(fam.getId());
        newFamily.setManufacturerId(fam.getManufacturerId());
        newFamily.setName(fam.getName());

        for(int s = 0 ; s < series.length(); s++)
        {
            Series serie = series.at(s);
            QList<Mcu> mcuList = requestMcuList(serie.getId());

            Series& newSeries = vendor.family(fam.getName()).addSeries(serie.getName());
            newSeries.setId(serie.getId());
            newSeries.setFamilyId(fam.getId());
            newSeries.setName(serie.getName());

            for(int m = 0; m < mcuList.length(); m++)
            {
                Mcu mcu = mcuList.at(m);
                Mcu& newMcu = vendor.family(fam.getName()).series(serie.getName()).addMcu(mcu);
                mcu.setParent(newMcu.getParent());
                newMcu = mcu;
            }
        }
    }
}

void RequestManager::loadDataFromDb(const QStringList &vendors, QMap<QString, Manufacturer>& vendorMap)
{
    foreach (QString vendorName, vendors)
    {
        if(!vendorMap.contains(vendorName))
            vendorMap.insert(vendorName, Manufacturer());

        Manufacturer& vendor = vendorMap[vendorName];
        loadDataFromDb(vendorName, vendor);
    }
}

//------------------------------------------------------------------------------
// Загрузка всех данных о MCU из базы данных
//------------------------------------------------------------------------------
void RequestManager::loadDataFromDb(QMap<QString, Manufacturer> &vendorMap)
{
    QList<Manufacturer> manufacturers = requestManufacturerList();

    for(int i = 0; i < manufacturers.length(); i++)
    {
        Manufacturer m = manufacturers.at(i);
        vendorMap.insert(m.getName(), m);

        Manufacturer& manufacturer = vendorMap[m.getName()];
        QList<Family> families = requestFamilyList(m.toKeilId());

        for(int f = 0; f < families.length(); f++)
        {
            Family fam = families.at(f);
            QList<Series> series = requestSeriesList(fam.getId());

            for(int s = 0 ; s < series.length(); s++)
            {
                Series serie = series.at(s);
                QList<Mcu> mcuList = requestMcuList(serie.getId());

                for(int m = 0; m < mcuList.length(); m++)
                {
                    Mcu mcu = mcuList.at(m);
                    manufacturer.family(fam.getName()).series(serie.getName()).addMcu(mcu);
                }
            }
        }
    }
}

//------------------------------------------------------------------------------
// Запросить список производителей
//------------------------------------------------------------------------------
QList<Manufacturer> RequestManager::requestManufacturerList()
{
    QList<Manufacturer> manufact;
    QSqlQuery result = DataBase::instance()->sendQuery("SELECT * FROM mcumanufacturer");

    while(result.next())
    {
        int id = result.value(0).toInt();
        QString name = result.value(1).toString();

        manufact.append(Manufacturer(id, name));
    }

    return manufact;
}

//------------------------------------------------------------------------------
// Достать из БД одного единственного производителя
//------------------------------------------------------------------------------
Manufacturer RequestManager::requestManufacturer(int id, QString name)
{
    Manufacturer manufact;
    QString queryStr;

    if(name.isEmpty())
    {
        queryStr = QString("SELECT * FROM mcumanufacturer "
                           "WHERE id = '%1';").arg(id);
    }
    else
    {
        queryStr = QString("SELECT * FROM mcumanufacturer "
                           "WHERE id = '%1' AND name = '%2';").
                            arg(id).arg(name);
    }

    QSqlQuery result = DataBase::instance()->sendQuery(queryStr);

    if(result.next())
    {
        manufact.setId(result.value(0).toInt());
        manufact.setName(result.value(1).toString());
    }

    return manufact;
}

//------------------------------------------------------------------------------
// Запросить список семейств
//------------------------------------------------------------------------------
QList<Family> RequestManager::requestFamilyList(int vendorId)
{
    QList<Family> fam;
    QString queryStr = QString("SELECT * FROM mcufamily "
                               "WHERE manufacturerId=%1").arg(vendorId);
    QSqlQuery result = DataBase::instance()->sendQuery(queryStr);

    while(result.next())
    {
        int id = result.value(0).toInt();
        QString familyName = result.value(1).toString();

        fam.append(Family(id, familyName, vendorId));
    }

    return fam;
}

//------------------------------------------------------------------------------
// Запросить список семейств
//------------------------------------------------------------------------------
QList<Family> RequestManager::requestFamilyList()
{
    QList<Family> fam;
    QString queryStr = QString("SELECT * FROM mcufamily");
    QSqlQuery result = DataBase::instance()->sendQuery(queryStr);

    while(result.next())
    {
        int id = result.value(0).toInt();
        QString familyName = result.value(1).toString();
        int manufacturerId = result.value(2).toInt();

        fam.append(Family(id, familyName, manufacturerId));
    }

    return fam;
}

//------------------------------------------------------------------------------
// Запросить список семейств
//------------------------------------------------------------------------------
QList<Series> RequestManager::requestSeriesList(int familyId)
{
    QList<Series> series;
    QString queryStr = QString("SELECT * FROM mcuseries "
                               "WHERE familyId=%1").arg(familyId);
    QSqlQuery result = DataBase::instance()->sendQuery(queryStr);

    while(result.next())
    {
        int id = result.value(0).toInt();
        QString name = result.value(1).toString();

        series.append(Series(id, name, familyId));
    }

    return series;
}

//------------------------------------------------------------------------------
// Запросить список микроконтроллеров
//------------------------------------------------------------------------------
QList<Mcu> RequestManager::requestMcuList(int seriesId)
{
    QList<Mcu> microcontrollers;
    QString queryStr = QString("SELECT * FROM mcu "
                               "WHERE seriesId=%1").arg(seriesId);
    QSqlQuery result = DataBase::instance()->sendQuery(queryStr);

    while(result.next())
    {
        int id = result.value(0).toInt();
        int seriesId = result.value(1).toInt();
        int userId = result.value(2).toInt();
        int debugAlgorithmId = result.value(3).toInt();
        QString name = result.value(4).toString();
        QString description = result.value(5).toString();
        QByteArray keyParameter = result.value(6).toByteArray();
        QByteArray webPageURL = result.value(7).toByteArray();
        QByteArray datasheetURL = result.value(8).toByteArray();
        QByteArray memInfo = result.value(9).toByteArray();
        QString micro = result.value(10).toString();
        QString advertising = result.value(11).toString();
        QString price = result.value(12).toString();
        QString timeuuid = result.value(13).toString();
        int hits = result.value(14).toInt();

        Mcu mcu;
        mcu.setId(id);
        mcu.setSeriesId(seriesId);
        mcu.setUserId(userId);
        mcu.setDebugAlgorithmId(debugAlgorithmId);
        mcu.setName(name);
        mcu.setDescription(description);
        mcu.setKeyParameter(keyParameter);
        mcu.fromCoWebPageURL(webPageURL);
        mcu.fromCoDatasheetURL(datasheetURL);
        mcu.setMemInfoFromJson(memInfo);
        mcu.setMicro(micro);
        mcu.setAdvertising(advertising);
        mcu.setPrice(price);
        mcu.setTimeuuid(timeuuid);
        mcu.setHits(hits);

        DebugAlgorithm da = requestDebugAlgorithm(debugAlgorithmId);
        mcu.setDebugAlgorithm(da);

        ProgAlgorithm fa = getMcuFlashAlgorithm(id);
        mcu.addAlgorithm(fa);

        microcontrollers.append(mcu);
    }

    return microcontrollers;
}

Mcu RequestManager::requestMcu(const QString& name)
{
    QString mcuName = name;
    Mcu mcu;
    QString queryStr = QString("SELECT * FROM mcu WHERE name = '%1' LIMIT 1").
                       arg(mcuName.remove("'"));
    QSqlQuery result = DataBase::instance()->sendQuery(queryStr);

    while(result.next())
    {
        int id = result.value(0).toInt();
        int seriesId = result.value(1).toInt();
        int userId = result.value(2).toInt();
        int debugAlgorithmId = result.value(3).toInt();
        QString name = result.value(4).toString();
        QString description = result.value(5).toString();
        QByteArray keyParameter = result.value(6).toByteArray();
        QByteArray webPageURL = result.value(7).toByteArray();
        QByteArray datasheetURL = result.value(8).toByteArray();
        QByteArray memInfo = result.value(9).toByteArray();
        QString micro = result.value(10).toString();
        QString advertising = result.value(11).toString();
        QString price = result.value(12).toString();
        QString timeuuid = result.value(13).toString();
        int hits = result.value(14).toInt();

        mcu.setId(id);
        mcu.setSeriesId(seriesId);
        mcu.setUserId(userId);
        mcu.setDebugAlgorithmId(debugAlgorithmId);
        mcu.setName(name);
        mcu.setDescription(description);
        mcu.setKeyParameter(keyParameter);
        mcu.fromCoWebPageURL(webPageURL);
        mcu.fromCoDatasheetURL(datasheetURL);
        mcu.setMemInfoFromJson(memInfo);
        mcu.setMicro(micro);
        mcu.setAdvertising(advertising);
        mcu.setPrice(price);
        mcu.setTimeuuid(timeuuid);
        mcu.setHits(hits);

        DebugAlgorithm da = requestDebugAlgorithm(debugAlgorithmId);
        mcu.setDebugAlgorithm(da);

        ProgAlgorithm fa = getMcuFlashAlgorithm(id);
        mcu.addAlgorithm(fa);

        break;
    }

    return mcu;
}

//------------------------------------------------------------------------------
// Загрузить из базы алгоритм отладки
//------------------------------------------------------------------------------
QList<DebugAlgorithm> RequestManager::requestDebugAlgorithmList()
{
    QList<DebugAlgorithm> daList;

    QString queryStr = QString("SELECT * FROM debug_algorithm");
    QSqlQuery result = DataBase::instance()->sendQuery(queryStr);

    while(result.next())
    {
        DebugAlgorithm da;

        int id = result.value(0).toInt();
        QString name = result.value(1).toString();
        QString description = result.value(2).toString();
        QString creatingDate = result.value(3).toString();
        QString updateDate = result.value(4).toString();
        QString timeUuid = result.value(5).toString();
        int documentId = result.value(6).toInt();

        da.setCoId(id);
        da.setName(name);
        da.setDescription(description);
        da.setCreateDate(creatingDate);
        da.setUpdateDate(updateDate);
        da.setTimeUUID(timeUuid);
        da.setDocumentId(documentId);

        daList.append(da);
    }

    return daList;
}

//------------------------------------------------------------------------------
// Загрузить из базы алгоритм отладки
//------------------------------------------------------------------------------
DebugAlgorithm RequestManager::requestDebugAlgorithm(int id)
{
    DebugAlgorithm da;

    QString queryStr = QString("SELECT * FROM debug_algorithm "
                               "WHERE id=%1 LIMIT 1").arg(id);
    QSqlQuery result = DataBase::instance()->sendQuery(queryStr);

    while(result.next())
    {
        id = result.value(0).toInt();
        QString name = result.value(1).toString();
        QString description = result.value(2).toString();
        QString creatingDate = result.value(3).toString();
        QString updateDate = result.value(4).toString();
        QString timeUuid = result.value(5).toString();
        int documentId = result.value(6).toInt();

        da.setCoId(id);
        da.setName(name);
        da.setDescription(description);
        da.setCreateDate(creatingDate);
        da.setUpdateDate(updateDate);
        da.setTimeUUID(timeUuid);
        da.setDocumentId(documentId);

        break;
    }

    return da;
}

DebugAlgorithm RequestManager::requestDebugAlgorithm(const QString &name)
{
    DebugAlgorithm da;

    QString queryStr = QString("SELECT * FROM debug_algorithm "
                               "WHERE name = '%1' LIMIT 1").arg(name);
    QSqlQuery result = DataBase::instance()->sendQuery(queryStr);

    while(result.next())
    {
        int id = result.value(0).toInt();
        QString name = result.value(1).toString();
        QString description = result.value(2).toString();
        QString creatingDate = result.value(3).toString();
        QString updateDate = result.value(4).toString();
        QString timeUuid = result.value(5).toString();
        int documentId = result.value(6).toInt();

        da.setCoId(id);
        da.setName(name);
        da.setDescription(description);
        da.setCreateDate(creatingDate);
        da.setUpdateDate(updateDate);
        da.setTimeUUID(timeUuid);
        da.setDocumentId(documentId);

        break;
    }

    return da;
}

//------------------------------------------------------------------------------
// Загрузить из базы алгоритм программирования
//------------------------------------------------------------------------------
ProgAlgorithm RequestManager::requestFlashAlgorithm(int algId)
{
    ProgAlgorithm fa;

    QString queryStr = QString("SELECT "
                               "flash_algorithm.id, "
                               "flash_algorithm.name, "
                               "flash_algorithm.description, "
                               "flash_algorithm.deviceType, "
                               "flash_algorithm.deviceSize, "
                               "flash_algorithm.create_date, "
                               "flash_algorithm.update_date, "
                               "flash_algorithm.timeuuid, "
                               "flash_algorithm.documentId "
                               "FROM flash_algorithm "
                               "WHERE id = '%1' LIMIT 1").arg(algId);

    QSqlQuery result = DataBase::instance()->sendQuery(queryStr);

    while(result.next())
    {
        qint32 id = result.value("id").toInt();
        QString name = result.value("name").toString();
        QString description = result.value("description").toString();
        QString createDate = result.value("create_date").toString();
        QString updateDate = result.value("update_date").toString();
        QString timeUuid = result.value("timeuuid").toString();
        int documentId = result.value("documentId").toInt();

        fa.setCoId(id);
        fa.parseCoName(name);
        fa.setDocumentId(documentId);
        fa.setTimeUUID(timeUuid);
        fa.setCreateDate(createDate);
        fa.setUpdateDate(updateDate);
        fa.setDescription(description);
        fa.setInstalled(true);
        break;
    }

    return fa;
}

//------------------------------------------------------------------------------
// Загрузить из базы алгоритм программирования
//------------------------------------------------------------------------------
ProgAlgorithm RequestManager::requestFlashAlgorithm(const QString& name,
                                                    QString* errorString)
{
    bool status;
    ProgAlgorithm fa;

    QString queryStr = QString("SELECT "
                               "flash_algorithm.id, "
                               "flash_algorithm.name, "
                               "flash_algorithm.description, "
                               "flash_algorithm.deviceType, "
                               "flash_algorithm.deviceSize, "
                               "flash_algorithm.create_date, "
                               "flash_algorithm.update_date, "
                               "flash_algorithm.timeuuid, "
                               "flash_algorithm.documentId "
                               "FROM flash_algorithm "
                               "WHERE name = '%1' LIMIT 1;").arg(name);

#if VERBOSE_DEBUG
    logDebug(QString("FLASH Algorithm Request:"));
    logDebug(QString("---------------------"));
    logDebug(QString("SELECT"));
    logDebug(QString("\tflash_algorithm.id,"));
    logDebug(QString("\tflash_algorithm.name,"));
    logDebug(QString("\tflash_algorithm.description,"));
    logDebug(QString("\tflash_algorithm.deviceType,"));
    logDebug(QString("\tflash_algorithm.deviceSize,"));
    logDebug(QString("\tflash_algorithm.create_date,"));
    logDebug(QString("\tflash_algorithm.update_date,"));
    logDebug(QString("\tflash_algorithm.timeuuid,"));
    logDebug(QString("\tflash_algorithm.documentId"));
    logDebug(QString("FROM"));
    logDebug(QString("\tflash_algorithm"));
    logDebug(QString("WHERE"));
    logDebug(QString("\tname = '%1'").arg(name));
    logDebug(QString("LIMIT 1;"));
    logDebug(QString("---------------------"));
#endif

    QSqlQuery result = DataBase::instance()->sendQuery(queryStr, &status);

    if(!status)
    {
        if(errorString)
            *errorString = result.lastError().text();
        return ProgAlgorithm();
    }

    while(result.next())
    {
        qint32 id = result.value("id").toInt();
        QString name = result.value("name").toString();
        QString description = result.value("description").toString();
        QString createDate = result.value("create_date").toString();
        QString updateDate = result.value("update_date").toString();
        QString timeUuid = result.value("timeuuid").toString();
        int documentId = result.value("documentId").toInt();

#if VERBOSE_DEBUG
        logDebug(QString("Request result:"));
        logDebug(QString("---------------------"));
        logDebug(QString("ID: %1").arg(id));
        logDebug(QString("Name: %1").arg(name));
        logDebug(QString("Description: %1").arg(description));
        logDebug(QString("Create Date: %1").arg(createDate));
        logDebug(QString("Update Date: %1").arg(updateDate));
        logDebug(QString("UUID: %1").arg(timeUuid));
        logDebug(QString("DocumentId: %1").arg(documentId));
        logDebug(QString("---------------------"));
#endif

        fa.setCoId(id);
        fa.parseCoName(name);
        fa.setDocumentId(documentId);
        fa.setTimeUUID(timeUuid);
        fa.setCreateDate(createDate);
        fa.setUpdateDate(updateDate);
        fa.setDescription(description);
        fa.setInstalled(true);
        break;
    }

    return fa;
}

//------------------------------------------------------------------------------
// Загрузить из базы список алгоритмов программирования
//------------------------------------------------------------------------------
QList<ProgAlgorithm> RequestManager::requestFlashAlgorithmList()
{
    QList<ProgAlgorithm> faList;

    QString queryStr = QString("SELECT "
                               "flash_algorithm.id, "
                               "flash_algorithm.name, "
                               "flash_algorithm.description, "
                               "flash_algorithm.deviceType, "
                               "flash_algorithm.deviceSize, "
                               "flash_algorithm.create_date, "
                               "flash_algorithm.update_date, "
                               "flash_algorithm.timeuuid, "
                               "flash_algorithm.documentId "
                               "FROM flash_algorithm");
    QSqlQuery result = DataBase::instance()->sendQuery(queryStr);

    while(result.next())
    {
        ProgAlgorithm fa;

        qint32 id = result.value("id").toInt();
        QString name = result.value("name").toString();
        QString description = result.value("description").toString();
        QString createDate = result.value("create_date").toString();
        QString updateDate = result.value("update_date").toString();
        QString timeUuid = result.value("timeuuid").toString();
        int documentId = result.value("documentId").toInt();

        fa.setCoId(id);
        fa.parseCoName(name);
        fa.setDocumentId(documentId);
        fa.setTimeUUID(timeUuid);
        fa.setCreateDate(createDate);
        fa.setUpdateDate(updateDate);
        fa.setDescription(description);
        fa.setInstalled(true);

        faList.append(fa);
    }

    return faList;
}

//------------------------------------------------------------------------------
// Узнать из базы алгоритм программирования микроконтроллера
//------------------------------------------------------------------------------
ProgAlgorithm RequestManager::getMcuFlashAlgorithm(int mcuId)
{
    ProgAlgorithm fa;
    int faId = -1;

    QString queryStr = QString("SELECT * FROM mcu_has_flash_algorithm "
                               "WHERE mcuId=%1 LIMIT 1").arg(mcuId);
    QSqlQuery result = DataBase::instance()->sendQuery(queryStr);

    while(result.next())
    {
        faId = result.value(1).toInt();
        break;
    }

    if(faId >= 0)
    {
        fa = requestFlashAlgorithm(faId);
    }

    return fa;
}

//------------------------------------------------------------------------------
// Создать нового производителя
//------------------------------------------------------------------------------
bool RequestManager::createManufacturer(const Manufacturer &vendor)
{
    QString errorStr;
    bool status = true;

    if(!vendor.isValid(&errorStr))
    {
        status = false;
    }
    else
    {
        // Проверяем наличие производителя в базе данных
        Manufacturer m = requestManufacturer(vendor.toKeilId(), vendor.toKeilName());

        if(m.isNull())
        {
            QString queryStr = QString("INSERT INTO mcumanufacturer "
                                       "VALUES ('%1','%2')").
                                arg(vendor.toKeilId()).
                                arg(vendor.toKeilName());

            QSqlQuery result = DataBase::instance()->sendQuery(queryStr, &status);

            if(!status)
            {
                errorStr = result.lastError().text();
            }
        }
    }

    if(!status)
    {
        logError(errorStr);
    }

    return status;
}

//------------------------------------------------------------------------------
// Создать новое семейство
//------------------------------------------------------------------------------
bool RequestManager::createFamily(Family& family)
{
    QString errorStr;
    bool status = true;

    if(!family.isValid(&errorStr))
    {
        status = false;
    }
    else
    {
        //Загружаем производителей из базы и смотрим нет ли уже такого
        //Заодно определяем последний id

        bool isset = false;

        QList<Family> families = requestFamilyList(family.getManufacturerId());

        for(int f = 0; f < families.length(); f++)
        {
            Family currFam = families.at(f);

            if(currFam.getManufacturerId() == family.getManufacturerId() &&
               currFam.getCoName().trimmed().toLower() == family.getCoName().trimmed().toLower())
            {
                family.setId(currFam.getId());
                isset = true;
                break;
            }
        }

        if(!isset)
        {
#if USE_UNIQUE_ID
            QString queryStr = QString("INSERT INTO mcufamily "
                                       "VALUES ('%1','%2','%3')").
                                       arg(family.getUniqueId()).
                                       arg(family.getCoName()).
                                       arg(family.getManufacturerId());

            QSqlQuery result = DataBase::instance()->sendQuery(queryStr, &status);

            if(!status)
            {
                errorStr = result.lastError().text();
            }
            else
                family.setId(family.getUniqueId());
#else
            int lastId = -1;

            //Поиск последнего айди
            QString queryStr = QString("SELECT MAX(id) FROM mcufamily");
            QSqlQuery result = DataBase::instance()->sendQuery(queryStr, &status);

            if(!status || !result.next())
            {
                errorStr = result.lastError().text();
            }
            else
            {
                lastId = result.value(0).toInt();
                family.setId(lastId + 1);

                queryStr = QString("INSERT INTO mcufamily "
                                   "VALUES ('%1','%2','%3')").
                                    arg(lastId + 1).
                                    arg(family.getCoName()).
                                    arg(family.getManufacturerId());

                result = DataBase::instance()->sendQuery(queryStr, &status);

                if(!status)
                {
                    errorStr = result.lastError().text();
                }
            }
#endif
        }
    }

    if(!status)
    {
        logError(errorStr);
    }

    return status;
}

//------------------------------------------------------------------------------
// Создать новую серию
//------------------------------------------------------------------------------
bool RequestManager::createSeries(Series& series)
{
    QString errorStr;
    bool status = true;

    if(!series.isValid(&errorStr))
    {
        status = false;
    }
    else
    {
#if USE_UNIQUE_ID
        bool isset = false;
        //
        // Сначала загружаем список существующих серий и проверяем нет ли уже такой
        //
        QList<Series> seriesList = requestSeriesList(series.getFamilyId());

        for(int s = 0; s < seriesList.length(); s++)
        {
            Series currSeries = seriesList.at(s);

            if(currSeries.getName().trimmed().toLower() ==
               series.getName().trimmed().toLower())
            {
                series.setId(currSeries.getId());
                isset = true;
                break;
            }
        }

        //
        // Создание новой серии
        //
        if(!isset)
        {
            QString queryStr = QString("INSERT INTO mcuseries "
                                       "VALUES ('%1','%2','%3')").
                                arg(series.getUniqueId()).
                                arg(series.getName()).
                                arg(series.getFamilyId());
            QSqlQuery result = DataBase::instance()->sendQuery(queryStr, &status);

            if(!status)
            {
                errorStr = result.lastError().text();
            }
            else
            {
                series.setId(series.getUniqueId());
            }
        }
#else
        bool isset = false;
        int lastId = -1;

        //Поиск последнего айди
        QString queryStr = QString("SELECT MAX(id) FROM mcuseries");
        QSqlQuery result = DataBase::instance()->sendQuery(queryStr, &status);

        if(!status || !result.next())
        {
            errorStr = result.lastError().text();
        }
        else
        {
            lastId = result.value(0).toInt();

            QList<Series> seriesList = requestSeriesList(series.getFamilyId());

            for(int s = 0; s < seriesList.length(); s++)
            {
                Series currSeries = seriesList.at(s);

                if(currSeries.getName().trimmed().toLower() ==
                   series.getName().trimmed().toLower())
                {
                    series.setId(currSeries.getId());
                    isset = true;
                    break;
                }
            }

            if(!isset)
            {
                series.setId(lastId + 1);

                QString queryStr = QString("INSERT INTO mcuseries "
                                           "VALUES ('%1','%2','%3')").
                                    arg(series.getId()).
                                    arg(series.getName()).
                                    arg(series.getFamilyId());
                QSqlQuery result = DataBase::instance()->sendQuery(queryStr, &status);

                if(!status)
                {
                    errorStr = result.lastError().text();
                }
            }
        }
#endif
    }

    if(!status)
    {
        logError(errorStr);
    }

    return status;
}

//------------------------------------------------------------------------------
// Создать новый микроконтроллер
//------------------------------------------------------------------------------
bool RequestManager::createMcu(Mcu &device)
{
    QString errorStr;
    bool status = true;

    if(!device.isValid(&errorStr))
    {
        status = false;
    }
    else
    {
#if USE_UNIQUE_ID
        bool isset = false;

        //
        // Сначала проверяем нет ли устройства в базе
        //
        QList<Mcu> mcuList = requestMcuList(device.getSeriesId());

        for(int m = 0; m < mcuList.length(); m++)
        {
            Mcu currMcu = mcuList.at(m);

            if(currMcu.getName().trimmed().toLower() ==
               device.getName().trimmed().toLower())
            {
                isset = true;
                device.setId(currMcu.getId());
                device.setTimeuuid(currMcu.getTimeuuid());
                break;
            }
        }

        //
        // Создаем новое устройство
        //
        if(!isset)
        {
            QString queryStr = QString("INSERT INTO mcu "
                                       "VALUES ('%1','%2','%3','%4','%5','%6','%7','%8','%9','%10','%11','%12','%13','%14','%15')").
                                arg(device.getUniqueId()).
                                arg(device.getSeriesId()).
                                arg(device.getUserId()).
                                arg(device.getDebugAlgorithm().coId()).
                                arg(device.getCoName()).
                                arg(device.coDescription()).
                                arg(device.coFeaturesSummary()).
                                arg(device.coWebPageUrl()).
                                arg(device.coDatasheetURL()).
                                arg(device.coMemInfo()).
                                arg(device.defSym2coMicro()).
                                arg(device.getAdvertising()).
                                arg(device.getPrice()).
                                arg(device.getTimeuuid()).
                                arg(device.getHits());

            QSqlQuery result = DataBase::instance()->sendQuery(queryStr, &status);

            if(!status)
            {
                errorStr = result.lastError().text();
            }
            else
            {
                device.setId(device.getUniqueId());
            }
        }
#else
        bool isset = false;
        int lastId = -1;

        //Поиск последнего айди
        QString queryStr = QString("SELECT MAX(id) FROM mcu");
        QSqlQuery result = DataBase::instance()->sendQuery(queryStr, &status);

        if(!status || !result.next())
        {
            errorStr = result.lastError().text();
        }
        else
        {
            lastId = result.value(0).toInt();

            QList<Mcu> mcuList = requestMcuList(device.getSeriesId());

            for(int m = 0; m < mcuList.length(); m++)
            {
                Mcu currMcu = mcuList.at(m);

                if(currMcu.getName().trimmed().toLower() ==
                   device.getName().trimmed().toLower())
                {
                    isset = true;
                    device.setId(currMcu.getId());
                    device.setTimeuuid(currMcu.getTimeuuid());
                    break;
                }
            }

            if(!isset)
            {
                /*
                QString memInfo = "[\"{\\\"type\\\":\\\"FLASH\\\",\\\"id\\\":0,"
                                  "\\\"start\\\":\\\"0x08000000\\\","
                                  "\\\"size\\\":\\\"0x00020000\\\"}\","
                                  "\"{\\\"type\\\":\\\"RAM\\\","
                                  "\\\"id\\\":0,\\\"start\\\":\\\"0x20000000\\\","
                                  "\\\"size\\\":\\\"0x00008000\\\"}\"]";
                */
                device.setId(lastId + 1);

                QString queryStr = QString("INSERT INTO mcu "
                                           "VALUES ('%1','%2','%3','%4','%5','%6','%7','%8','%9','%10','%11','%12','%13','%14','%15')").
                                    arg(device.getId()).
                                    arg(device.getSeriesId()).
                                    arg(device.getUserId()).
                                    arg(device.getDebugAlgorithm().coId()).
                                    arg(device.getCoName()).
                                    arg(device.coDescription()).
                                    arg(device.coFeaturesSummary()).
                                    arg(device.coWebPageUrl()).
                                    arg(device.coDatasheetURL()).
                                    arg(device.coMemInfo()).
                                    arg(device.defSym2coMicro()).
                                    arg(device.getAdvertising()).
                                    arg(device.getPrice()).
                                    arg(device.getTimeuuid()).
                                    arg(device.getHits());

                QSqlQuery result = DataBase::instance()->sendQuery(queryStr, &status);

                if(!status)
                {
                    errorStr = result.lastError().text();
                }
            }
        }
#endif
    }

    if(!status)
    {
        logError(errorStr);
    }

    return status;
}

//------------------------------------------------------------------------------
// Добавить алгоритм отладки в базу
//------------------------------------------------------------------------------
bool RequestManager::createDebugAlgorithm(DebugAlgorithm &algo)
{
    QString errorStr;
    bool status = true;

    if(algo.isNull())
    {
        status = false;
        errorStr = tr("Algorithm name is not defined");
    }
    else
    {
        //Загружаем алгоритмы из базы
        bool isset = false;
        int lastId = 0;
        QList<DebugAlgorithm> daList = requestDebugAlgorithmList();

        for(int a = 0; a < daList.length(); a++)
        {
            DebugAlgorithm da = daList.at(a);

            if(da.coId() > lastId)
            {
                lastId = da.coId();
            }

            if(da.name().trimmed().toLower() == algo.name().trimmed().toLower())
            {
                isset = true;
                algo = da;
                break;
            }
        }

        if(!isset)
        {
            QString queryStr = QString("INSERT INTO debug_algorithm "
                                       "VALUES ('%1','%2','%3','%4','%5','%6','%7')").
                                       arg(algo.getUniqueId()).
                                       arg(algo.name()).
                                       arg(algo.description()).
                                       arg(algo.creationDate("yyyy-MM-dd HH:mm:ss.z")).
                                       arg(algo.creationDate("yyyy-MM-dd HH:mm:ss.z")).
                                       arg(algo.timeUUID()).
                                       arg(algo.documentId());

            QSqlQuery result = DataBase::instance()->sendQuery(queryStr, &status);

            if(!status)
            {
                errorStr = result.lastError().text();
            }
            else
            {
                algo.setCoId(algo.getUniqueId());
            }
        }
    }

    if(!status)
    {
        logError(errorStr);
    }

    return status;
}

//------------------------------------------------------------------------------
// Добавить алгоритм программирования в базу
//------------------------------------------------------------------------------
bool RequestManager::createFlashAlgorithm(ProgAlgorithm& algo)
{
    QString errorStr;
    bool status = true;

    if(!algo.isValid(&errorStr))
    {
        status = false;
    }
    else
    {
        bool isset = false;

        //
        // Сначала проверяем нет ли устройства в базе
        //
#if 0
        auto progAlgList = requestFlashAlgorithmList();

        for (auto currAlg : progAlgList)
        {
            if(currAlg.getPath() == algo.getPath())
            {
                isset = true;
                algo.setCoId(currAlg.coId());
                algo.setTimeUUID(currAlg.timeUUID());
                algo.setCreateDate(currAlg.creationDate());
                algo.setUpdateDate(currAlg.updateDate());
                algo.setDescription(currAlg.description());
                break;
            }
        }
#else
        auto foundAlg = requestFlashAlgorithm(algo.getPath());

        if(!foundAlg.isNull())
        {
            isset = true;
            algo.setCoId(foundAlg.coId());
            algo.setTimeUUID(foundAlg.timeUUID());
            algo.setCreateDate(foundAlg.creationDate());
            algo.setUpdateDate(foundAlg.updateDate());
            algo.setDescription(foundAlg.description());
        }
#endif

        //
        // Создаем новое устройство
        //
        if(!isset)
        {
            QString queryStr = QString("INSERT INTO flash_algorithm "
                                       "VALUES ('%1','%2','%3','%4','%5','%6','%7','%8','%9')").
                                arg(algo.getUniqueId()).
                                arg(algo.getPath()).
                                arg(algo.description()).
                                arg("").                      //deviceType
                                arg("").                      //deviceSize
                                arg(algo.creationDate()).
                                arg(algo.updateDate()).
                                arg(algo.timeUUID()).
                                arg(algo.documentId());

            QSqlQuery result = DataBase::instance()->sendQuery(queryStr, &status);

            if(!status)
            {
                errorStr = result.lastError().text();
            }
            else
            {
                algo.setCoId(algo.getUniqueId());
            }
        }
    }

    if(!status)
    {
        logError(errorStr);
    }

    return status;
}

//------------------------------------------------------------------------------
// Создание связи между алгоритмом и устройством
//------------------------------------------------------------------------------
bool RequestManager::createFlashAlgorithmLink(const Mcu& device,
                                              const ProgAlgorithm& algo,
                                              QString* errorString)
{
    QString errorStr;
    bool status = true;

    if(!algo.isValid(&errorStr) || !device.isValid(&errorStr))
    {
        status = false;
    }
    else
    {
        auto foundAlg = requestFlashAlgorithm(algo.getPath(), errorString);
        auto foundDevice = requestMcu(device.getCoName());

        if(foundAlg.isNull())
        {
#if VERBOSE_DEBUG
            logDebug(QString("FLASH Algorithm Info:"));
            logDebug(QString("---------------------"));
            logDebug(QString("ID: %1").arg(foundAlg.coId()));
            logDebug(QString("Name: %1").arg(foundAlg.name()));
            logDebug(QString("Description: %1").arg(foundAlg.description()));
            logDebug(QString("UUID: %1").arg(foundAlg.timeUUID()));
            logDebug(QString("Create Date: %1").arg(foundAlg.creationDate()));
            logDebug(QString("Update Date: %1").arg(foundAlg.updateDate()));
            logDebug(QString("Vendor: %1").arg(foundAlg.getVendor()));
            logDebug(QString("Version: %1").arg(foundAlg.getVersion()));
            logDebug(QString("---------------------"));
#endif

            errorStr = QString("The Flash Algorithm '%1' is not found").arg(algo.getPath());
            status = false;
        }
        else if(foundDevice.isNull())
        {
            errorStr = QString("The Mcu '%1' is not found").arg(device.getCoName());
            status = false;
        }
        else
        {
            // Проверка существования связи
            QString queryStr = QString("SELECT "
                                       "mcu_has_flash_algorithm.mcuId, "
                                       "mcu_has_flash_algorithm.flashAlgorithmId "
                                       "FROM mcu_has_flash_algorithm "
                                       "WHERE mcuId = '%1' AND flashAlgorithmId = '%2' LIMIT 1").
                                arg(device.getUniqueId()).
                                arg(algo.getUniqueId());

            QSqlQuery result = DataBase::instance()->sendQuery(queryStr, &status);

            if(!status)
            {
                errorStr = result.lastError().text();
            }
            else
            {
                QPair<qint32, qint32> link;
                bool exists = false;

                while(result.next())
                {
                    qint32 mcuId = result.value("mcuId").toInt();
                    qint32 flashAlgorithmId = result.value("flashAlgorithmId").toInt();

                    link.first = mcuId;
                    link.second = flashAlgorithmId;
                    exists = true;

                    break;
                }

                if(!exists)
                {
                    queryStr = QString("INSERT INTO mcu_has_flash_algorithm "
                                       "VALUES ('%1','%2')").
                                       arg(device.getUniqueId()).
                                       arg(algo.getUniqueId());

                    result = DataBase::instance()->sendQuery(queryStr, &status);

                    if(!status)
                    {
                        errorStr = result.lastError().text();
                    }
                }
            }
        }
    }

    if(!status)
    {
        logError(errorStr);
    }

    return status;
}

//------------------------------------------------------------------------------
// Удаление контроллера из базы данных
//------------------------------------------------------------------------------
bool RequestManager::removeMcu(Mcu mcu)
{
    QString errorStr;
    bool status = true;

    if(mcu.getId() < 0)
    {
        status = false;
        errorStr = tr("Некорректный идентификатор контроллера");
    }
    else
    {
        //Поиск последнего айди
        QString queryStr = QString("DELETE FROM mcu WHERE id='%1'").arg(mcu.getId());
        QSqlQuery result = DataBase::instance()->sendQuery(queryStr, &status);

        if(!status)
        {
            errorStr = result.lastError().text();
        }
    }

    if(!status)
    {
        logError(errorStr);
    }

    return status;
}

//------------------------------------------------------------------------------
// Удаление серии процессоров
//------------------------------------------------------------------------------
bool RequestManager::removeSerie(Series serie)
{
    QString errorStr;
    bool status = true;

    if(serie.getId() < 0)
    {
        status = false;
        errorStr = tr("Некорректный идентификатор серии");
    }
    else
    {
        QList<Mcu> mcuList;

        for(auto it = serie.mcuMap().begin(); it != serie.mcuMap().end(); ++it)
        {
            mcuList << it.value();
        }

        for(int m = 0; m < mcuList.count(); m++)
        {
            Mcu currMcu = mcuList.at(m);

            if(!removeMcu(currMcu))
            {
                status = false;
                break;
            }
        }

        //Удаление серии из таблицы серий
        if(status)
        {
            QString queryStr = QString("DELETE FROM mcuseries WHERE id='%1'").arg(serie.getId());
            QSqlQuery result = DataBase::instance()->sendQuery(queryStr, &status);

            if(!status)
            {
                errorStr = result.lastError().text();
            }
        }
    }

    if(!status)
    {
        logError(errorStr);
    }

    return status;
}

//------------------------------------------------------------------------------
// Удаление семейства процессоров от выбранного производителя
//------------------------------------------------------------------------------
bool RequestManager::removeFamily(Family family)
{
    QString errorStr;
    bool status = true;

    if(family.getId() < 0)
    {
        status = false;
        errorStr = tr("Некорректный идентификатор семейства");
    }
    else
    {
        QList<Series> seriesList;

        for(auto it = family.seriesMap().begin(); it != family.seriesMap().end(); ++it)
        {
            seriesList.append(it.value());
        }

        for(int s = 0; s < seriesList.count(); s++)
        {
            Series currSerie = seriesList.at(s);

            if(!removeSerie(currSerie))
            {
                status = false;
                break;
            }
        }

        //Удаление серии из таблицы серий
        if(status)
        {
            QString queryStr = QString("DELETE FROM mcufamily WHERE id='%1'").arg(family.getId());
            QSqlQuery result = DataBase::instance()->sendQuery(queryStr, &status);

            if(!status)
            {
                errorStr = result.lastError().text();
            }
        }
    }

    if(!status)
    {
        logError(errorStr);
    }

    return status;
}

//------------------------------------------------------------------------------
// Удаление процессоров от выбранного производителя
//------------------------------------------------------------------------------
bool RequestManager::removeManufacturer(Manufacturer manufacturer)
{
    QString errorStr;
    bool status = true;

    if(manufacturer.getId() < 0)
    {
        status = false;
        errorStr = tr("Некорректный идентификатор производителя");
    }
    else
    {
        QList<Family> familyList;

        for(auto it = manufacturer.families().begin(); it != manufacturer.families().end(); ++it)
        {
            familyList.append(it.value());
        }

        for(int f = 0; f < familyList.count(); f++)
        {
            Family currFamily = familyList.at(f);

            if(!removeFamily(currFamily))
            {
                status = false;
                break;
            }
        }

        //Удаление  из таблицы
        if(status)
        {
            QString queryStr = QString("DELETE FROM mcumanufacturer WHERE id='%1'").arg(manufacturer.getId());
            QSqlQuery result = DataBase::instance()->sendQuery(queryStr, &status);

            if(!status)
            {
                errorStr = result.lastError().text();
            }
        }
    }

    if(!status)
    {
        logError(errorStr);
    }

    return status;
}

//------------------------------------------------------------------------------
// Удаление алгоритма отладки из базы
//------------------------------------------------------------------------------
bool RequestManager::removeDebugAlgorithm(DebugAlgorithm da)
{
    QString errorStr;
    bool status = true;

    if(da.coId() < 0)
    {
        status = false;
        errorStr = tr("Некорректный идентификатор алгоритма");
    }
    else
    {
        //Удаление  из таблицы
        QString queryStr = QString("DELETE FROM debug_algorithm WHERE id='%1'").arg(da.coId());
        QSqlQuery result = DataBase::instance()->sendQuery(queryStr, &status);

        if(!status)
        {
            errorStr = result.lastError().text();
        }
    }

    if(!status)
    {
        logError(errorStr);
    }

    return status;
}

//------------------------------------------------------------------------------
// Удаление алгоритма программирования из базы
//------------------------------------------------------------------------------
bool RequestManager::removeFlashAlgorithm(ProgAlgorithm fa)
{
    QString errorStr;
    bool status = true;

    if(fa.coId() < 0)
    {
        status = false;
        errorStr = tr("Некорректный идентификатор алгоритма");
    }
    else
    {
        //Удаление  из таблицы
        QString queryStr = QString("DELETE FROM flash_algorithm WHERE id='%1'").arg(fa.coId());
        QSqlQuery result = DataBase::instance()->sendQuery(queryStr, &status);

        if(!status)
        {
            errorStr = result.lastError().text();
        }
    }

    if(!status)
    {
        logError(errorStr);
    }

    return status;
}

//------------------------------------------------------------------------------
// Обновить информацию о процессоре
//------------------------------------------------------------------------------
bool RequestManager::updateMcuInfo(Mcu mcu)
{
    QString errorStr;
    bool status = true;

    if(mcu.getId() < 0)
    {
        status = false;
        errorStr = tr("Некорректный идентификатор процессора");
    }
    else
    {
        //Обновление записи
        QString queryStr = QString("UPDATE mcu SET "
                                   "debugAlgorithmId = '%1', "
                                   "description = '%2', "
                                   "keyParameter = '%3', "
                                   "webPageURL = '%4', "
                                   "datasheetURL = '%5', "
                                   "memInfo = '%6' WHERE "
                                   "id = %7;").
                            arg(mcu.getDebugAlgorithm().coId()).
                            arg(mcu.getDescription()).
                            arg(QString(mcu.getKeyParameter())).
                            arg(QString(mcu.getWebPageURL())).
                            arg(QString(mcu.getDatasheetURL())).
                            arg(QString(mcu.getMemInfo())).
                            arg(mcu.getId());

        QSqlQuery result = DataBase::instance()->sendQuery(queryStr, &status);

        if(!status)
        {
            errorStr = result.lastError().text();
        }
        else
        {
            ProgAlgorithm fa = getMcuFlashAlgorithm(mcu.getId());

            if(fa.coId() > -1)
            {
                //Обновляем алгоритм программирования
                queryStr = QString("UPDATE mcu_has_flash_algorithm SET "
                                   "flashAlgorithmId = '%1' WHERE "
                                   "mcuId = %2;").
                            arg(mcu.getFlashAlgorithm()->coId()).
                            arg(mcu.getId());

                result = DataBase::instance()->sendQuery(queryStr, &status);

                if(!status)
                {
                    errorStr = result.lastError().text();
                }
            }
            //СОздаем новый алгоритм программирования
            else
            {
                //Обновляем алгоритм программирования
                queryStr = QString("INSERT INTO mcu_has_flash_algorithm VALUES "
                                   "('%1', '%2');").
                                    arg(mcu.getId()).
                                    arg(mcu.getFlashAlgorithm()->coId());

                result = DataBase::instance()->sendQuery(queryStr, &status);

                if(!status)
                {
                    errorStr = result.lastError().text();
                }
            }
        }
    }

    if(!status)
    {
        logError(errorStr);
    }

    return status;
}

//------------------------------------------------------------------------------
// Исправляет в БД имена вендоров и их идентификаторы и приводит к стандарту Keil
//------------------------------------------------------------------------------
bool RequestManager::fixVendorIDs(QString& errorString)
{
    // Обновление таблицы `mcumanufacturer`
    // Обновление таблицы `mcufamily`
    // Обновление таблицы `component_supports_mcumanufacturer`
    // Обновление таблицы `board_supports_mcumanufacturer`
    return fixManufacturerTable(errorString) &&
           fixFamilyTable(errorString) &&
           fixComponentManufacturerTable(errorString) &&
           fixBoardManufacturerTable(errorString);

}

//------------------------------------------------------------------------------
// Формирует SQL-запрос на обновление данных вендора в БД
//------------------------------------------------------------------------------
bool RequestManager::updateVendorName(const Manufacturer manufacturer,
                                      QString &errorString)
{
    bool status;
    QString queryStr = QString("UPDATE mcumanufacturer SET "
                               "id = '%1', "
                               "name = '%2' WHERE "
                               "id = %1;").
                        arg(manufacturer.getId()).
                        arg(manufacturer.getName());

    QSqlQuery result = DataBase::instance()->sendQuery(queryStr, &status);

    if(!status)
    {
        errorString = result.lastError().text();
    }

    return status;
}

bool RequestManager::updateManufacturerTable(int id,
                                             int newId,
                                             const QString &newName,
                                             QString &errorString)
{
    bool status;
    QString queryStr = QString("UPDATE mcumanufacturer SET "
                               "id = '%2', "
                               "name = '%3' WHERE "
                               "id = %1;").
                        arg(id).
                        arg(newId).
                        arg(newName);

    QSqlQuery result = DataBase::instance()->sendQuery(queryStr, &status);

    if(!status)
    {
        errorString = result.lastError().text();
    }

    return status;
}


//------------------------------------------------------------------------------
// Формирует SQL-запрос на обновление ID вендора в таблице Family
//------------------------------------------------------------------------------
bool RequestManager::updateFamilyTable(const Family &family,
                                       int vendorId,
                                       QString &errorString)
{
    bool status;
    QString queryStr = QString("UPDATE mcufamily SET "
                               "id = '%1', "
                               "familyName = '%2', "
                               "manufacturerId = '%3' WHERE "
                               "id = %1;").
                        arg(family.getId()).
                        arg(family.getName()).
                        arg(vendorId);

    QSqlQuery result = DataBase::instance()->sendQuery(queryStr, &status);

    if(!status)
    {
        errorString = result.lastError().text();
    }

    return status;
}

//------------------------------------------------------------------------------
// Обновление информации о производителе платы
//------------------------------------------------------------------------------
bool RequestManager::updateBoardManufacturerTable(int boardId,
                                                  int vendorId,
                                                  QString &errorString)
{
    bool status;
    QString queryStr = QString("UPDATE board_supports_mcumanufacturer SET "
                               "boardId = '%1', "
                               "mcuManufacturerId = '%2' WHERE "
                               "boardId = %1;").
                        arg(boardId).
                        arg(vendorId);

    QSqlQuery result = DataBase::instance()->sendQuery(queryStr, &status);

    if(!status)
    {
        errorString = result.lastError().text();
        return false;
    }

    return true;
}

//------------------------------------------------------------------------------
// Исправление ID вендора для платы
//------------------------------------------------------------------------------
bool RequestManager::fixBoardManufacturerTable(QString &errorString)
{
    struct Board
    {
        int boardId;
        int mcuManufacturerId;
    };

    bool status;
    QList<Board> boards;
    QSqlQuery result = DataBase::instance()->sendQuery("SELECT `boardId`, `mcuManufacturerId` "
                                                       "FROM board_supports_mcumanufacturer",
                                                       &status);

    if(!status)
    {
        errorString = result.lastError().text();
        return false;
    }

    while(result.next())
    {
        Board b;
        b.boardId = result.value(0).toInt();
        b.mcuManufacturerId = result.value(1).toInt();

        boards.append(b);
    }

    foreach(Board b, boards)
    {
        if(b.mcuManufacturerId < 1000)
        {
            int vendorId = Manufacturer::co2keilId(b.mcuManufacturerId);

            if(!updateBoardManufacturerTable(b.boardId, vendorId, errorString))
            {
                return false;
            }
        }
    }

    return true;
}

//------------------------------------------------------------------------------
// Исправление ID и имен производителей в таблице mcumanufacturer
//------------------------------------------------------------------------------
bool RequestManager::fixManufacturerTable(QString &errorString)
{
    QList<Manufacturer> manufacturers = requestManufacturerList();

    foreach(Manufacturer m, manufacturers)
    {
        if(m.getId() < 1000)
        {
            if(!updateManufacturerTable(m.getId(), m.toKeilId(), m.toKeilName(), errorString))
                return false;
        }
    }

    return true;
}

//------------------------------------------------------------------------------
// Исправление ID производителей в таблице mcufamily
//------------------------------------------------------------------------------
bool RequestManager::fixFamilyTable(QString &errorString)
{
    QList<Family> families = requestFamilyList();

    foreach(Family f, families)
    {
        if(f.getManufacturerId() < 1000)
        {
            int vendorId = Manufacturer::co2keilId(f.getManufacturerId());

            if(!updateFamilyTable(f, vendorId, errorString))
                return false;
        }
    }

    return true;
}

