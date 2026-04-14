#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QString>
#include <QSet>
#include "packmanager.h"
#include "services/paths.h"
#include "utils/ziparchive.h"
#include "pdscparser.h"
#include "jdscparser.h"
#include "models/mcu/manufacturer.h"
#include "models/database/requestmanager.h"
#include "utils/makelink.h"

PackManager::PackManager(QObject *parent) : QObject(parent)
{

}

//------------------------------------------------------------------------------
// Поиск в пакете файла описания и его разбор
//------------------------------------------------------------------------------
void PackManager::readPackDescription(PackDescription &pack)
{
    QString errorString;

    //
    // Распаковываем файл описания в каталог установки
    //
    logInfo("Extract Package Description File");

    if(!extractPackDescriptionFile(pack, errorString))
    {
        if(errorString.isEmpty())
        {
            logError("Couldn't extract Package Description File");
        }
        else
        {
            logError(QString("Couldn't extract Package Description File: %1").arg(errorString));
        }

        return;
    }

    //
    // Искусственно добавляем в описание компоненты CMSIS CORE
    //
    logInfo("Creating \"CMSIS CORE\" components");

    QMap<QString, QString> cmsisCores = Paths::instance()->cmsisCores();

    for(auto it = cmsisCores.begin(); it != cmsisCores.end(); ++it)
    {
        Component coComponent;
        Category coCategory = Category::categoryCommon();

        coCategory.setSubCategoryName("Device");

        coComponent.setLayerId(Component::LAYER_MCU);
        coComponent.setType(Component::COMPONENT);
        coComponent.setVersion(it.key());
        coComponent.setName(QString("CMSIS_Core_%1").arg(coComponent.getVersion()));
        coComponent.setCategory(coCategory);
        coComponent.setDescription(it.value());

        coComponent.setPdscVendor("ARM");
        coComponent.setPdscClass("CMSIS");
        coComponent.setPdscGroup("CORE");
        coComponent.setPdscVersion(coComponent.getVersion());

        //
        // Чтение списка файлов в арвхиве и распаковка
        //
        QFile cmsisZip(Paths::instance()->cmsisCore(coComponent.getVersion()));

        if(cmsisZip.exists())
        {
            QList<ZipArchive::ArchiveEntry> files = ZipArchive().listContents(cmsisZip.fileName());

            foreach(ZipArchive::ArchiveEntry f, files)
            {
                if(!f.isDir)
                {
                    coComponent.files().append("header=" + f.fullPath.replace('/', '\\'));
                }
            }
        }
        else
        {
            logError(QString("The %1 file was not found").arg(cmsisZip.fileName()));
            return;
        }

        pack.coComponentMap().insert(coComponent.getUuid(), coComponent);
        pack.cmsisComponents().insert(coComponent.getVersion(), &pack.coComponentMap()[coComponent.getUuid()]);
    }

    //
    // Разбор файла описания
    //
    logInfo("Starts parsing the package description file");

    PackDescriptionParser * parser;

    if(pack.archiveInfo().isPdsc())
        parser = new PdscParser();
    else if(pack.archiveInfo().isJdsc())
        parser = new JdscParser();
    else
    {
        logError("The selected package type is not valid");
        return;
    }

    if(parser->parse(pack))
    {
        // PDSC не содержит описание устройств. Загружаем их из базы данных
        if(!pack.hasDevices() && !pack.packVendor().isEmpty())
        {
            QStringList supportVendors;

            logInfo("Loading device data from the database");

            if(pack.archiveInfo().isPdsc())
            {
                for(auto& pComponent : pack.pdscComponentList())
                {
                    QStringList currentVendorList = pComponent.supportVendors();

                    QSet<QString> set = QSet<QString>::fromList(supportVendors);
                    set.unite(QSet<QString>::fromList(currentVendorList));
                    supportVendors = set.toList();
                }
            }
            else if(pack.archiveInfo().isJdsc())
            {
                supportVendors.append(pack.packVendor());
            }

            QMap<QString, Component> componentMap;
            RequestManager::instance()->loadDataFromDb(supportVendors, pack.vendors());
            RequestManager::instance()->requestComponentMap(pack.vendors(), componentMap);
            //pack.coComponentMap().unite(componentMap);
            parser->reloadComponents(pack);
        }

        updatePaths(pack);
        logInfo("The package description file has been parsed");
    }
    else
    {
        logError("Packet parsing error");
    }

    delete parser;
}

