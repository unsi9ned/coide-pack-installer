#include <QDebug>
#include "mainviewmodel.h"
#include "services/settings.h"

//------------------------------------------------------------------------------
// Конструктор
//------------------------------------------------------------------------------
MainViewModel::MainViewModel(QObject *parent) : QObject(parent)
{

}

//------------------------------------------------------------------------------
// Адрес начала области FLASH-памяти
//------------------------------------------------------------------------------
QString MainViewModel::flashStart() const
{
    return m_codeMemory ? m_codeMemory->startAddrHex() : QString();
}

//------------------------------------------------------------------------------
// Объем FLASH-памяти
//------------------------------------------------------------------------------
QString MainViewModel::flashSize() const
{
    return m_codeMemory ? m_codeMemory->sizeHex() : QString();
}

//------------------------------------------------------------------------------
// Адрес начала области RAM-памяти
//------------------------------------------------------------------------------
QString MainViewModel::ramStart() const
{
    return m_dataMemory ? m_dataMemory->startAddrHex() : QString();
}

//------------------------------------------------------------------------------
// Объем RAM-памяти
//------------------------------------------------------------------------------
QString MainViewModel::ramSize() const
{
    return m_dataMemory ? m_dataMemory->sizeHex() : QString();
}

//------------------------------------------------------------------------------
// Характеристики MCU
//------------------------------------------------------------------------------
QString MainViewModel::features() const
{
    if (!m_currentMcuObj) return QString();
    return m_currentMcuObj->featuresSummary().join("\n");
}

//------------------------------------------------------------------------------
// Описание MCU
//------------------------------------------------------------------------------
QString MainViewModel::description() const
{
    if (!m_currentMcuObj) return QString();

    if (!m_currentMcuObj->getDescription().isEmpty())
        return m_currentMcuObj->getDescription();
    if (!m_currentMcuObj->coDescription().isEmpty())
        return m_currentMcuObj->coDescription();
    return m_pack.description();
}

//------------------------------------------------------------------------------
// Веб-страница производителя mcu
//------------------------------------------------------------------------------
QString MainViewModel::webPageUrl() const
{
    if (!m_currentMcuObj) return QString();
    return m_currentMcuObj->getWebPageURL();
}

//------------------------------------------------------------------------------
// Ссылка на даташит
//------------------------------------------------------------------------------
QString MainViewModel::datasheetUrl() const
{
    if (!m_currentMcuObj) return QString();
    return m_currentMcuObj->getDatasheetURL();
}

//------------------------------------------------------------------------------
// Путь к SVD-файлу внутри пакета
//------------------------------------------------------------------------------
QString MainViewModel::svdLocalPath() const
{
    if (!m_currentMcuObj) return QString();
    return m_currentMcuObj->svdLocalPath();
}

//------------------------------------------------------------------------------
// Версия пакета
//------------------------------------------------------------------------------
QString MainViewModel::releaseVersion() const
{
    return m_pack.release();
}

//------------------------------------------------------------------------------
// Загрузка пакета *.pack
//------------------------------------------------------------------------------
void MainViewModel::loadDeviceFamilyPack()
{
    loadDeviceFamilyPack(Settings::instance()->lastLoadedPack());
}

//------------------------------------------------------------------------------
// Загрузка пакета *.pack
// path - путь к файлу *.pack
//------------------------------------------------------------------------------
void MainViewModel::loadDeviceFamilyPack(const QString &path)
{
    if(path.isEmpty()) return;

    emit loadStarted();
    emit statusMessage("Загрузка пакета...", 0);

    // Сохраняем путь к пакету
    Settings::instance()->saveLastLoadedPack(path);

    // Очищаем и загружаем новый пакет
    m_pack.clear();
    m_pack.setPathToArchive(path);
    m_packManager.readPackDescription(m_pack);

    // Обновляем список производителей
    m_vendors.clear();

    for(auto v = m_pack.vendors().begin(); v != m_pack.vendors().end(); ++v)
    {
        m_vendors.append(v.key());
    }
    emit vendorsChanged();

    // Сбрасываем выбор
    m_currentVendor.clear();
    m_currentFamily.clear();
    m_currentSeries.clear();
    m_currentMcu.clear();
    m_currentMcuObj = nullptr;
    m_codeMemory = nullptr;
    m_dataMemory = nullptr;

    emit familiesChanged();
    emit seriesChanged();
    emit mcusChanged();
    emit mcuChanged();

    emit loadFinished();
    emit statusMessage("Пакет загружен", 3000);

    // Восстанавливаем выбор
    restoreSelection();
}

