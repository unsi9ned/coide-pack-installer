#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QString>
#include "packmanager.h"
#include "paths.h"
#include "ziparchive.h"
#include "pdscparser.h"
#include "manufacturer.h"
#include "requestmanager.h"
#include "makelink.h"

PackManager::PackManager(QObject *parent) : QObject(parent)
{
    connect(this, SIGNAL(errorOccured(QString)), SLOT(debugPrintMessage(QString)));
    connect(this, SIGNAL(eventOccured(QString)), SLOT(debugPrintMessage(QString)));

    connect(RequestManager::instance(), SIGNAL(errorOccured(QString)), SLOT(debugPrintMessage(QString)));
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
    if(!extractPDSC(pack, errorString))
    {
        if(errorString.isEmpty())
            emit errorOccured("Couldn't extract pdsc file");
        else
            emit errorOccured(QString("Couldn't extract pdsc file: %1").arg(errorString));

        return;
    }

    //
    // Разбор файла описания
    //
    PdscParser parser;
    parser.parse(pack);
}

//------------------------------------------------------------------------------
// Установка пакета
//------------------------------------------------------------------------------
void PackManager::packInstall(PackDescription &pack)
{
    //
    // Проверка валидности устанавливаемого пакета
    //
    if(!pack.isValid())
    {
        emit errorOccured(QString("The '%1' package is not valid").arg(pack.name()));
        return;
    }

    emit eventOccured(QString("The '%1' package is being installed").arg(pack.name()));

    //
    // Подготовка каталога пакета
    //
    QString errorString;
    QDir packInstallDir;
    Manufacturer vendor = pack.vendors().first();
    QString path = Paths::instance()->coIdePackDir(vendor.getName(), pack.release());

    packInstallDir.setPath(path);
    pack.setInstallDir(path);

    //
    // Создание каталога установки
    //
    emit eventOccured(QString("Creating an installation directory"));

    if(!packInstallDir.exists() && !packInstallDir.mkpath(packInstallDir.path()))
    {
        emit errorOccured("The package directory cannot be created");
        return;
    }

    //
    // Распаковка файла описание аппаратуры в каталог установки
    //
    emit eventOccured(QString("Extracting a pdsc file"));

    if(!extractPDSC(pack, errorString))
    {
        if(errorString.isEmpty())
            emit errorOccured("Couldn't extract pdsc file");
        else
            emit errorOccured(QString("Couldn't extract pdsc file: %1").arg(errorString));

        return;
    }

    //
    // Распаковка SVD-файлов и добавление в TXT-базу
    //
    if(!extractSVD(pack, errorString))
    {
        if(errorString.isEmpty())
            emit errorOccured("Couldn't extract SVD files");
        else
            emit errorOccured(QString("Couldn't extract SVD files: %1").arg(errorString));

        return;
    }

    //
    // Формирование базы данных SVD
    //
    if(!makeSvdDatabase(pack, errorString))
    {
        if(errorString.isEmpty())
            emit errorOccured("An error occurred when creating the SVD database");
        else
            emit errorOccured(QString("An error occurred when creating the SVD database: %1").arg(errorString));

        return;
    }

    //
    // Распаковка исходников
    //
    if(!extractSources(pack, errorString))
    {
        if(errorString.isEmpty())
            emit errorOccured("Couldn't extract sources files");
        else
            emit errorOccured(QString("Couldn't extract sources files: %1").arg(errorString));

        return;
    }

    //
    // Создание компонентов в файловой структуре CoIDE
    //
    if(!createComponentMirrors(pack, errorString))
    {
        if(errorString.isEmpty())
            emit errorOccured("Couldn't create component mirror");
        else
            emit errorOccured(QString("Couldn't create component mirror: %1").arg(errorString));

        return;
    }

    //
    // Загружаем из базы данных информацию об устройствах
    //
    RequestManager * reqManager = RequestManager::instance();

    if(!reqManager->fixVendorIDs(errorString))
    {
        if(errorString.isEmpty())
            emit errorOccured("An error occurred during the fix of the manufacturer's ID");
        else
            emit errorOccured(QString("An error occurred during the fix of the manufacturer's ID: %1").arg(errorString));

        return;
    }

    //
    // Создаем новых производителей (или обновляем существующие)
    //
    for(auto it = pack.vendors().begin(); it != pack.vendors().end(); ++it)
    {
        Manufacturer& m = it.value();

        emit eventOccured(QString("Adding the manufacturer %1").arg(m.getName()));

        if(!reqManager->createManufacturer(m))
            break;

        for(auto f = m.families().begin(); f != m.families().end(); ++f)
        {
            Family& fam = f.value();

            emit eventOccured(QString("Adding the family %1").arg(fam.getName()));

            if(!reqManager->createFamily(fam))
                break;

            for(auto s = fam.seriesMap().begin(); s!= fam.seriesMap().end(); ++s)
            {
                Series& series = s.value();

                emit eventOccured(QString("Adding the series %1").arg(series.getName()));

                if(!reqManager->createSeries(series))
                    break;

                for(auto dev = series.mcuMap().begin(); dev != series.mcuMap().end(); ++dev)
                {
                    Mcu& mcu = dev.value();
                    DebugAlgorithm& algorithm = mcu.getDebugAlgorithm();

                    emit eventOccured(QString("Adding the Debug algorithm '%1'").arg(mcu.getDebugAlgorithm().name()));

                    if(!reqManager->createDebugAlgorithm(algorithm))
                        break;
                    else
                        mcu.setDebugAlgorithmId(algorithm.coId());

                    emit eventOccured(QString("Adding the MCU = %1").arg(mcu.getName()));

                    if(!reqManager->createMcu(mcu))
                        break;
                }
            }
        }
    }

    //
    // Установка компонентов
    //
    QMap<QString, Component>& componentMap = pack.components();
    QMap<QString, QStringList>& componentFileMap = pack.coComponentMap();

    foreach(QString uuid, componentMap.keys())
    {
        Component& component = componentMap[uuid];
        QStringList& files = componentFileMap[uuid];

        emit eventOccured(QString("Creating component '%1/%2'").
                          arg(component.getName()).
                          arg(component.getVersion()));

        if(!reqManager->createComponent(component, &errorString))
        {
            if(errorString.isEmpty())
                emit errorOccured(QString("An error occurred while adding the '%1' component").
                                  arg(component.getName()));
            else
                emit errorOccured(QString("An error occurred while adding the '%1' component: %2").
                                  arg(component.getName()).
                                  arg(errorString));

            return;
        }
    }

#if 0
    {
#if 0
        QMap<QString, Manufacturer> vendors;
        reqManager->loadDataFromDb(vendors);

        qInfo() << "Manufacturers:" << vendors.count();
#endif
//        for (int i = 0; i < reqManager->getManufacturerCount(); i++)
//        {
//            Manufacturer m = reqManager->getManufacturer(i);
//            qInfo() << QString("%2:%1").arg(m.getName()).arg(m.getId());
//        }
    }
#endif

    //
    // Завершение установки
    //
    emit eventOccured(QString("Package installation '%1' completed").arg(pack.name()));
}

