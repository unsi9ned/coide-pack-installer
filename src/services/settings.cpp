#include <QApplication>
#include "settings.h"
#include "paths.h"

//------------------------------------------------------------------------------
// Статические переменные
//------------------------------------------------------------------------------
Settings* Settings::m_instance = nullptr;

//------------------------------------------------------------------------------
// Конструктор формы
//------------------------------------------------------------------------------
Settings::Settings(QObject *parent) : QObject(parent)
{

}

//------------------------------------------------------------------------------
// Создание единственного объекта
//------------------------------------------------------------------------------
Settings *Settings::instance()
{
    if(!m_instance)
        m_instance = new Settings();

    return m_instance;
}

//------------------------------------------------------------------------------
// Удаляем единственный объект
//------------------------------------------------------------------------------
Settings::~Settings()
{
    delete m_instance;
}

//------------------------------------------------------------------------------
// Получить путь к каталогу с CoIDE
//------------------------------------------------------------------------------
QString Settings::coIdePath()
{
    return Settings::getCustomParameter(MAIN_SETTINGS_GRP,
                                        MAIN_PARAMETER_COIDE,
                                        Paths::instance()->appDirPath()).toString();
}

//------------------------------------------------------------------------------
// Путь к последнему загруженному пакету
//------------------------------------------------------------------------------
QString Settings::lastLoadedPack()
{
    return Settings::getCustomParameter(MAIN_SETTINGS_GRP,
                                        MAIN_PARAMETER_PACK,
                                        Paths::instance()->appDirPath()).toString();
}

//------------------------------------------------------------------------------
// Возвращает имя производителя, которого пользователь выбрал после загрузки пакета
//------------------------------------------------------------------------------
QString Settings::selectedVendor()
{
    return Settings::getCustomParameter(DEVICE_SETTINGS_GRP,
                                        DEVICE_PARAMETER_VENDOR).toString();
}

//------------------------------------------------------------------------------
// Возвращает имя ядра, которое пользователь выбрал после загрузки пакета
//------------------------------------------------------------------------------
QString Settings::selectedCore()
{
    return Settings::getCustomParameter(DEVICE_SETTINGS_GRP,
                                        DEVICE_PARAMETER_CORE).toString();
}

//------------------------------------------------------------------------------
// Возвращает серию, которою пользователь выбрал после загрузки пакета
//------------------------------------------------------------------------------
QString Settings::selectedSeries()
{
    return Settings::getCustomParameter(DEVICE_SETTINGS_GRP,
                                        DEVICE_PARAMETER_SERIES).toString();
}

//------------------------------------------------------------------------------
// Возвращает имя mcu, которое пользователь выбрал после загрузки пакета
//------------------------------------------------------------------------------
QString Settings::selectedMcu()
{
    return Settings::getCustomParameter(DEVICE_SETTINGS_GRP,
                                        DEVICE_PARAMETER_MCU).toString();
}

//------------------------------------------------------------------------------
// Сохранить имя выбранного ком-порта в файл конфигурации
//------------------------------------------------------------------------------
void Settings::saveCoIdePath(const QString &path)
{
    Settings::saveCustomParameter(MAIN_SETTINGS_GRP, MAIN_PARAMETER_COIDE, path);
}

//------------------------------------------------------------------------------
// Сохранить путь к последнему загруженному пакету
//------------------------------------------------------------------------------
void Settings::saveLastLoadedPack(const QString &path)
{
    Settings::saveCustomParameter(MAIN_SETTINGS_GRP, MAIN_PARAMETER_PACK, path);
}

//------------------------------------------------------------------------------
// Сохраняет имя производителя, которого пользователь выбрал после загрузки пакета
//------------------------------------------------------------------------------
void Settings::saveSelectedVendor(const QString &vendor)
{
    Settings::saveCustomParameter(DEVICE_SETTINGS_GRP, DEVICE_PARAMETER_VENDOR, vendor);
}

//------------------------------------------------------------------------------
// Сохраняет имя ядра, которое пользователь выбрал после загрузки пакета
//------------------------------------------------------------------------------
void Settings::saveSelectedCore(const QString &core)
{
    Settings::saveCustomParameter(DEVICE_SETTINGS_GRP, DEVICE_PARAMETER_CORE, core);
}

//------------------------------------------------------------------------------
// Сохраняет серию, которою пользователь выбрал после загрузки пакета
//------------------------------------------------------------------------------
void Settings::saveSelectedSeries(const QString &series)
{
    Settings::saveCustomParameter(DEVICE_SETTINGS_GRP, DEVICE_PARAMETER_SERIES, series);
}

//------------------------------------------------------------------------------
// Сохраняет имя mcu, которое пользователь выбрал после загрузки пакета
//------------------------------------------------------------------------------
void Settings::saveSelectedMcu(const QString &mcu)
{
    Settings::saveCustomParameter(DEVICE_SETTINGS_GRP, DEVICE_PARAMETER_MCU, mcu);
}

//------------------------------------------------------------------------------
// Сохранить произвольный параметр
//------------------------------------------------------------------------------
void Settings::saveCustomParameter(const QString &group, const QString &key, QVariant value)
{
    QSettings settings(Paths::instance()->appSettingsFile(), QSettings::IniFormat);

    settings.beginGroup(group);
    settings.setValue(key, value.toString());
    settings.endGroup();
    settings.sync();
}

//------------------------------------------------------------------------------
// Загрузить произвольный параметр
//------------------------------------------------------------------------------
QVariant Settings::getCustomParameter(const QString &group, const QString &key, QVariant defaultValue)
{
    QVariant value;
    QSettings settings(Paths::instance()->appSettingsFile(), QSettings::IniFormat);

    settings.beginGroup(group);
    value = settings.value(key, defaultValue);
    settings.endGroup();

    return value;
}




