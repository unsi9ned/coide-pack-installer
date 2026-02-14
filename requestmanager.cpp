#include "requestmanager.h"
#include "componentsinfo.h"
#include "logger.h"

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

    connect(DataBase::instance(),
            SIGNAL(errorOccured(QString)),
            SIGNAL(errorOccured(QString)));

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
// Загрузка всех данных о MCU из базы данных
//------------------------------------------------------------------------------
void RequestManager::loadDataFromDb(QMap<QString, Manufacturer> &vendorMap)
{
    vendorMap.clear();

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
                QList<Mcu> mcuList = requestMcuList(serie);

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
QList<Mcu> RequestManager::requestMcuList(Series serie)
{
    QList<Mcu> microcontrollers;
    QString queryStr = QString("SELECT * FROM mcu "
                               "WHERE seriesId=%1").arg(serie.getId());
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

        memInfo = memInfo.replace("\\", "");
        memInfo = memInfo.replace("\"{", "{");
        memInfo = memInfo.replace("}\"", "}");

        QByteArray ba;
        ba.append("{\"memInfo\" : ");
        ba.append(memInfo);
        ba.append("}");

        memInfo = ba;


        Mcu mcu;
        mcu.setId(id);
        mcu.setSeriesId(seriesId);
        mcu.setUserId(userId);
        mcu.setDebugAlgorithmId(debugAlgorithmId);
        mcu.setName(name);
        mcu.setDescription(description);
        mcu.setKeyParameter(keyParameter);
        mcu.setWebPageURL(webPageURL);
        mcu.setDatasheetURL(datasheetURL);
        mcu.setMemInfo(memInfo);
        mcu.setMicro(micro);
        mcu.setAdvertising(advertising);
        mcu.setPrice(price);
        mcu.setTimeuuid(timeuuid);
        mcu.setHits(hits);

        DebugAlgorithm da = requestDebugAlgorithm(debugAlgorithmId);
        mcu.setDebugAlgorithm(da);

        ProgAlgorithm fa = getMcuFlashAlgorithm(id);
        mcu.setFlashAlgorithm(fa);

        microcontrollers.append(mcu);
    }

    return microcontrollers;
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

        da.setCoId(id);
        da.setName(name);

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

        da.setCoId(id);
        da.setName(name);
        break;
    }

    return da;
}

//------------------------------------------------------------------------------
// Загрузить из базы алгоритм программирования
//------------------------------------------------------------------------------
ProgAlgorithm RequestManager::requestFlashAlgorithm(int id)
{
    ProgAlgorithm fa;

    QString queryStr = QString("SELECT * FROM flash_algorithm "
                               "WHERE id=%1 LIMIT 1").arg(id);
    QSqlQuery result = DataBase::instance()->sendQuery(queryStr);

    while(result.next())
    {
        id = result.value(0).toInt();
        QString name = result.value(1).toString();

        fa.setCoId(id);
        fa.setName(name);
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

    QString queryStr = QString("SELECT * FROM flash_algorithm");
    QSqlQuery result = DataBase::instance()->sendQuery(queryStr);

    while(result.next())
    {
        ProgAlgorithm fa;

        int id = result.value(0).toInt();
        QString name = result.value(1).toString();

        fa.setCoId(id);
        fa.setName(name);

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
        emit errorOccured(errorStr);
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
               currFam.getName().trimmed().toLower() == family.getName().trimmed().toLower())
            {
                family.setId(currFam.getId());
                isset = true;
                break;
            }
        }

        if(!isset)
        {
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
                                    arg(family.getName()).
                                    arg(family.getManufacturerId());

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
        emit errorOccured(errorStr);
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
    }

    if(!status)
    {
        emit errorOccured(errorStr);
    }

    return status;
}