//------------------------------------------------------------------------------
// Поиск в архиве файла описания *.pdsc
//------------------------------------------------------------------------------
QString PackManager::findPDSC(const PackDescription& pack)
{
    QString pdsc;
    ZipArchive packet;

    QList<ZipArchive::ArchiveEntry> files = packet.listContents(pack.pathToArchive());

    foreach(ZipArchive::ArchiveEntry f, files)
    {
        if(f.extension.toLower() == "pdsc")
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
bool PackManager::extractPDSC(PackDescription &pack, QString &errorString)
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
    QString pdscFilePath = findPDSC(pack);

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
        if(!ZipArchive().extractFile(pack.pathToArchive(), s.destDirectory, s.pathInArchive))
        {
            errorString = QString("An error occurred while extracting the %1 file").arg(s.pathInArchive);
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

    QFile svdDatabase;
    Manufacturer vendor = pack.vendors().first();
    QString dbFilename = Paths::instance()->coIdeDataDir() + "/" + vendor.getName() + ".txt";

    svdDatabase.setFileName(dbFilename);

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
            line += mcuName + ", ";
        }
        line += ".." + svd.relativePath + "\r\n";
        svdDatabase.write(line.toLatin1());
    }

    svdDatabase.close();

    return true;
}

//------------------------------------------------------------------------------
// Формирование полного набора файлов для каждого уникального компонента
// Удаляет метаданные из пути к файлу. Читает список файлов из категории include
//------------------------------------------------------------------------------
void PackManager::loadCoComponents(PackDescription &pack)
{
    QMap<QString, QStringList>& coMap = pack.coComponentMap();
    coMap.clear();

    if(pack.pathToArchive().isEmpty())
    {
        return;
    }

    foreach(Component c, pack.components())
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
                if(!f.isDir && !list.contains(f.fullPath))
                {
                    list.append(f.fullPath.replace('/', '\\'));
                }
            }
        }
    }
}

//------------------------------------------------------------------------------
// Возвращает полный список файлов, входящих во все компоненты
// Эта функция необходимо для распаковки архива, чтобы не повторять команды
// распаковки одного и того же файла по нескольку раз
//------------------------------------------------------------------------------
QStringList PackManager::getFullFileList(PackDescription &pack)
{
    QMap<QString, QStringList>& coMap = pack.coComponentMap();
    QStringList list;

    if(coMap.isEmpty())
    {
        loadCoComponents(pack);
    }

    for(auto it = coMap.begin(); it != coMap.end(); ++it)
    {
        QStringList coList = it.value();

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

    if(pack.coComponentMap().isEmpty())
        loadCoComponents(pack);

    QMap<QString, QStringList>& fileMap = pack.coComponentMap();
    QMap<QString, Component>& componentMap = pack.components();

    for(auto it = fileMap.begin(); it != fileMap.end(); ++it)
    {
        QStringList coList = it.value();
        Component& component = componentMap[it.key()];
        QDir rootDir;

        if(component.getType() == Component::DRIVER)
            rootDir.setPath(Paths::instance()->coIdeDriversDir() + "/!" + component.getUuid() + "/src");
        else
            rootDir.setPath(Paths::instance()->coIdeComponentsDir() + "/!" + component.getUuid() + "/src");

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

            subDir.setPath(rootDir.path() + "/" + info.path());

            if(!subDir.exists() && !subDir.mkpath(subDir.path()))
            {
                errorString = QString("The %1 directory cannot be created").arg(rootDir.path());
                return false;
            }

            if(!MakeLink::createLink(linkPath, targetPath))
            {
                errorString = QString("Failed to create symbolic link to %1 file").arg(f);
                return false;
            }
        }
    }

    return true;
}

//------------------------------------------------------------------------------
// Временная функция для печати ошибок и сообщений
//------------------------------------------------------------------------------
void PackManager::debugPrintMessage(QString e)
{
    qInfo() << e;
}
