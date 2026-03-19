#include "mcudetailsviewmodel.h"

//------------------------------------------------------------------------------
// Конструктор
//------------------------------------------------------------------------------
McuDetailsViewModel::McuDetailsViewModel(QObject *parent) :
    QObject(parent),
    m_pack(nullptr),
    m_currentMcuObj(nullptr),
    m_codeMemory(nullptr),
    m_dataMemory(nullptr),
    m_debugAlgorithm(nullptr)
{

}

//------------------------------------------------------------------------------
// Адрес начала области FLASH-памяти
//------------------------------------------------------------------------------
QString McuDetailsViewModel::flashStart() const
{
    return m_codeMemory ? m_codeMemory->startAddrHex() : QString();
}

//------------------------------------------------------------------------------
// Объем FLASH-памяти
//------------------------------------------------------------------------------
QString McuDetailsViewModel::flashSize() const
{
    return m_codeMemory ? m_codeMemory->sizeHex() : QString();
}

//------------------------------------------------------------------------------
// Адрес начала области RAM-памяти
//------------------------------------------------------------------------------
QString McuDetailsViewModel::ramStart() const
{
    return m_dataMemory ? m_dataMemory->startAddrHex() : QString();
}

//------------------------------------------------------------------------------
// Объем RAM-памяти
//------------------------------------------------------------------------------
QString McuDetailsViewModel::ramSize() const
{
    return m_dataMemory ? m_dataMemory->sizeHex() : QString();
}

//------------------------------------------------------------------------------
// Характеристики MCU
//------------------------------------------------------------------------------
QString McuDetailsViewModel::features() const
{
    if (!m_currentMcuObj) return QString();
    return m_currentMcuObj->featuresSummary().join("\n");
}

//------------------------------------------------------------------------------
// Описание MCU
//------------------------------------------------------------------------------
QString McuDetailsViewModel::description() const
{
    if (!m_currentMcuObj) return QString();

    if (!m_currentMcuObj->getDescription().isEmpty())
        return m_currentMcuObj->getDescription();
    if (!m_currentMcuObj->coDescription().isEmpty())
        return m_currentMcuObj->coDescription();
    return m_pack->description();
}

//------------------------------------------------------------------------------
// Веб-страница производителя mcu
//------------------------------------------------------------------------------
QString McuDetailsViewModel::webPageUrl() const
{
    if (!m_currentMcuObj) return QString();
    return m_currentMcuObj->getWebPageURL();
}

//------------------------------------------------------------------------------
// Ссылка на даташит
//------------------------------------------------------------------------------
QString McuDetailsViewModel::datasheetUrl() const
{
    if (!m_currentMcuObj) return QString();
    return m_currentMcuObj->getDatasheetURL();
}

//------------------------------------------------------------------------------
// Путь к SVD-файлу внутри пакета
//------------------------------------------------------------------------------
QString McuDetailsViewModel::svdLocalPath() const
{
    if (!m_currentMcuObj) return QString();
    return m_currentMcuObj->svdLocalPath();
}

//------------------------------------------------------------------------------
// Алгоритм отладки
//------------------------------------------------------------------------------
QString McuDetailsViewModel::debugAlgorithm() const
{
    if (!m_debugAlgorithm) return QString();
    return m_debugAlgorithm->name();
}

//------------------------------------------------------------------------------
// Выбор MCU и загрузка подробной информации об устройстве
//------------------------------------------------------------------------------
void McuDetailsViewModel::selectMcu(QString vendor,
                                    QString family,
                                    QString series,
                                    QString mcu)
{
    emit loadDataStarted();

    //
    // Обнуление кэшированных данных
    //
    m_currentMcuObj = nullptr;
    m_codeMemory = nullptr;
    m_dataMemory = nullptr;
    m_debugAlgorithm = nullptr;
    m_flashAlgorithms.clear();
    m_currentFlashAlgorithm.clear();

    if(!m_pack || !m_pack->isValid() || vendor.isEmpty() || family.isEmpty() || series.isEmpty() || mcu.isEmpty())
    {
        emit loadDataFailed();
        return;
    }

    //
    // Загрузка данных только для уже существующих устройств
    //
    if(m_pack->vendors().keys().contains(vendor) &&
       m_pack->vendor(vendor).families().keys().contains(family) &&
       m_pack->vendor(vendor).family(family).seriesMap().keys().contains(series) &&
       m_pack->vendor(vendor).family(family).series(series).mcuMap().keys().contains(mcu))
    {
        // Получаем объект MCU
        m_currentMcuObj = &m_pack->vendor(vendor).family(family).series(series).mcu(mcu);

        m_codeMemory = m_currentMcuObj->getCodeMemory();
        m_dataMemory = m_currentMcuObj->getDataMemory();
        m_debugAlgorithm = &m_currentMcuObj->getDebugAlgorithm();

        int32_t flashStartAddr = m_codeMemory ? m_codeMemory->startAddr() : -1;
        ProgAlgorithm * flashAlgorithm = m_currentMcuObj->getFlashAlgorithm(flashStartAddr);

        m_currentFlashAlgorithm = flashAlgorithm ? flashAlgorithm->name() : QString();
        m_flashAlgorithms.clear();

        foreach(auto algo, m_currentMcuObj->algorithms())
        {
            m_flashAlgorithms.append(algo.name());
        }

        emit loadDataFinished();
        emit mcuChanged();
    }
}