//------------------------------------------------------------------------------
// Создать новый микроконтроллер
//------------------------------------------------------------------------------
bool RequestManager::createMcu(Manufacturer man,
                        Family fam,
                        Series ser,
                        QString mcuName)
{
    QString errorStr;
    bool status = true;

    if(mcuName.isEmpty())
    {
        status = false;
        errorStr = tr("Название контроллера имеет нулевую длину");
    }
    else if(man.getId() < 0 || fam.getId() < 0 || ser.getId() < 0)
    {
        status = false;
        errorStr = tr("Некорректный идентификатор производителя, семейства или серии");
    }
    else
    {
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

            QList<Mcu> mcuList = requestMcuList(ser);

            for(int m = 0; m < mcuList.length(); m++)
            {
                Mcu currMcu = mcuList.at(m);

                if(currMcu.getName().trimmed().toLower() ==
                   mcuName.trimmed().toLower())
                {
                    isset = true;
                }
            }

            if(!isset)
            {
                QString memInfo = "[\"{\\\"type\\\":\\\"FLASH\\\",\\\"id\\\":0,\\\"start\\\":\\\"0x08000000\\\",\\\"size\\\":\\\"0x00020000\\\"}\",\"{\\\"type\\\":\\\"RAM\\\",\\\"id\\\":0,\\\"start\\\":\\\"0x20000000\\\",\\\"size\\\":\\\"0x00008000\\\"}\"]";

                QString queryStr = QString("INSERT INTO mcu "
                                           "VALUES ('%1','%2','%3','%4','%5','%6','%7','%8','%9','%10','%11','%12','%13','%14','%15')").
                                    arg(lastId + 1).
                                    arg(ser.getId()).
                                    arg(USER_ID_COOCOX).
                                    arg(1).
                                    arg(mcuName).
                                    arg("Desription").
                                    arg("[\"Features\"]").
                                    arg("[\"http://google.com/\"]").
                                    arg("[\"http://google.com/datasheet\"]").
                                    arg(memInfo).
                                    arg(ser.getName()).
                                    arg("0").
                                    arg("0").
                                    arg("ae08a837-c4ed-4374-b372-caafd3dcf3cd").
                                    arg("0");

                QSqlQuery result = DataBase::instance()->sendQuery(queryStr, &status);

                if(!status)
                {
                    errorStr = result.lastError().text();
                }
            }
            else
            {
                status = false;
                errorStr = tr("Микроконтроллер с такой маркировкой уже существует");
            }
        }
    }

    if(!status)
    {
        emit errorOccured(errorStr);
    }

    return status;
}

//------------------------------------------------------------------------------
// Добавить алгоритм отладки в базу
//------------------------------------------------------------------------------
bool RequestManager::createDebugAlgorithm(QString nameAlg)
{
    QString errorStr;
    bool status = true;

    if(nameAlg.isEmpty())
    {
        status = false;
        errorStr = tr("Название алгоритма имеет нулевую длину");
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

            if(da.name().trimmed().toLower() == nameAlg.trimmed().toLower())
            {
                isset = true;
            }
        }

        if(isset)
        {
            status = false;
            errorStr = tr("Алгоритм %1 уже есть в базе").arg(nameAlg);
        }
        else
        {
            QString queryStr = QString("INSERT INTO debug_algorithm "
                                       "VALUES ('%1','%2','%3','%4','%5','%6','%7')").
                                arg(lastId + 1).
                                arg(nameAlg).
                                arg("").
                                arg("2012-10-30 18:09:28.0").
                                arg("2012-10-30 18:09:28.0").
                                arg("da181949-d3d0-41f6-b16c-956433f00e0b").
                                arg(3);

            QSqlQuery result = DataBase::instance()->sendQuery(queryStr, &status);

            if(!status)
            {
                errorStr = result.lastError().text();
            }
        }
    }

    if(!status)
    {
        emit errorOccured(errorStr);
    }

    return status;
}

//------------------------------------------------------------------------------
// Добавить алгоритм программирования в базу
//------------------------------------------------------------------------------
bool RequestManager::createFlashAlgorithm(QString nameAlg)
{
    QString errorStr;
    bool status = true;

    if(nameAlg.isEmpty())
    {
        status = false;
        errorStr = tr("Название алгоритма имеет нулевую длину");
    }
    else
    {
        //Загружаем алгоритмы из базы
        bool isset = false;
        int lastId = 0;
        QList<ProgAlgorithm> faList = requestFlashAlgorithmList();

        for(int a = 0; a < faList.length(); a++)
        {
            ProgAlgorithm fa = faList.at(a);

            if(fa.coId() > lastId)
            {
                lastId = fa.coId();
            }

            if(fa.name().trimmed().toLower() == nameAlg.trimmed().toLower())
            {
                isset = true;
            }
        }

        if(isset)
        {
            status = false;
            errorStr = tr("Алгоритм %1 уже есть в базе").arg(nameAlg);
        }
        else
        {
            QString queryStr = QString("INSERT INTO flash_algorithm "
                                       "VALUES ('%1','%2','%3','%4','%5','%6','%7','%8','%9')").
                                arg(lastId + 1).
                                arg(nameAlg).
                                arg("").
                                arg("").
                                arg("").
                                arg("2012-10-30 18:09:28.0").
                                arg("2012-10-30 18:09:28.0").
                                arg("da181949-d3d0-41f6-b16c-956433f00e0b").
                                arg(2);

            QSqlQuery result = DataBase::instance()->sendQuery(queryStr, &status);

            if(!status)
            {
                errorStr = result.lastError().text();
            }
        }
    }

    if(!status)
    {
        emit errorOccured(errorStr);
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
        emit errorOccured(errorStr);
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
        emit errorOccured(errorStr);
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
        emit errorOccured(errorStr);
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
        emit errorOccured(errorStr);
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
        emit errorOccured(errorStr);
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
        emit errorOccured(errorStr);
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
        emit errorOccured(errorStr);
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