//------------------------------------------------------------------------------
// Восстановление выбора устройства позле загрузки пакета
//------------------------------------------------------------------------------
void MainViewModel::restoreSelection()
{
    QString vendor = Settings::instance()->selectedVendor();
    QString family = Settings::instance()->selectedCore();
    QString series = Settings::instance()->selectedSeries();
    QString mcu = Settings::instance()->selectedMcu();

    // Восстанавливаем производителя
    if (!vendor.isEmpty())
    {
        selectVendor(vendor);

        // Восстанавливаем семейство
        if (!family.isEmpty())
        {
            selectFamily(family);

            // Восстанавливаем серию
            if (!series.isEmpty())
            {
                selectSeries(series);

                // Восстанавливаем MCU
                if (!mcu.isEmpty())
                {
                    selectMcu(mcu);
                }
            }
        }
    }
}

//------------------------------------------------------------------------------
// Перезагрузка списка семейств
//------------------------------------------------------------------------------
void MainViewModel::updateFamilies()
{
    auto families = m_pack.vendor(m_currentVendor).families();

    m_families.clear();

    for(auto f = families.begin(); f != families.end(); ++f)
    {
        m_families.append(f.key());
    }

    emit familiesChanged();

    // Сбрасываем зависимые списки
    m_series.clear();
    emit seriesChanged();

    m_mcus.clear();
    emit mcusChanged();
}

//------------------------------------------------------------------------------
// Перезагрузка списка серий устройств
//------------------------------------------------------------------------------
void MainViewModel::updateSeries()
{
    auto series = m_pack.vendor(m_currentVendor).family(m_currentFamily).seriesMap();

    m_series.clear();

    for(auto s = series.begin(); s != series.end(); ++s)
    {
        m_series.append(s.key());
    }

    emit seriesChanged();

    m_mcus.clear();
    emit mcusChanged();
}

//------------------------------------------------------------------------------
// Перезагрузка списка устройств
//------------------------------------------------------------------------------
void MainViewModel::updateMcus()
{
    auto devices = m_pack.vendor(m_currentVendor).family(m_currentFamily).series(m_currentSeries).mcuMap();

    m_mcus.clear();

    for(auto d = devices.begin(); d != devices.end(); ++d)
    {
        m_mcus.append(d.key());
    }

    emit mcusChanged();
}

//------------------------------------------------------------------------------
// Перезагрузка информации об устройстве
//------------------------------------------------------------------------------
void MainViewModel::updateMcuDetails()
{
    // Получаем объект MCU
    m_currentMcuObj = &m_pack.vendor(m_currentVendor)
                           .family(m_currentFamily)
                           .series(m_currentSeries)
                           .mcu(m_currentMcu);

    m_codeMemory = m_currentMcuObj->getCodeMemory();
    m_dataMemory = m_currentMcuObj->getDataMemory();

    emit mcuChanged();
}

//------------------------------------------------------------------------------
// Выбор производителя
//------------------------------------------------------------------------------
void MainViewModel::selectVendor(const QString &vendor)
{
    if(!m_pack.vendors().keys().contains(vendor)) return;

    m_currentVendor = vendor;
    Settings::instance()->saveSelectedVendor(m_currentVendor);

    updateFamilies();
}

//------------------------------------------------------------------------------
// Выбор семейства
//------------------------------------------------------------------------------
void MainViewModel::selectFamily(const QString& family)
{
    QStringList vendorList = m_pack.vendors().keys();
    QStringList familyList = vendorList.contains(m_currentVendor) ?
                             m_pack.vendor(m_currentVendor).families().keys() :
                             QStringList();

    if(!familyList.contains(family)) return;

    m_currentFamily = family;
    Settings::instance()->saveSelectedCore(m_currentFamily);

    updateSeries();
}

//------------------------------------------------------------------------------
// Выбор серии
//------------------------------------------------------------------------------
void MainViewModel::selectSeries(const QString &series)
{
    if(m_pack.vendors().keys().contains(m_currentVendor) &&
       m_pack.vendor(m_currentVendor).families().keys().contains(m_currentFamily) &&
       m_pack.vendor(m_currentVendor).family(m_currentFamily).seriesMap().keys().contains(series))
    {
        m_currentSeries = series;
        Settings::instance()->saveSelectedSeries(m_currentSeries);

        updateMcus();
    }
}

//------------------------------------------------------------------------------
// Выбор устройства
//------------------------------------------------------------------------------
void MainViewModel::selectMcu(const QString& mcu)
{
    if(m_pack.vendors().keys().contains(m_currentVendor) &&
       m_pack.vendor(m_currentVendor).families().keys().contains(m_currentFamily) &&
       m_pack.vendor(m_currentVendor).family(m_currentFamily).seriesMap().keys().contains(m_currentSeries) &&
       m_pack.vendor(m_currentVendor).family(m_currentFamily).series(m_currentSeries).mcuMap().keys().contains(mcu))
    {
        m_currentMcu = mcu;
        Settings::instance()->saveSelectedMcu(m_currentMcu);

        updateMcuDetails();
    }
}
