#include "mcuinfo.h"
#include "logger.h"

QString McuInfo::getIdePath() const
{
    return idePath;
}

McuInfo::McuInfo(DataBase * db, QObject *parent) : QObject(parent),
    idePath(IDE_PATH),
    db(db)
{
    loadIdePath();

    connect(db,
            SIGNAL(errorOccured(QString)),
            SIGNAL(errorOccured(QString)));

    connect(db,
            SIGNAL(dbConnected()),
            this,
            SLOT(saveIdePath()));


    if(db->isOpen())
    {
        loadDataFromDb();
    }
    else
    {
        connect(db,
                SIGNAL(dbConnected()),
                this,
                SLOT(loadDataFromDb()));
    }
}

McuInfo::~McuInfo()
{
    manufacturers.clear();
}

//------------------------------------------------------------------------------
// Запросить список производителей
//------------------------------------------------------------------------------
QList<Manufacturer> McuInfo::requestManufacturerList()
{
    QList<Manufacturer> manufact;
    QSqlQuery result = db->sendQuery("SELECT * FROM mcumanufacturer");

    while(result.next())
    {
        int id = result.value(0).toInt();
        QString name = result.value(1).toString();

        manufact.append(Manufacturer(id, name));
    }

    return manufact;
}

//------------------------------------------------------------------------------
// Запросить список семейств
//------------------------------------------------------------------------------
QList<Family> McuInfo::requestFamilyList(Manufacturer manufact)
{
    QList<Family> fam;
    QString queryStr = QString("SELECT * FROM mcufamily "
                               "WHERE manufacturerId=%1").arg(manufact.getId());
    QSqlQuery result = db->sendQuery(queryStr);

    while(result.next())
    {
        int id = result.value(0).toInt();
        QString familyName = result.value(1).toString();

        fam.append(Family(id, familyName, manufact.getId()));
    }

    return fam;
}