//------------------------------------------------------------------------------
// Установка пакета
//------------------------------------------------------------------------------
bool PackManager::packInstall(PackDescription &pack, QString& errorString)
{
    //
    // Проверка валидности устанавливаемого пакета
    //
    if(!pack.isValid())
    {
        logError(QString("The '%1' package is not valid").arg(pack.name()));
        return false;
    }

    logInfo(QString("The '%1' package is being installed").arg(pack.name()));

    //
    // Подготовка каталога пакета
    //
    QDir packInstallDir;
    Manufacturer vendor = pack.vendors().first();
    QString path = Paths::instance()->coIdePackDir(vendor.getName(), pack.release());

    packInstallDir.setPath(path);
    pack.setInstallDir(path);

    //
    // Создание каталога установки
    //
    logInfo(QString("Creating an installation directory"));

    if(!packInstallDir.exists() && !packInstallDir.mkpath(packInstallDir.path()))
    {
        logError("The package directory cannot be created");
        return false;
    }

    //
    // Распаковка файла описание аппаратуры в каталог установки
    //
    logInfo(QString("Extracting a pdsc file"));

    if(!extractPackDescriptionFile(pack, errorString))
    {
        if(errorString.isEmpty())
            logError("Couldn't extract pdsc file");
        else
            logError(QString("Couldn't extract pdsc file: %1").arg(errorString));

        return false;
    }

    //
    // Распаковка SVD-файлов и добавление в TXT-базу
    //
    logInfo(QString("Extracting SVD files"));

    if(!extractSVD(pack, errorString))
    {
        if(errorString.isEmpty())
            logError("Couldn't extract SVD files");
        else
            logError(QString("Couldn't extract SVD files: %1").arg(errorString));

        return false;
    }

    //
    // Формирование базы данных SVD
    //
    logInfo(QString("Creating an SVD database"));

    if(!makeSvdDatabase(pack, errorString))
    {
        if(errorString.isEmpty())
            logError("An error occurred when creating the SVD database");
        else
            logError(QString("An error occurred when creating the SVD database: %1").arg(errorString));

        return false;
    }

    //
    // Распаковка FLM-файлов
    //
    logInfo(QString("Extracting FLM files"));

    if(!extractFLM(pack, errorString))
    {
        if(errorString.isEmpty())
            logError("Couldn't extract FLM files");
        else
            logError(QString("Couldn't extract FLM files: %1").arg(errorString));

        return false;
    }

    //
    // Распаковка исходников
    //
    logInfo("Unpacking sources");

    if(!extractSources(pack, errorString))
    {
        if(errorString.isEmpty())
            logError("Couldn't extract sources files");
        else
            logError(QString("Couldn't extract sources files: %1").arg(errorString));

        return false;
    }

    //
    // Загружаем из базы данных информацию об устройствах
    //
    RequestManager * reqManager = RequestManager::instance();

    logInfo("Correction of manufacturer IDs");

    if(!reqManager->fixVendorIDs(errorString))
    {
        if(errorString.isEmpty())
            logError("An error occurred during the fix of the manufacturer's ID");
        else
            logError(QString("An error occurred during the fix of the manufacturer's ID: %1").arg(errorString));

        return false;
    }

    //
    // Создаем новых производителей (или обновляем существующие)
    //
    for(auto it = pack.vendors().begin(); it != pack.vendors().end(); ++it)
    {
        Manufacturer& m = it.value();

        logInfo(QString("Adding the manufacturer %1").arg(m.getName()));

        if(!reqManager->createManufacturer(m))
            break;

        for(auto f = m.families().begin(); f != m.families().end(); ++f)
        {
            Family& fam = f.value();

            logInfo(QString("Adding the family %1").arg(fam.getName()));

            if(!reqManager->createFamily(fam))
                break;

            for(auto s = fam.seriesMap().begin(); s!= fam.seriesMap().end(); ++s)
            {
                Series& series = s.value();

                logInfo(QString("Adding the series %1").arg(series.getName()));

                if(!reqManager->createSeries(series))
                    break;

                for(auto dev = series.mcuMap().begin(); dev != series.mcuMap().end(); ++dev)
                {
                    Mcu& mcu = dev.value();
                    DebugAlgorithm& algorithm = mcu.getDebugAlgorithm();
                    ProgAlgorithm*  flashAlgorithm = mcu.getDefaultFlashAlgorithm();

                    logInfo(QString("Adding the Debug algorithm '%1'").arg(mcu.getDebugAlgorithm().name()));

                    if(!reqManager->createDebugAlgorithm(algorithm))
                        break;
                    else
                    {
                        mcu.setDebugAlgorithmId(algorithm.coId());
                        Paths::instance()->copyDebugAlgorithm(algorithm.name());
                    }

                    if(flashAlgorithm)
                    {
                        logInfo(QString("Adding the Flash algorithm '%1'").arg(flashAlgorithm->name()));

                        if(!reqManager->createFlashAlgorithm(*flashAlgorithm))
                            break;
                    }
                    else
                        logWarning("The default programming algorithm was not found");

                    logInfo(QString("Adding the MCU = %1").arg(mcu.getName()));

                    if(!reqManager->createMcu(mcu))
                        break;
                    else if(flashAlgorithm)
                    {
                        logInfo(QString("Creating a link to the Flash Algorithm '%1'").arg(flashAlgorithm->name()));

                        if(!reqManager->createFlashAlgorithmLink(mcu, *flashAlgorithm))
                            break;
                    }
                }
            }
        }
    }

    //
    // Создание вспомогательной таблицы атрибутов компонента
    //
    logInfo("Creating a table of component attributes");

    if(!reqManager->createComponentPdscAttrTable(&errorString))
    {
        if(errorString.isEmpty())
            logError(QString("Error when creating the components attribute table"));
        else
            logError(QString("Error when creating the components attribute table: %1").
                     arg(errorString));

        return false;
    }

    //
    // Установка компонентов
    //
    QMap<QString, Component>& componentMap = pack.coComponentMap();

    // Сортировка компоннетов в алфавитном порядке
    QMultiMap<QString, QString> sortedComponentMap;

    for(const auto& component : componentMap)
    {
        sortedComponentMap.insert(component.getName(), component.getUuid());
    }

    logInfo("Adding components to the database");

    for(auto it = sortedComponentMap.begin(); it != sortedComponentMap.end(); ++it)
    {
        QString uuid = it.value();
        Component& component = componentMap[uuid];

        logInfo(QString("Creating component '%1/%2'").
                arg(component.getName()).
                arg(component.getDescription()));

        if(component.isPersisted())
        {
            logInfo(QString("The '%1/%2' component was created earlier").
                    arg(component.getName()).
                    arg(component.getDescription()));
        }
        else
        {
            if(!reqManager->createComponent(component, &errorString))
            {
                if(errorString.isEmpty())
                    logError(QString("An error occurred while adding the '%1' component").
                             arg(component.getName()));
                else
                    logError(QString("An error occurred while adding the '%1' component: %2").
                             arg(component.getName()).
                             arg(errorString));

                return false;
            }
        }
    }

    //
    // Создание компонентов в файловой структуре CoIDE
    //
    logInfo("Creating mirrors of components");

    if(!createComponentMirrors(pack, errorString))
    {
        if(errorString.isEmpty())
            logError("Couldn't create component mirror");
        else
            logError(QString("Couldn't create component mirror: %1").arg(errorString));

        return false;
    }

    //
    // Завершение установки
    //
    logInfo(QString("Package installation '%1' completed").arg(pack.name()));

    return true;
}

