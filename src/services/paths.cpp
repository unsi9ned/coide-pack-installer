#include <QApplication>
#include <QDir>
#include <QFile>
#include <QString>
#include <QDateTime>
#include <QDirIterator>
#include <QDebug>
#ifdef Q_OS_WIN
#include <windows.h>
#endif
#include "paths.h"
#include "settings.h"

Paths* Paths::m_instance = nullptr;

const QMap<QString, QString> Paths::_coDebugAlgorithmsMap = {
    {"Cortex-M0",  "cortex-m0.xml"},
    {"Cortex-M0+", "cortex-m0plus.xml"},
    {"Cortex-M3",  "cortex-m3.xml"},
    {"Cortex-M4",  "cortex-m4.xml"},

    //TODO временное решение для полной установки пакетов с новыми ядрами,
    //которых нет в CoIDE
    {"Cortex-M33",  "cortex-m3.xml"},

    //TODO для Cortex-M1 добавляем базовый алгоритм, который находится
    //в каталоге программы
    {"Cortex-M1",  "cortex-m1.xml"},
};

const QMap<QString, QString> Paths::_cmsisCoreMap = {
    {"4.3.0", "CMSIS-CORE for Cortex-M, SC000, and SC300"},
    {"5.6.0", "CMSIS-CORE for Cortex-M, SC000, SC300, Star-MC1, ARMv8-M, ARMv8.1-M"},
    {"6.3.0", "CMSIS-CORE for Cortex-M, SC000, SC300, Star-MC, ARMv8-M, ARMv8.1-M"},
};

//------------------------------------------------------------------------------
// Загрузка пути к каталогу CoIDE
//------------------------------------------------------------------------------
Paths::Paths(QObject *parent) : QObject(parent)
{

}

//------------------------------------------------------------------------------
// Создание единственного экземпляра класса
//------------------------------------------------------------------------------
Paths *Paths::instance()
{
    if(m_instance == nullptr)
       m_instance = new Paths();

    return m_instance;
}

//------------------------------------------------------------------------------
// Освобождение ресурсов
//------------------------------------------------------------------------------
Paths::~Paths()
{
    if(m_instance)
        delete m_instance;
}

//------------------------------------------------------------------------------
// Путь к каталогу программы
//------------------------------------------------------------------------------
QString Paths::appDirPath()
{
#ifdef Q_OS_WIN
    char buffer[MAX_PATH];
    GetModuleFileNameA(NULL, buffer, MAX_PATH);
    QString path(buffer);
    return path.mid(0, path.lastIndexOf('\\'));
#else
    return QApplication::applicationDirPath();
#endif
}

//------------------------------------------------------------------------------
// Путь к файлу настроек
//------------------------------------------------------------------------------
QString Paths::appSettingsFile()
{
    return appDirPath() + "/" + Paths::APP_SETTINGS;
}

//------------------------------------------------------------------------------
// Путь к логу программы
//------------------------------------------------------------------------------
QString Paths::appLogFile()
{
    return appDirPath() + "/" + Paths::APP_LOG;
}

//------------------------------------------------------------------------------
// Путь к папке с алгоритмами отладки, которые идут в комплекте с программой
//------------------------------------------------------------------------------
QString Paths::appDebugAlgorithmDir()
{
    return appDirPath() + "/" + Paths::APP_DEBUG_ALGORITHMS_DIR;
}

//------------------------------------------------------------------------------
// Путь к программе-архиватору 7z
//------------------------------------------------------------------------------
QString Paths::sevenZipExe()
{
    return appDirPath() + "/" + Paths::APP_UTILS_DIR + "/" + Paths::SEVEN_ZIP;
}

//------------------------------------------------------------------------------
// Путь к архиву с исходниками CMSIS CORE
//------------------------------------------------------------------------------
QString Paths::cmsisCore(QString version)
{
    QString path =  appDirPath() + "/" +
                    Paths::APP_CMSIS_DIR + "/" +
                    QString(Paths::CMSIS_CORE_ZIP).replace("%VERSION%", version);

    return path;
}

//------------------------------------------------------------------------------
// Путь к CoIDE
//------------------------------------------------------------------------------
QString Paths::coIdeDir()
{
    return Settings::instance()->coIdePath();
}

