#include <QDebug>
#include <QFile>
#include "ziparchive.h"
#include "services/paths.h"

ZipArchive::ZipArchive(QObject *parent) : QObject(parent)
{

}

//------------------------------------------------------------------------------
// Вывод списка файлов внутри архива
//------------------------------------------------------------------------------
QList<ZipArchive::ArchiveEntry> ZipArchive::listContents(const QString &pathToArchive, QString inheritPath)
{
    QList<ZipArchive::ArchiveEntry> fileList;
    QStringList args = QStringList() << "l" << "-slt" << pathToArchive;
    QStringList lines;

    args = inheritPath.isEmpty() ? args : QStringList() << args << inheritPath;
    lines = execute(args);

    if(!lines.isEmpty())
    {
        QStringList technicalInfo = {
            "Path",
            "Folder",
            "Size",
            "Packed Size",
            "Modified",
            "Created",
            "Accessed",
            "Attributes",
            "Encrypted",
            "Comment",
            "CRC",
            "Method",
            "Host OS",
            "Version",
            "\r"
        };

        // На всякий случай заполняем список пустыми строками, чтобы не было
        // ошибки доступа
        for(int i = 0; lines.size() < technicalInfo.size(); i++)
        {
            lines.append(QString());
        }

        for(auto i = 0; i < lines.count();)
        {
            bool validInfo = true;
            QStringList currFileInfo = lines.mid(i, technicalInfo.size());
            QMap<QString, QStringList> fileInfoMap;

            for(int j = 0; j < technicalInfo.size(); j++)
            {
                if(currFileInfo.at(j).startsWith(technicalInfo.at(j), Qt::CaseInsensitive) == false)
                {
                    validInfo = false;
                    break;
                }
                else
                    fileInfoMap.insert(technicalInfo.at(j),
                                       currFileInfo.at(j).split('=', QString::SkipEmptyParts) << "");
            }

            if(validInfo)
            {
                ZipArchive::ArchiveEntry entry = from7zOutput(fileInfoMap);
                fileList.append(entry);
                i += technicalInfo.size();
            }
            else
            {
                i++;
            }
        }
    }

    return fileList;
}

//------------------------------------------------------------------------------
// Извлечь файл из архива в указанную директорию
// Формат команды: 7z e -y <archive> -o"<outdir>" "<filepath>"
//------------------------------------------------------------------------------
bool ZipArchive::extractFile(const QString &pathToArchive,
                             const QString &destDir,
                             const QString &pathToFile)
{
    enum ExtractingStage
    {
        START_COMMAND = 0,
        PROCESSING_ARCHIVE,
        EXTRACTING_FILE,
        GET_STATUS,
    }
    stage = START_COMMAND;

    QStringList args;
    args << "e" << QString("%1").arg(pathToArchive);
    args << "-y";
    args << QString("-o%1").arg(destDir);
    args << QString("%1").arg(pathToFile);

    QStringList lines = execute(args);

#if 0
    qInfo() << args.join(' ');
    qInfo() << lines.join("\r\n");
#endif

    foreach (QString line, lines)
    {
        if(line.startsWith("Processing archive:"))
        {
            QString archPath = (line.split(": ") << "").at(1).trimmed();
            QString validPath = pathToArchive;

            if(archPath.replace('\\', '/') == validPath.replace('\\', '/'))
            {
                stage = PROCESSING_ARCHIVE;
            }
            else
            {
                return false;
            }
        }
        else if(line.startsWith("Extracting"))
        {
            QString filePath = (line.split(' ', QString::SkipEmptyParts) << "").at(1).trimmed();
            QString validPath = pathToFile;

            if(filePath.replace('\\', '/') != validPath.replace('\\', '/') || stage != PROCESSING_ARCHIVE)
            {
                return false;
            }
            else
            {
                stage = EXTRACTING_FILE;
            }
        }
        else if(line.startsWith("Everything is"))
        {
            if(stage == EXTRACTING_FILE && line.toLower().trimmed() == "everything is ok")
            {
                return true;
            }
            else
            {
                return false;
            }
        }
    }

    return false;
}