void PackManager::logError(const QString& error)
{
    m_lastError = error;
    Loggable::logError(m_lastError);
}

//------------------------------------------------------------------------------
// Поиск в архиве файла описания *.pdsc
//------------------------------------------------------------------------------
QString PackManager::findPackDescriptionFile(const PackDescription& pack,
                                             const QString& extension)
{
    QString pdsc;
    ZipArchive packet;

    QList<ZipArchive::ArchiveEntry> files = packet.listContents(pack.pathToArchive());

    foreach(ZipArchive::ArchiveEntry f, files)
    {
        if(f.extension.toLower() == extension)
        {
            pdsc = f.fullPath;
            break;
        }
    }

    return pdsc;
}

//------------------------------------------------------------------------------
// Извлечение файла описание во временный каталог
//------------------------------------------------------------------------------
bool PackManager::extractPackDescriptionFile(PackDescription &pack,
                                             QString &errorString)
{
    //
    // Данные не валидны
    //
    if(pack.pathToArchive().isEmpty())
    {
        errorString = QString("The archive path is not set");
        return false;
    }

    //
    // Находим pdsc-файл в архиве
    //
    QString pdscFilePath = findPackDescriptionFile(pack, pack.archiveInfo().isPdsc() ? "pdsc" : "json");

    if(pdscFilePath.isEmpty())
    {
        errorString = "Packet description file not found";
        return false;
    }

    QDir packDestDir;

    //
    // Путь к каталогу установки уже известен
    //
    if(!pack.installDir().isEmpty())
    {
        packDestDir.setPath(pack.installDir());

        //
        // Но реально каталог не существует. Пытаемся создать
        //
        if(!packDestDir.exists() && !packDestDir.mkpath(packDestDir.path()))
        {
            errorString = "The package directory cannot be created";
            return false;
        }
        else
        {
            pack.setPathToPdsc(packDestDir.path() + "/" + pdscFilePath);
        }
    }
    //
    // Подготовка каталога установки
    //
    else
    {
        packDestDir.setPath(Paths::instance()->coIdeTempDir());

        if(!packDestDir.exists() && !packDestDir.mkpath(packDestDir.path()))
        {
            errorString = "The package directory cannot be created";
            return false;
        }
        else
        {
            pack.setPathToPdsc(packDestDir.path() + "/" + pdscFilePath);
        }
    }

    return ZipArchive().extractFile(pack.pathToArchive(),
                                    packDestDir.path(),
                                    pdscFilePath);
}