//------------------------------------------------------------------------------
// Путь к базе данных CoIDE
//------------------------------------------------------------------------------
QString Paths::coIdeDatabaseFile()
{
    return coIdeDir() + "/" + Paths::CO_DATABASE;
}

//------------------------------------------------------------------------------
// Путь к каталогу с алгоритмами отладки CoIDE
//------------------------------------------------------------------------------
QString Paths::coIdeDebugAlgorithmDir()
{
    return coIdeDir() + "/" + Paths::CO_DEBUG_ALGORITHM_DIR;
}

//------------------------------------------------------------------------------
// Путь к каталогу с алгоритмами программирования CoIDE
//------------------------------------------------------------------------------
QString Paths::coIdeFlashAlgorithmDir()
{
    return coIdeDir() + "/" + Paths::CO_FLASH_ALGORITHM_DIR;
}

//------------------------------------------------------------------------------
// Возвращает имя файла алгоритма отладки
//------------------------------------------------------------------------------
QString Paths::coIdeDebugAlgorithmFileName(const QString &coreName)
{
    for(auto it = Paths::_coDebugAlgorithmsMap.begin();
             it != Paths::_coDebugAlgorithmsMap.end();
           ++it)
    {
        if(it.key() == coreName)
        {
            return it.value();
        }
    }

    return "";
}

//------------------------------------------------------------------------------
// Возвращает путь к алгоритму отладки
//------------------------------------------------------------------------------
QString Paths::coIdeDebugAlgorithmFile(const QString &coreName)
{
    return coIdeDebugAlgorithmDir() + "/" + coIdeDebugAlgorithmFileName(coreName);
}

//------------------------------------------------------------------------------
// Возвращает путь к каталогу data
//------------------------------------------------------------------------------
QString Paths::coIdeDataDir()
{
    return coIdeDir() + "/" + Paths::CO_DATA_DIR;
}

//------------------------------------------------------------------------------
// Возвращает путь к каталогу пакетов DFP
//------------------------------------------------------------------------------
QString Paths::coIdePacketsDir()
{
    return coIdeDir() + "/" + Paths::CO_PACK_DIR;
}

//------------------------------------------------------------------------------
// Возвращает путь к конкретному распакованному/установленному пакету
//------------------------------------------------------------------------------
QString Paths::coIdePackDir(const QString &vendor,
                            const QString &release)
{
    return coIdePacketsDir() + "/" + vendor + "/" + release;
}

//------------------------------------------------------------------------------
// Путь к каталогу установки ядра CMSIS
//------------------------------------------------------------------------------
QString Paths::coIdeCmsisDir(QString version)
{
    QString path = coIdeDir() + "/" + Paths::CO_PACK_DIR + "/" + Paths::CO_CMSIS_PACK_DIR;

    if(!version.isEmpty())
        path += "/" + version;

    return path;
}

//------------------------------------------------------------------------------
// Путь к временной папке
//------------------------------------------------------------------------------
QString Paths::coIdeTempDir()
{
    return coIdeDir() + "/" + Paths::CO_TEMP_DIR;
}

//------------------------------------------------------------------------------
// Путь к репозиторию
//------------------------------------------------------------------------------
QString Paths::coIdeRepositoryDir()
{
    return coIdeDir() + "/" + Paths::CO_REPO_DIR;
}

//------------------------------------------------------------------------------
// Путь к каталогу компонентов
//------------------------------------------------------------------------------
QString Paths::coIdeComponentsDir()
{
    return coIdeRepositoryDir() + "/" + Paths::CO_COMPONENTS_DIR;
}

//------------------------------------------------------------------------------
// Путь к конкретному компоненту
//------------------------------------------------------------------------------
QString Paths::coIdeComponentDir(int id, const QString &name)
{
    return coIdeComponentsDir() + "/" + QString("%1_%2").arg(id).arg(name);
}

//------------------------------------------------------------------------------
// Путь к каталогу драйверов
//------------------------------------------------------------------------------
QString Paths::coIdeDriversDir()
{
    return coIdeRepositoryDir() + "/" + Paths::CO_DRIVERS_DIR;
}

