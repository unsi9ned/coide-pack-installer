#include <QApplication>
#include <QDir>
#include <QFile>
#include "paths.h"

Paths* Paths::m_instance = nullptr;

const QMap<QString, QString> Paths::_coDebugAlgorithmsMap = {
    {"Cortex-M0",  "cortex-m0.xml"},
    {"Cortex-M0+", "cortex-m0plus.xml"},
    {"Cortex-M3",  "cortex-m3.xml"},
    {"Cortex-M4",  "cortex-m4.xml"},

    //TODO временное решение для полной установки пакетов с новыми ядрами,
    //которых нет в CoIDE
    {"Cortex-M33",  "cortex-m3.xml"},
};

//------------------------------------------------------------------------------
// Загрузка пути к каталогу CoIDE
//------------------------------------------------------------------------------
Paths::Paths(QObject *parent) : QObject(parent)
{
    QFile settingFile(appSettingsFile());
    QByteArray idePath;

    if(settingFile.exists() && settingFile.open(QFile::ReadOnly))
    {
        idePath = settingFile.readLine();

        if(idePath.isEmpty())
            _coIdeDirectory = QApplication::applicationDirPath();
        else
            _coIdeDirectory = QString(idePath);

        settingFile.close();
    }
    else
    {
        _coIdeDirectory = QApplication::applicationDirPath();
    }
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
// Путь к файлу настроек
//------------------------------------------------------------------------------
QString Paths::appSettingsFile()
{
    return QApplication::applicationDirPath() + "/" + Paths::APP_SETTINGS;
}

//------------------------------------------------------------------------------
// Путь к программе-архиватору 7z
//------------------------------------------------------------------------------
QString Paths::sevenZipExe()
{
    return QApplication::applicationDirPath() + "/" + Paths::APP_UTILS_DIR + "/" + Paths::SEVEN_ZIP;
}

//------------------------------------------------------------------------------
// Путь к CoIDE
//------------------------------------------------------------------------------
QString Paths::coIdeDir()
{
    return _coIdeDirectory;
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
// Задать новый путь к CoIDE
//------------------------------------------------------------------------------
void Paths::setCoIdeDir(const QString &dir)
{
    if(dir.isEmpty())
        return;

    QDir coIdeDir;
    coIdeDir.setPath(dir);

    if(coIdeDir.exists())
    {
        QFile settingFile(appSettingsFile());

        if(settingFile.open(QFile::WriteOnly))
        {
            _coIdeDirectory = coIdeDir.path();
            settingFile.write(coIdeDir.path().toLatin1());
            settingFile.close();
        }
    }
}