//------------------------------------------------------------------------------
// Извлечение файлов описание периферии SVD
//------------------------------------------------------------------------------
bool PackManager::extractSVD(PackDescription &pack, QString &errorString)
{
    QDir dir;

    dir.setPath(pack.installDir());

    //
    // Пакет не валиден
    //
    if(pack.pathToArchive().isEmpty())
    {
        errorString = QString("The archive path is not set");
        return false;
    }
    else if(pack.installDir().isEmpty() || !dir.exists())
    {
        errorString = QString("The installation directory is not defined or does not exist");
        return false;
    }
    else if(!pack.isValid())
    {
        errorString = QString("The '%1' package is not valid").arg(pack.name());
        return false;
    }

    //
    // Формирование списка файлов для распаковки
    //
    Manufacturer& vendor = pack.vendors().first();

    foreach(QString familyName, vendor.families().keys())
    {
        Family& family = vendor.family(familyName);

        foreach(QString seriesName, family.seriesMap().keys())
        {
            Series& series = vendor.family(familyName).series(seriesName);

            foreach (QString mcuName, series.mcuMap().keys())
            {
                Mcu& mcu = vendor.family(familyName).series(seriesName).mcu(mcuName);
                QString svd = mcu.svdLocalPath();

                if(svd.isEmpty()) continue;

                if(!vendor.svdList().contains(Manufacturer::SvdInfo(svd)))
                {
                    Manufacturer::SvdInfo newSvd(svd);
                    newSvd.destDirectory = pack.installDir() + "/" + QFileInfo(svd).path();
                    newSvd.relativePath = pack.installDir() + "/" + svd;
                    newSvd.relativePath = newSvd.relativePath.remove(Paths::instance()->coIdeDir());
                    newSvd.destDirectory = newSvd.destDirectory.replace('/', '\\');
                    newSvd.relativePath = newSvd.relativePath.replace('/', '\\');
                    newSvd.addMcuName(mcu.getName());
                    vendor.svdList().append(newSvd);
                }
                else
                {
                    Manufacturer::SvdInfo * currSvd = vendor.svd(svd);

                    if(currSvd)
                    {
                        currSvd->addMcuName(mcu.getName());
                    }
                }
            }
        }
    }

    //
    // Распаковка
    //
    foreach (Manufacturer::SvdInfo s, vendor.svdList())
    {
        logInfo(QString("Extract file '%1'").arg(s.pathInArchive));

        if(!ZipArchive().extractFile(pack.pathToArchive(), s.destDirectory, s.pathInArchive))
        {
            errorString = QString("An error occurred while extracting the %1 file").arg(s.pathInArchive);
            return false;
        }
    }

    return true;
}

//------------------------------------------------------------------------------
// Извлечение алгоритмов программирования FLM
//------------------------------------------------------------------------------
bool PackManager::extractFLM(PackDescription& pack, QString& errorString)
{
    QDir dir;

    dir.setPath(pack.installDir());

    //
    // Пакет не валиден
    //
    if(pack.pathToArchive().isEmpty())
    {
        errorString = QString("The archive path is not set");
        return false;
    }
    else if(pack.installDir().isEmpty() || !dir.exists())
    {
        errorString = QString("The installation directory is not defined or does not exist");
        return false;
    }
    else if(!pack.isValid())
    {
        errorString = QString("The '%1' package is not valid").arg(pack.name());
        return false;
    }

    //
    // Формирование списка файлов для распаковки
    //
    Manufacturer& vendor = pack.vendors().first();

    foreach(QString familyName, vendor.families().keys())
    {
        Family& family = vendor.family(familyName);

        foreach(QString seriesName, family.seriesMap().keys())
        {
            Series& series = vendor.family(familyName).series(seriesName);

            foreach (QString mcuName, series.mcuMap().keys())
            {
                Mcu& mcu = vendor.family(familyName).series(seriesName).mcu(mcuName);

                for(auto& fa : mcu.algorithms())
                {
                    QString faLocalPath = fa.name();

                    if(faLocalPath.isEmpty()) continue;

                    if(!vendor.flmList().contains(Manufacturer::FlmInfo(faLocalPath)))
                    {
                        Manufacturer::FlmInfo newFlm(faLocalPath);
                        newFlm.destDirectory = pack.installDir() + "/" + QFileInfo(faLocalPath).path();
                        newFlm.relativePath = pack.installDir() + "/" + faLocalPath;
                        newFlm.relativePath = newFlm.relativePath.remove(Paths::instance()->coIdeDir());
                        newFlm.destDirectory = newFlm.destDirectory.replace('/', '\\');
                        newFlm.relativePath = newFlm.relativePath.replace('/', '\\');
                        newFlm.addMcuName(mcu.getName());
                        vendor.flmList().append(newFlm);
                    }
                    else
                    {
                        Manufacturer::FlmInfo * currFlm = vendor.flm(faLocalPath);

                        if(currFlm)
                        {
                            currFlm->addMcuName(mcu.getName());
                        }
                    }
                }
            }
        }
    }

    //
    // Распаковка
    //
    foreach (Manufacturer::FlmInfo flm, vendor.flmList())
    {
        logInfo(QString("Extract file '%1'").arg(flm.pathInArchive));

        if(!ZipArchive().extractFile(pack.pathToArchive(), flm.destDirectory, flm.pathInArchive))
        {
            errorString = QString("An error occurred while extracting the %1 file").arg(flm.pathInArchive);
            return false;
        }
    }

    return true;
}