//------------------------------------------------------------------------------
// Запросить список семейств
//------------------------------------------------------------------------------
QList<Family> McuInfo::requestFamilyList()
{
    QList<Family> fam;
    QString queryStr = QString("SELECT * FROM mcufamily");
    QSqlQuery result = db->sendQuery(queryStr);

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
QList<Series> McuInfo::requestSeriesList(Family fam)
{
    QList<Series> series;
    QString queryStr = QString("SELECT * FROM mcuseries "
                               "WHERE familyId=%1").arg(fam.getId());
    QSqlQuery result = db->sendQuery(queryStr);

    while(result.next())
    {
        int id = result.value(0).toInt();
        QString name = result.value(1).toString();

        series.append(Series(id, name, fam.getId()));
    }

    return series;
}

//------------------------------------------------------------------------------
// Запросить список микроконтроллеров
//------------------------------------------------------------------------------
QList<Mcu> McuInfo::requestMcuList(Series serie)
{
    QList<Mcu> microcontrollers;
    QString queryStr = QString("SELECT * FROM mcu "
                               "WHERE seriesId=%1").arg(serie.getId());
    QSqlQuery result = db->sendQuery(queryStr);

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
QList<DebugAlgorithm> McuInfo::requestDebugAlgorithmList()
{
    QList<DebugAlgorithm> daList;

    QString queryStr = QString("SELECT * FROM debug_algorithm");
    QSqlQuery result = db->sendQuery(queryStr);

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
DebugAlgorithm McuInfo::requestDebugAlgorithm(int id)
{
    DebugAlgorithm da;

    QString queryStr = QString("SELECT * FROM debug_algorithm "
                               "WHERE id=%1 LIMIT 1").arg(id);
    QSqlQuery result = db->sendQuery(queryStr);

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
ProgAlgorithm McuInfo::requestFlashAlgorithm(int id)
{
    ProgAlgorithm fa;

    QString queryStr = QString("SELECT * FROM flash_algorithm "
                               "WHERE id=%1 LIMIT 1").arg(id);
    QSqlQuery result = db->sendQuery(queryStr);

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
QList<ProgAlgorithm> McuInfo::requestFlashAlgorithmList()
{
    QList<ProgAlgorithm> faList;

    QString queryStr = QString("SELECT * FROM flash_algorithm");
    QSqlQuery result = db->sendQuery(queryStr);

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
ProgAlgorithm McuInfo::getMcuFlashAlgorithm(int mcuId)
{
    ProgAlgorithm fa;
    int faId = -1;

    QString queryStr = QString("SELECT * FROM mcu_has_flash_algorithm "
                               "WHERE mcuId=%1 LIMIT 1").arg(mcuId);
    QSqlQuery result = db->sendQuery(queryStr);

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
// Загрузить данные из базы по контроллерам
//------------------------------------------------------------------------------
void McuInfo::loadDataFromDb()
{
    if(db->isOpen())
    {
        disconnect(db,
                   SIGNAL(dbConnected()),
                   this,
                   SLOT(loadDataFromDb()));
    }

    this->manufacturers.clear();

    QList<Manufacturer> manufacturers = requestManufacturerList();

    for(int i = 0; i < manufacturers.length(); i++)
    {
        Manufacturer * man = &manufacturers[i];
        QList<Family> families = requestFamilyList(*man);

        for(int f = 0; f < families.length(); f++)
        {
            Family fam = families.at(f);
            QList<Series> series = requestSeriesList(fam);

            for(int s = 0 ; s < series.length(); s++)
            {
                Series serie = series.at(s);
                QList<Mcu> mcuList = requestMcuList(serie);

                for(int m = 0; m < mcuList.length(); m++)
                {
                    Mcu mcu = mcuList.at(m);
                    serie.addMcu(mcu.getName()) = mcu;
                }
                fam.addSeries(serie.getName()) = serie;
            }
            man->addFamily(fam.getName()) = fam;
        }
    }
    this->manufacturers.append(manufacturers);

    // Загрузка алгоритмов отладки и программирования из базы
    loadAlgorithmFromDb();

    emit infoReady();
}

//------------------------------------------------------------------------------
// Загрузка алгоритмов отладки и программирования из базы
//------------------------------------------------------------------------------
void McuInfo::loadAlgorithmFromDb()
{
    //Загрузка алгоритмов отладки
    this->debugAlgList.clear();
    this->debugAlgList.append(this->requestDebugAlgorithmList());

    //Загрузка алгоритмов программирования
    this->flashAlgList.clear();
    this->flashAlgList.append(this->requestFlashAlgorithmList());

    emit algInfoReady();
}

//------------------------------------------------------------------------------
// Сохранить путь к среде разработки
//------------------------------------------------------------------------------
void McuInfo::saveIdePath()
{
    QString path = this->idePath;
    QFile confFile(CONFIG_FILE);

    if(confFile.open(QFile::WriteOnly))
    {
        confFile.write(QByteArray().append(path));
        confFile.close();
    }
    else
    {
        emit errorOccured(tr("Не удалось создать файл %1").arg(confFile.fileName()));
    }
}

//------------------------------------------------------------------------------
// Загрузить из файла путь к среде разработки
//------------------------------------------------------------------------------
void McuInfo::loadIdePath()
{
    QFile file(CONFIG_FILE);
    QString path = IDE_PATH;

    if(file.exists())
    {
        if(file.open(QFile::ReadOnly))
        {
            QByteArray ba = file.readAll();
            path = QString(ba).trimmed();
            file.close();

            Logger::instance()->addEvent(QString("Reading the configuration file '%1'").arg(CONFIG_FILE));

            changeIdePath(path);
        }
    }
    else
        Logger::instance()->addEvent(QString("The configuration file '%1' was not found").arg(CONFIG_FILE));

    this->idePath = path;
}

//------------------------------------------------------------------------------
// Создать нового производителя
//------------------------------------------------------------------------------
bool McuInfo::createManufacturer(QString newMan)
{
    QString errorStr;
    bool status = true;

    if(newMan.isEmpty())
    {
        status = false;
        errorStr = tr("Название проиводителя имеет нулевую длину");
    }
    else
    {
        //Загружаем производителей из базы и смотрим нет ли уже такого
        //Заодно определяем последний id
        bool isset = false;
        int lastId = 0;
        QList<Manufacturer> manufacturers = requestManufacturerList();

        for(int m = 0; m < manufacturers.length(); m++)
        {
            Manufacturer man = manufacturers.at(m);

            if(man.getId() > lastId)
            {
                lastId = man.getId();
            }

            if(man.getName().trimmed().toLower() == newMan.trimmed().toLower())
            {
                isset = true;
            }
        }

        if(isset)
        {
            status = false;
            errorStr = tr("Производитель с таким названием уже есть в базе");
        }
        else
        {
            QString queryStr = QString("INSERT INTO mcumanufacturer "
                                       "VALUES ('%1','%2')").
                                arg(lastId + 1).
                                arg(newMan);

            QSqlQuery result = db->sendQuery(queryStr, &status);

            if(!status)
            {
                errorStr = result.lastError().text();
            }
            else
            {
                //Перезагружаем данные о контроллерах
                loadDataFromDb();
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
bool McuInfo::createFamily(Manufacturer manufacturer, QString newFamily)
{
    QString errorStr;
    bool status = true;

    if(newFamily.isEmpty())
    {
        status = false;
        errorStr = tr("Название семейства имеет нулевую длину");
    }
    else if(manufacturer.getId() < 0)
    {
        status = false;
        errorStr = tr("Некорректный идентификатор производителя");
    }
    else
    {
        //Загружаем производителей из базы и смотрим нет ли уже такого
        //Заодно определяем последний id
        bool isset = false;
        int lastId = 0;
        QList<Family> families = requestFamilyList();

        for(int f = 0; f < families.length(); f++)
        {
            Family currFam = families.at(f);

            if(currFam.getId() > lastId)
            {
                lastId = currFam.getId();
            }
            if((currFam.getManufacturerId() == manufacturer.getId()) &&
                newFamily.trimmed().toLower() == currFam.getName().trimmed().toLower())
            {
                isset = true;
            }
        }

        if(!isset)
        {
            QString queryStr = QString("INSERT INTO mcufamily "
                                       "VALUES ('%1','%2','%3')").
                                arg(lastId + 1).
                                arg(newFamily).
                                arg(manufacturer.getId());

            QSqlQuery result = db->sendQuery(queryStr, &status);

            if(!status)
            {
                errorStr = result.lastError().text();
            }
            else
            {
                //Перезагружаем данные о контроллерах
                loadDataFromDb();
            }
        }
        else
        {
            status = false;
            errorStr = tr("Выбранный производитель уже содержит такое семейство");
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
bool McuInfo::createSerie(Manufacturer man, Family fam, QString newSerie)
{
    QString errorStr;
    bool status = true;

    if(newSerie.isEmpty())
    {
        status = false;
        errorStr = tr("Название серии имеет нулевую длину");
    }
    else if(man.getId() < 0 || fam.getId() < 0)
    {
        status = false;
        errorStr = tr("Некорректный идентификатор производителя или семейства");
    }
    else
    {
        bool isset = false;
        int lastId = -1;

        //Поиск последнего айди
        QString queryStr = QString("SELECT MAX(id) FROM mcuseries");
        QSqlQuery result = db->sendQuery(queryStr, &status);

        if(!status || !result.next())
        {
            errorStr = result.lastError().text();
        }
        else
        {
            lastId = result.value(0).toInt();

            QList<Series> series = requestSeriesList(fam);

            for(int s = 0; s < series.length(); s++)
            {
                Series currSerie = series.at(s);

                if(currSerie.getName().trimmed().toLower() ==
                   newSerie.trimmed().toLower())
                {
                    isset = true;
                }
            }

            if(!isset)
            {
                QString queryStr = QString("INSERT INTO mcuseries "
                                           "VALUES ('%1','%2','%3')").
                                    arg(lastId + 1).
                                    arg(newSerie).
                                    arg(fam.getId());

                QSqlQuery result = db->sendQuery(queryStr, &status);

                if(!status)
                {
                    errorStr = result.lastError().text();
                }
                else
                {
                    //Перезагружаем данные о контроллерах
                    loadDataFromDb();
                }
            }
            else
            {
                status = false;
                errorStr = tr("Выбранный производитель уже содержит такую серию");
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
bool McuInfo::createMcu(Manufacturer man,
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
        QSqlQuery result = db->sendQuery(queryStr, &status);

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

                QSqlQuery result = db->sendQuery(queryStr, &status);

                if(!status)
                {
                    errorStr = result.lastError().text();
                }
                else
                {
                    //Перезагружаем данные о контроллерах
                    loadDataFromDb();
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
bool McuInfo::createDebugAlgorithm(QString nameAlg)
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

            QSqlQuery result = db->sendQuery(queryStr, &status);

            if(!status)
            {
                errorStr = result.lastError().text();
            }
            else
            {
                //Перезагружаем данные об алгоритмах
                loadAlgorithmFromDb();
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
bool McuInfo::createFlashAlgorithm(QString nameAlg)
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

            QSqlQuery result = db->sendQuery(queryStr, &status);

            if(!status)
            {
                errorStr = result.lastError().text();
            }
            else
            {
                //Перезагружаем данные об алгоритмах
                loadAlgorithmFromDb();
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
bool McuInfo::removeMcu(Mcu mcu)
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
        QSqlQuery result = db->sendQuery(queryStr, &status);

        if(!status)
        {
            errorStr = result.lastError().text();
        }
        else
        {
            //Перезагружаем данные о контроллерах
            loadDataFromDb();
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
bool McuInfo::removeSerie(Series serie)
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
            QSqlQuery result = db->sendQuery(queryStr, &status);

            if(!status)
            {
                errorStr = result.lastError().text();
            }
        }

        //Перезагружаем данные о контроллерах
        loadDataFromDb();
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
bool McuInfo::removeFamily(Family family)
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
            QSqlQuery result = db->sendQuery(queryStr, &status);

            if(!status)
            {
                errorStr = result.lastError().text();
            }
        }

        //Перезагружаем данные о контроллерах
        loadDataFromDb();
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
bool McuInfo::removeManufacturer(Manufacturer manufacturer)
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
            QSqlQuery result = db->sendQuery(queryStr, &status);

            if(!status)
            {
                errorStr = result.lastError().text();
            }
        }

        //Перезагружаем данные о контроллерах
        loadDataFromDb();
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
bool McuInfo::removeDebugAlgorithm(DebugAlgorithm da)
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
        QSqlQuery result = db->sendQuery(queryStr, &status);

        if(!status)
        {
            errorStr = result.lastError().text();
        }

        //Перезагружаем данные об алгоритмах
        loadAlgorithmFromDb();
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
bool McuInfo::removeFlashAlgorithm(ProgAlgorithm fa)
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
        QSqlQuery result = db->sendQuery(queryStr, &status);

        if(!status)
        {
            errorStr = result.lastError().text();
        }

        //Перезагружаем данные об алгоритмах
        loadAlgorithmFromDb();
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
bool McuInfo::updateMcuInfo(Mcu mcu)
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

        QSqlQuery result = db->sendQuery(queryStr, &status);

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

                result = db->sendQuery(queryStr, &status);

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

                result = db->sendQuery(queryStr, &status);

                if(!status)
                {
                    errorStr = result.lastError().text();
                }
            }
        }

        //Перезагружаем данные
        loadDataFromDb();
    }

    if(!status)
    {
        emit errorOccured(errorStr);
    }

    return status;
}

//------------------------------------------------------------------------------
// Поиск алгоритмов отладки, которые не прописаны в базе
//------------------------------------------------------------------------------
void McuInfo::searchNewDebugAlgorithm()
{
    QDir algDir(idePath + DEBUGGER_DIR);

    this->newDebugAlgList.clear();

    if(algDir.exists())
    {
        QStringList extension;
        extension << "*.xml" << ".XML";

        QStringList fileList = algDir.entryList(extension, QDir::Files, QDir::Name);

        foreach (QString file, fileList)
        {
            bool exist = false;

            foreach(DebugAlgorithm currDa, this->debugAlgList)
            {
                if(currDa.name().toLower() == file.toLower())
                {
                    exist = true;
                    break;
                }
            }

            if(!exist)
            {
                DebugAlgorithm newDA;
                newDA.setName(file);
                this->newDebugAlgList.append(newDA);
            }
        }
    }
}

//------------------------------------------------------------------------------
// Поиск алгоритмов программирования, которые не прописаны в базе
//------------------------------------------------------------------------------
void McuInfo::searchNewFlashAlgorithm()
{
    QDir algDir(idePath + FLASH_DIR);

    this->newFlashAlgList.clear();

    if(algDir.exists())
    {
        QStringList extension;
        extension << "*.elf" << "*.ELF" << "*.a" << "*.A";

        QStringList fileList = algDir.entryList(extension, QDir::Files, QDir::Name);

        foreach (QString file, fileList)
        {
            bool exist = false;

            foreach(ProgAlgorithm currFa, this->flashAlgList)
            {
                if(currFa.name().toLower() == file.toLower())
                {
                    exist = true;
                    break;
                }
            }

            if(!exist)
            {
                ProgAlgorithm newFA;
                newFA.setName(file);
                this->newFlashAlgList.append(newFA);
            }
        }
    }
}