//------------------------------------------------------------------------------
// Разбор вывода команды "7z l -slt <archive>"
//------------------------------------------------------------------------------
ZipArchive::ArchiveEntry ZipArchive::from7zOutput(const QMap<QString, QStringList> &fileInfo)
{
    ZipArchive::ArchiveEntry entry;

    entry.fullPath = fileInfo["Path"].at(1).trimmed();
    entry.size = fileInfo["Size"].at(1).trimmed().toInt();
    entry.compressedSize = fileInfo["Packed Size"].at(1).trimmed().toInt();
    entry.crc32 = fileInfo["CRC"].at(1).trimmed().toInt();
    entry.modified = parseDateTime(fileInfo["Modified"].at(1).trimmed());
    entry.isDir = fileInfo["Folder"].at(1).trimmed() == "+";

    if(entry.fullPath.contains("\\"))
    {
        entry.name = entry.fullPath.mid(entry.fullPath.lastIndexOf("\\") + 1);
    }
    else if(entry.fullPath.contains("/"))
    {
        entry.name = entry.fullPath.mid(entry.fullPath.lastIndexOf("/") + 1);
    }
    else
    {
        entry.name = entry.fullPath;
    }

    if(entry.name.contains('.'))
    {
        entry.extension = entry.name.mid(entry.name.lastIndexOf('.') + 1);
    }

    return entry;
}

//------------------------------------------------------------------------------
// Разбор параметра даты и времени
//------------------------------------------------------------------------------
QDateTime ZipArchive::parseDateTime(const QString &dateTimeStr)
{
    QDateTime dt;

    // Формат 1: ISO (7z -slt) - 2017-10-03 11:25:52
    dt = QDateTime::fromString(dateTimeStr, "yyyy-MM-dd hh:mm:ss");
    if (dt.isValid()) return dt;

    // Формат 2: ISO с T - 2017-10-03T11:25:52
    dt = QDateTime::fromString(dateTimeStr, Qt::ISODate);
    if (dt.isValid()) return dt;

    // Формат 3: RU - 03.10.2017 11:25:52
    dt = QDateTime::fromString(dateTimeStr, "dd.MM.yyyy hh:mm:ss");
    if (dt.isValid()) return dt;

    // Формат 4: US - 10/03/2017 11:25:52
    dt = QDateTime::fromString(dateTimeStr, "MM/dd/yyyy hh:mm:ss");
    if (dt.isValid()) return dt;

    // Формат 5: US с AM/PM - 10/03/2017 11:25:52 PM
    dt = QDateTime::fromString(dateTimeStr, "MM/dd/yyyy hh:mm:ss AP");
    if (dt.isValid()) return dt;

    // Формат 6: Короткий год - 03.10.17 11:25
    dt = QDateTime::fromString(dateTimeStr, "dd.MM.yy hh:mm");
    if (dt.isValid()) return dt;

    // Формат 7: Только дата - 2017-10-03
    dt = QDateTime::fromString(dateTimeStr, "yyyy-MM-dd");
    if (dt.isValid()) return dt;

    // Invalid
    return QDateTime::currentDateTime();
}

//------------------------------------------------------------------------------
// Выполнить команду 7z с заданным набором аргументов
//------------------------------------------------------------------------------
QStringList ZipArchive::execute(QStringList args)
{
    QProcess * sevenZipProc = new QProcess(this);

    sevenZipProc->start(Paths::instance()->sevenZipExe(), args, QProcess::ReadWrite);

    if(!sevenZipProc->waitForStarted(5000))
    {
        sevenZipProc->close();
    }
    else if(!sevenZipProc->waitForFinished(5000))
    {
        sevenZipProc->close();
    }
    else
    {
        QByteArray stdOut = sevenZipProc->readAllStandardOutput();
        QStringList lines = QString::fromUtf8(stdOut).split('\n', QString::SkipEmptyParts);
        return lines;
    }

    delete sevenZipProc;
    return QStringList();
}