//------------------------------------------------------------------------------
// Формирование TXT-файла базы данных SVD
//------------------------------------------------------------------------------
bool PackManager::makeSvdDatabase(PackDescription &pack, QString& errorString)
{
    //
    // Пакет не валиден
    //
    if(!pack.isValid())
    {
        errorString = QString("The '%1' package is not valid").arg(pack.name());
        return false;
    }

    QMap<QString, QStringList> svdSupportMcuMap;
    QMap<QString, QString> mcuHasSvdMap;
    QFile svdDatabase;
    Manufacturer vendor = pack.vendors().first();
    QList<Manufacturer::SvdInfo> svdList = vendor.svdList();
    QString dbFilename = Paths::instance()->coIdeDataDir() + "/" + vendor.getName() + ".txt";

    svdDatabase.setFileName(dbFilename);

    //
    // Если файл существует, но нельзя просто создать новый, т.к. может
    // быть потеряна информация об Mcu, которые были добавлены из более новых пакетов
    //
    if(svdDatabase.exists() && svdDatabase.open(QFile::ReadOnly))
    {
        QByteArray line;

        do
        {
            line = svdDatabase.readLine();

            if(!line.isEmpty())
            {
                QStringList items = QString(line).split(',');
                QString svdPath;

                if(!items.isEmpty())
                {
                    svdPath = items.last().trimmed();

                    // Проверка существования svd-файла
                    QFile currSvd(Paths::instance()->coIdeDataDir() + "\\" + svdPath);

                    if(currSvd.exists())
                    {
                        items.removeLast();

                        foreach (QString mcuName, items)
                        {
                            QStringList * mcuList = nullptr;

                            mcuName = mcuName.trimmed();
                            mcuHasSvdMap.insert(mcuName, svdPath);

                            if(svdSupportMcuMap.contains(svdPath))
                            {
                                mcuList = &svdSupportMcuMap[svdPath];
                            }
                            else
                            {
                                svdSupportMcuMap.insert(svdPath, QStringList());
                                mcuList = &svdSupportMcuMap[svdPath];
                            }

                            if(!mcuList->contains(mcuName))
                                mcuList->append(mcuName);
                        }
                    }
                }
            }
        }
        while(!line.isEmpty());
        svdDatabase.close();

        //
        // Обновляем информацию данными из PDSC
        //
        foreach(auto svdInfo, vendor.svdList())
        {
            QString svdPath = ".." + svdInfo.relativePath;

            foreach(QString mcuName, svdInfo.mcuList)
            {
                mcuName = mcuName.toUpper();

                bool mcuExists = mcuHasSvdMap.contains(mcuName);
                bool svdBelongMcu = mcuHasSvdMap.value(mcuName) == svdPath;
                bool svdExists = svdSupportMcuMap.contains(svdPath);

                // Создать новый Mcu и новую группу
                if(!mcuExists && !svdExists)
                {
                    QStringList mcuList = QStringList() << mcuName;
                    mcuHasSvdMap.insert(mcuName, svdPath);
                    svdSupportMcuMap.insert(svdPath, mcuList);
                }
                // Перенести Mcu в другую группу
                else if(mcuExists && svdExists && !svdBelongMcu)
                {
                    QString oldGroupName = mcuHasSvdMap.value(mcuName);
                    QStringList& oldGroupList = svdSupportMcuMap[oldGroupName];
                    QStringList& newGroupList = svdSupportMcuMap[svdPath];

                    oldGroupList.removeOne(mcuName);
                    if(!newGroupList.contains(mcuName))
                        newGroupList.append(mcuName);

                    // Заменить svd для данного mcu
                    mcuHasSvdMap[mcuName] = svdPath;

                    // Если группа опустела, то удаляем ее
                    if(oldGroupList.isEmpty())
                        svdSupportMcuMap.remove(oldGroupName);
                }
                // Создать новую группу и перенести туда Mcu
                else if(mcuExists && !svdExists)
                {
                    // Новая группа
                    svdSupportMcuMap.insert(svdPath, QStringList());

                    QString oldGroupName = mcuHasSvdMap.value(mcuName);
                    QStringList& oldGroupList = svdSupportMcuMap[oldGroupName];
                    QStringList& newGroupList = svdSupportMcuMap[svdPath];

                    oldGroupList.removeOne(mcuName);
                    if(!newGroupList.contains(mcuName))
                        newGroupList.append(mcuName);

                    // Заменить svd для данного mcu
                    mcuHasSvdMap[mcuName] = svdPath;

                    // Если группа опустела, то удаляем ее
                    if(oldGroupList.isEmpty())
                        svdSupportMcuMap.remove(oldGroupName);
                }
            }
        }

        //
        // Создаем новый файл с обновленными данными
        //
        if(!svdDatabase.open(QFile::WriteOnly))
        {
            errorString = QString("Failed to create %1 file: %2").arg(dbFilename).arg(svdDatabase.errorString());
            return false;
        }

        for(auto it = svdSupportMcuMap.begin(); it != svdSupportMcuMap.end(); ++it)
        {
            QString svdPath = it.key();
            QString line;

            foreach(QString mcuName, it.value())
            {
                line += mcuName.toUpper() + ", ";
            }
            line += svdPath + "\r\n";
            svdDatabase.write(line.toLatin1());
        }

        svdDatabase.close();
    }
    else if(!svdDatabase.exists())
    {
        if(!svdDatabase.open(QFile::WriteOnly))
        {
            errorString = QString("Failed to create %1 file: %2").arg(dbFilename).arg(svdDatabase.errorString());
            return false;
        }

        foreach(auto svd, vendor.svdList())
        {
            QString line;

            foreach(QString mcuName, svd.mcuList)
            {
                line += mcuName.toUpper() + ", ";
            }
            line += ".." + svd.relativePath + "\r\n";
            svdDatabase.write(line.toLatin1());
        }

        svdDatabase.close();
    }

    return true;
}

