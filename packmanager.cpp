#include <QDebug>
#include <QDir>
#include "packmanager.h"
#include "paths.h"
#include "ziparchive.h"
#include "pdscparser.h"

PackManager::PackManager(QObject *parent) : QObject(parent)
{
    connect(this, SIGNAL(errorOccured(QString)), SLOT(debugPrintMessage(QString)));
    connect(this, SIGNAL(eventOccured(QString)), SLOT(debugPrintMessage(QString)));
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
        errorString = "Couldn't extract pdsc file" + errorString.isEmpty() ? "" : QString(": %1").arg(errorString);

        emit errorOccured(errorString);
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

    emit eventOccured(QString("Creating an installation directory"));

    if(!packInstallDir.exists() && !packInstallDir.mkpath(packInstallDir.path()))
    {
        emit errorOccured("The package directory cannot be created");
        return;
    }

    emit eventOccured(QString("Extracting a pdsc file"));

    if(!extractPDSC(pack, errorString))
    {
        errorString = "Couldn't extract pdsc file" + errorString.isEmpty() ? "" : QString(": %1").arg(errorString);

        emit errorOccured(errorString);
        return;
    }

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

    // Находим pdsc-файл в архиве
    QString pdscFilePath = findPDSC(pack);

    if(pdscFilePath.isEmpty())
    {
        errorString = "Packet description file not found";
        return false;
    }

    //
    // Подготовка каталога установки
    //
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
    // Формируем путь к каталогу и создаем его
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
// Формирование TXT-файла базы данных SVD
//------------------------------------------------------------------------------
void PackManager::svdFileRegister(const QString &vendorName,
                                  const QString &svdFileName)
{

}

//------------------------------------------------------------------------------
// Временная функция для печати ошибок и сообщений
//------------------------------------------------------------------------------
void PackManager::debugPrintMessage(QString e)
{
    qInfo() << e;
}