//------------------------------------------------------------------------------
// Путь к конкретному драйверу
//------------------------------------------------------------------------------
QString Paths::coIdeDriverDir(int id, const QString &name)
{
    return coIdeDriversDir() + "/" + QString("%1_%2").arg(id).arg(name);
}

//------------------------------------------------------------------------------
// Путь к каталогу примеров
//------------------------------------------------------------------------------
QString Paths::coIdeExamplesDir()
{
    return coIdeRepositoryDir() + "/" + Paths::CO_EXAMPLES_DIR;
}

//------------------------------------------------------------------------------
// Путь к каталогу примера
//------------------------------------------------------------------------------
QString Paths::coIdeExampleDir(int id, const QString &name)
{
    return coIdeExamplesDir() + "/" + QString("%1_%2").arg(id).arg(name);
}

//------------------------------------------------------------------------------
// Карта доступных CMSIS Core <version>, <description>
//------------------------------------------------------------------------------
QMap<QString, QString> Paths::cmsisCores()
{
    return Paths::_cmsisCoreMap;
}

//------------------------------------------------------------------------------
// Задать новый путь к CoIDE
//------------------------------------------------------------------------------
void Paths::setCoIdeDir(const QString &dir)
{
    Settings::instance()->saveCoIdePath(dir);
}

//------------------------------------------------------------------------------
// Копирует алгоритм отладки из папки algorithms в папку назначения CoIDE
//------------------------------------------------------------------------------
void Paths::copyDebugAlgorithm(const QString& algorithmName)
{
    if(algorithmName.isEmpty()) return;

    QString destFilename = coIdeDebugAlgorithmDir() + "/" + algorithmName;
    QString sourceFilename = appDebugAlgorithmDir() + "/" + algorithmName;
    QDir destDir = coIdeDebugAlgorithmDir();
    QFile destFile(destFilename);
    QFileInfo destFileInfo(destFilename);
    QFileInfo sourceFileInfo(sourceFilename);

    if(!destDir.exists()) return;
    if(!sourceFileInfo.exists()) return;

    if(destFileInfo.exists() &&
       (destFileInfo.size() != sourceFileInfo.size() ||
        destFileInfo.lastModified() != sourceFileInfo.lastModified()))
    {
        destFile.remove();
        QFile::copy(sourceFilename, destFilename);
    }
    else if(!destFileInfo.exists())
        QFile::copy(sourceFilename, destFilename);
}

//------------------------------------------------------------------------------
// Просматривает содержимое папке компонента и возвращается список найденных файлов
//------------------------------------------------------------------------------
QStringList Paths::componentFiles(qint32 componentId, const QString& componentName)
{
    QStringList files;
    QString directory = coIdeComponentsDir() + "/" +
                        QString("%1_%2").arg(componentId).arg(componentName) + "/src";

    QDirIterator it(directory, QDir::Files, QDirIterator::Subdirectories);

    while (it.hasNext())
    {
        files << it.next();
    }

    return files;
}

//------------------------------------------------------------------------------
// Просматривает содержимое папке компонента и возвращается список найденных файлов
//------------------------------------------------------------------------------
QStringList Paths::componentFilesTruncated(qint32 componentId, const QString& componentName)
{
    QStringList files;
    QString directory = coIdeComponentsDir() + "/" +
                        QString("%1_%2").arg(componentId).arg(componentName) + "/src/";

    QDirIterator it(directory, QDir::Files, QDirIterator::Subdirectories);

    while (it.hasNext())
    {
        files << it.next().remove(directory.replace('\\', '/'));
    }

    return files;
}

//------------------------------------------------------------------------------
// Просматривает содержимое папке компонента и возвращается список найденных файлов
//------------------------------------------------------------------------------
QList<QFileInfo> Paths::componentFilesVerbose(qint32 componentId, const QString& componentName)
{
    QList<QFileInfo> fileInfoList;
    QString directory = coIdeComponentsDir() + "/" +
                        QString("%1_%2").arg(componentId).arg(componentName) + "/src/";

    QDirIterator it(directory, QDir::Files, QDirIterator::Subdirectories);

    while (it.hasNext())
    {
        QString fullPath = it.next();
        QString shortPath = fullPath.remove(directory.replace('\\', '/'));
        fileInfoList << QFileInfo(shortPath);
    }

    return fileInfoList;
}