//------------------------------------------------------------------------------
// Формирование полного набора файлов для каждого уникального компонента
// Удаляет метаданные из пути к файлу. Читает список файлов из категории include
//------------------------------------------------------------------------------
void PackManager::loadCoComponents(PackDescription &pack)
{
    QMap<QString, QStringList>& coMap = pack.componentFilesMap();
    QMap<QString, Component>& componentMap = pack.coComponentMap();

    coMap.clear();

    if(pack.pathToArchive().isEmpty())
    {
        return;
    }

    foreach(Component c, pack.coComponentMap())
    {
        coMap.insert(c.getUuid(), QStringList());

        QStringList& list = coMap[c.getUuid()];
        QStringList includes = c.includes();
        QStringList sources = c.sources();
        QStringList libraries = c.libraries();
        QStringList scripts = c.linkerScripts();

        foreach(QString src, sources)
        {
            if(!list.contains(src))
                list.append(src.replace('/', '\\'));
        }

        foreach(QString lib, libraries)
        {
            if(!list.contains(lib))
                list.append(lib.replace('/', '\\'));
        }

        foreach(QString ld, scripts)
        {
            if(!list.contains(ld))
                list.append(ld.replace('/', '\\'));
        }

        // Чтение списка файлов в каталоге
        foreach(QString dir, includes)
        {
            QList<ZipArchive::ArchiveEntry> files = ZipArchive().listContents(pack.pathToArchive(), dir.replace('/', '\\'));

            foreach(ZipArchive::ArchiveEntry f, files)
            {
                if(!f.isDir && !list.contains(f.fullPath) && f.extension.toLower() == "h")
                {
                    list.append(f.fullPath.replace('/', '\\'));
                }
            }
        }
    }

    //
    // Дополняем компонент Compile заголовочными h-файлами, которые не были
    // явно указаны в pdsc
    //
    QString compileUuid;

    for(auto it = componentMap.begin(); it != componentMap.end(); ++it)
    {
       if(it.value().getName().startsWith("Compile_"))
       {
           compileUuid = it.key();
           break;
       }
    }

    //
    // Читаем список файлов в архиве и находим h-файлы, которые не были включены
    // ни в один из компонентов
    //
#if 0
    if(!compileUuid.isEmpty())
    {
        QList<ZipArchive::ArchiveEntry> files = ZipArchive().listContents(pack.pathToArchive());

        foreach(ZipArchive::ArchiveEntry f, files)
        {
            if(!f.isDir && f.extension.toLower() == "h")
            {
                bool componentContainsFile = false;

                for(auto it = coMap.begin(); it != coMap.end(); ++it)
                {
                    QStringList fList = it.value();

                    if(fList.contains(f.fullPath.replace('/', '\\')))
                    {
                        componentContainsFile = true;
                        break;
                    }
                }

                if(!componentContainsFile && f.name != "FlashOS.h")
                {
                    QStringList& list = coMap[compileUuid];
                    list.append(f.fullPath);
                }
            }
        }
    }
#else
    // TODO некоторые pdsc-файлы составлены некорректно
    // Пока такое решение для них
    // Оказалось, что копировать все h-файлы - не очень хорошее решение,
    // потому что появляется много ненужного мусора
    //
    if(!compileUuid.isEmpty() &&
       (pack.packVendor() == "NordicSemiconductor" ||
        pack.packVendor() == "Nordic Semiconductor"))
    {
        QString includeDir = "Device/Include/";

        QList<ZipArchive::ArchiveEntry> files = ZipArchive().listContents(pack.pathToArchive(), includeDir.replace('/', '\\'));

        foreach(ZipArchive::ArchiveEntry f, files)
        {
            if(!f.isDir && f.extension.toLower() == "h")
            {
                bool componentContainsFile = false;

                for(auto it = coMap.begin(); it != coMap.end(); ++it)
                {
                    QStringList fList = it.value();

                    if(fList.contains(f.fullPath.replace('/', '\\')))
                    {
                        componentContainsFile = true;
                        break;
                    }
                }

                if(!componentContainsFile)
                {
                    QStringList& list = coMap[compileUuid];
                    list.append(f.fullPath);
                }
            }
        }
    }
#endif

    return;
}

