#include <QDebug>
#include <QDir>
#include "packmanager.h"
#include "paths.h"
#include "ziparchive.h"
#include "pdscparser.h"

PackManager::PackManager(QObject *parent) : QObject(parent)
{

}

//------------------------------------------------------------------------------
// Поиск в пакете файла описания и его разбор
//------------------------------------------------------------------------------
void PackManager::readPackDescription(const QString &pathToPack, PackDescription &pack)
{
    QString pdsc = findPDSC(pathToPack);

    //
    // Ищем внутри архива файл описания
    //
    if(pdsc.isEmpty())
    {
        emit errorOccured("Packet description file not found");
        return;
    }

    //
    // Проверяем наличие временного каталога и создаем, если его нет
    //
    QDir coTempDir;
    coTempDir.setPath(Paths::instance()->coIdeTempDir());

    if(!coTempDir.exists() && !coTempDir.mkpath(coTempDir.path()))
    {
        emit errorOccured("Temp folder not exists");
        return;
    }

    //
    // Распаковываем файл описания во временный каталог
    //
    if(!extractPDSC(pathToPack, pdsc))
    {
        emit errorOccured("Couldn't extract pdsc file");
        return;
    }

    //
    // Разбор файла описания
    //
    PdscParser parser;
    parser.parse(Paths::instance()->coIdeTempDir() + "/" + pdsc, pack);
}

void PackManager::packInstall(const PackDescription &pack)
{

}

//------------------------------------------------------------------------------
// Поиск в архиве файла описания *.pdsc
//------------------------------------------------------------------------------
QString PackManager::findPDSC(const QString &pathToPack)
{
    QString pdsc;
    ZipArchive packet;

    QList<ZipArchive::ArchiveEntry> files = packet.listContents(pathToPack);

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
bool PackManager::extractPDSC(const QString &pathToPack, const QString& pathToPdsc)
{
    ZipArchive packet;
    return packet.extractFile(pathToPack, Paths::instance()->coIdeTempDir(), pathToPdsc);
}

void PackManager::svdFileRegister(const QString &vendorName,
                                  const QString &svdFileName)
{

}