//------------------------------------------------------------------------------
// Возвращает полный список файлов, входящих во все компоненты
// Эта функция необходимо для распаковки архива, чтобы не повторять команды
// распаковки одного и того же файла по нескольку раз
//------------------------------------------------------------------------------
QStringList PackManager::getFullFileList(PackDescription &pack)
{
    QMap<QString, QStringList>& coMap = pack.componentFilesMap();
    QStringList list;

    if(coMap.isEmpty())
    {
        loadCoComponents(pack);
    }

    for(auto it = coMap.begin(); it != coMap.end(); ++it)
    {
        Component& component = pack.coComponentMap()[it.key()];
        QStringList coList = it.value();

        if(component.getName().startsWith("CMSIS_Core_"))
            continue;

        foreach(QString f, coList)
        {
            if(!list.contains(f))
            {
                list.append(f);
            }
        }
    }

    return list;
}

//------------------------------------------------------------------------------
// Возвращает список файлов, входящих в ядро CMSIS
//------------------------------------------------------------------------------
QStringList PackManager::getCmsisFileList(PackDescription &pack, const QString version)
{
    QMap<QString, QStringList>& coMap = pack.componentFilesMap();
    QStringList list;

    if(coMap.isEmpty())
    {
        loadCoComponents(pack);
    }

    for(auto it = coMap.begin(); it != coMap.end(); ++it)
    {
        Component& component = pack.coComponentMap()[it.key()];
        QStringList coList = it.value();

        if(component.getName() == QString("CMSIS_Core_%1").arg(version))
        {
            foreach(QString f, coList)
            {
                if(!list.contains(f))
                {
                    list.append(f);
                }
            }
        }
    }

    return list;
}

//------------------------------------------------------------------------------
// Распаковка исходников
//------------------------------------------------------------------------------
bool PackManager::extractSources(PackDescription &pack, QString &errorString)
{
    QDir dir;

    dir.setPath(pack.installDir());

    //
    // Пакет не валиден
    //
    if(pack.pathToArchive().isEmpty())
    {
        errorString = QString("The archive path is not set");
        return false;
    }
    else if(pack.installDir().isEmpty() || !dir.exists())
    {
        errorString = QString("The installation directory is not defined or does not exist");
        return false;
    }
    else if(!pack.isValid())
    {
        errorString = QString("The '%1' package is not valid").arg(pack.name());
        return false;
    }

    //
    // Формирование списка файлов CMSIS для распаковки
    //
    QMap<QString, QString> cmsisCores = Paths::instance()->cmsisCores();

    for(auto it = cmsisCores.begin(); it != cmsisCores.end(); ++it)
    {
        QStringList cmsisFiles = getCmsisFileList(pack, it.key());

        //
        // Распаковка
        //
        foreach (QString s, cmsisFiles)
        {
            QString destinationDir = Paths::instance()->coIdeCmsisDir(it.key()).replace('/', '\\') + "\\" +
                                     QFileInfo(s).path().replace('/','\\');

            logInfo(QString("Extract file '%1'").arg(s));

            if(!ZipArchive().extractFile(Paths::instance()->cmsisCore(it.key()), destinationDir, s))
            {
                errorString = QString("An error occurred while extracting the %1 file").arg(s);
                return false;
            }
        }
    }

    //
    // Формирование списка файлов для распаковки
    //
    QStringList sources = getFullFileList(pack);

    if(sources.isEmpty())
    {
        return false;
    }

    //
    // Распаковка
    //
    foreach (QString s, sources)
    {
        QString destinationDir = pack.installDir() + "\\" + QFileInfo(s).path().replace('/','\\');

        logInfo(QString("Extract file '%1'").arg(s));

        if(!ZipArchive().extractFile(pack.pathToArchive(), destinationDir, s))
        {
            errorString = QString("An error occurred while extracting the %1 file").arg(s);
            return false;
        }
    }

    return true;
}

//------------------------------------------------------------------------------
// Создает файловую структуру компонента в системе CoIDE.
// Фактически создает ссылки на файлы, которые распакованы в каталог установки пакета
//------------------------------------------------------------------------------
bool PackManager::createComponentMirrors(PackDescription &pack, QString &errorString)
{
    if(!pack.isValid())
    {
        errorString = QString("The '%1' package is not valid").arg(pack.name());
        return false;
    }
    else if(pack.installDir().isEmpty())
    {
        errorString = QString("The installation directory is not defined or does not exist");
        return false;
    }

    if(pack.componentFilesMap().isEmpty())
        loadCoComponents(pack);

    QMap<QString, QStringList>& fileMap = pack.componentFilesMap();
    QMap<QString, Component>& componentMap = pack.coComponentMap();

    for(auto it = fileMap.begin(); it != fileMap.end(); ++it)
    {
        QStringList coList = it.value();
        Component& component = componentMap[it.key()];
        QDir rootDir;

        if(component.getType() == Component::DRIVER)
            rootDir.setPath(Paths::instance()->coIdeDriversDir() +
                            "/" + QString::number(component.getId()) +
                            "_" + component.getName() +
                            "/src");
        else
            rootDir.setPath(Paths::instance()->coIdeComponentsDir() +
                            "/" + QString::number(component.getId()) +
                            "_" + component.getName() +
                            "/src");

        if(!rootDir.exists() && !rootDir.mkpath(rootDir.path()))
        {
            errorString = QString("The mirror directory %1 cannot be created").arg(rootDir.path());
            return false;
        }

        foreach(QString f, coList)
        {
            QFileInfo info(f);
            QDir subDir;
            QString targetPath = pack.installDir() + "/" + f;
            QString linkPath = rootDir.path() + "/" + f;

            // CoIDE не импортирует ld-файлы
            if(info.suffix().toLower() == "ld")
            {
                linkPath = rootDir.path() + "/" +
                           info.path() + "/" +
                           info.completeBaseName() + "." + "ls";
            }

            if(component.getName().startsWith("CMSIS_Core_"))
            {
                targetPath = Paths::instance()->coIdeCmsisDir(component.getVersion()) + "/" + f;
            }

            subDir.setPath(rootDir.path() + "/" + info.path());

            if(!subDir.exists() && !subDir.mkpath(subDir.path()))
            {
                errorString = QString("The %1 directory cannot be created").arg(rootDir.path());
                return false;
            }

            QString relativePath = linkPath;
            int beginPos = (component.getType() == Component::DRIVER) ?
                           Paths::instance()->coIdeDriversDir().length() :
                           Paths::instance()->coIdeComponentsDir().length();

            relativePath.remove(0, beginPos + 1);

            logInfo(QString("Create symbolic link '%1'").arg(relativePath));

            if(!QFile(linkPath).exists() && !MakeLink::createLink(linkPath, targetPath))
            {
                errorString = QString("Failed to create symbolic link to %1 file").arg(f);
                return false;
            }
        }
    }

    return true;
}

//------------------------------------------------------------------------------
// Обновление путей к алгоритмам программировани после загрузки pdsc
//------------------------------------------------------------------------------
void PackManager::updatePaths(PackDescription& pack)
{
    //
    // Формирование списка файлов для распаковки
    //
    Manufacturer& vendor = pack.vendors().first();

    foreach(QString familyName, vendor.families().keys())
    {
        Family& family = vendor.family(familyName);

        foreach(QString seriesName, family.seriesMap().keys())
        {
            Series& series = vendor.family(familyName).series(seriesName);

            foreach (QString mcuName, series.mcuMap().keys())
            {
                Mcu& mcu = vendor.family(familyName).series(seriesName).mcu(mcuName);

                for(ProgAlgorithm& a : mcu.algorithms())
                {
                    QString path = Paths::instance()->coIdePackDir(vendor.toKeilName(), pack.release());
                    path = QString(path + "/" + a.name()).replace(Paths::instance()->coIdeDir(), "..");
                    a.setInstallPath(path);
                }
            }
        }
    }
}

