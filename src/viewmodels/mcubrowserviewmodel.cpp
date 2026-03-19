#include <QtConcurrent/QtConcurrent>
#include "mcubrowserviewmodel.h"
#include "services/settings.h"

//------------------------------------------------------------------------------
// Конструктор
//------------------------------------------------------------------------------
McuBrowserViewModel::McuBrowserViewModel(QObject *parent) : QObject(parent)
{
    connect(this, &McuBrowserViewModel::loadResult,
            this, &McuBrowserViewModel::onLoadResult,
            Qt::QueuedConnection);
}

//------------------------------------------------------------------------------
// Деструктор
//------------------------------------------------------------------------------
McuBrowserViewModel::~McuBrowserViewModel()
{
    saveSelection();
}

//------------------------------------------------------------------------------
// Возвращает выбранного производителя
//------------------------------------------------------------------------------
QString McuBrowserViewModel::selectedVendor() const
{
    return m_selectedNode.type == DeviceNode::VendorType ?
                                  m_selectedNode.name :
                                  m_selectedNode.vendorName;
}

//------------------------------------------------------------------------------
// Возвращает выбранное семейство
//------------------------------------------------------------------------------
QString McuBrowserViewModel::selectedFamily() const
{
    return m_selectedNode.type == DeviceNode::FamilyType ?
                                  m_selectedNode.name :
                                  m_selectedNode.familyName;
}

//------------------------------------------------------------------------------
// Возвращает выбранную серию
//------------------------------------------------------------------------------
QString McuBrowserViewModel::selectedSeries() const
{
    return m_selectedNode.type == DeviceNode::SeriesType ?
                                  m_selectedNode.name :
                                  m_selectedNode.seriesName;
}

//------------------------------------------------------------------------------
// Возвращает выбранной чип
//------------------------------------------------------------------------------
QString McuBrowserViewModel::selectedMcu() const
{
    return m_selectedNode.type == DeviceNode::McuType ? m_selectedNode.name : QString();
}

//------------------------------------------------------------------------------
// Выбор узла
//------------------------------------------------------------------------------
void McuBrowserViewModel::selectNode(const DeviceNode& node)
{
    m_selectedNode = node;
}

//------------------------------------------------------------------------------
// Сохранение информации о выбранном устройстве
//------------------------------------------------------------------------------
void McuBrowserViewModel::saveSelection()
{
    if(!m_selectedNode.isValid()) return;

    // Сохраняем в настройках
    Settings::instance()->saveSelectedVendor(m_selectedNode.vendorName);
    Settings::instance()->saveSelectedCore(m_selectedNode.familyName);
    Settings::instance()->saveSelectedSeries(m_selectedNode.seriesName);
    Settings::instance()->saveSelectedMcu(m_selectedNode.isMcu() ? m_selectedNode.name : QString());
}

//------------------------------------------------------------------------------
// Загрузка последнего загруженного пакета
//------------------------------------------------------------------------------
bool McuBrowserViewModel::loadPack()
{
    return loadPack(Settings::instance()->lastLoadedPack());
}

//------------------------------------------------------------------------------
// Загрузка и разбор пакета
//------------------------------------------------------------------------------
bool McuBrowserViewModel::loadPack(const QString& path)
{
    m_pack.clear();
    m_pack.setPathToArchive(path);
    m_packManager.readPackDescription(m_pack);

    buildDeviceTree();
    restoreSelection();

    return true;
}

//------------------------------------------------------------------------------
// Загрузка и разбор пакета
//------------------------------------------------------------------------------
void McuBrowserViewModel::loadPackAsync()
{
    loadPackAsync(Settings::instance()->lastLoadedPack());
}

//------------------------------------------------------------------------------
// Загрузка и разбор пакета
//------------------------------------------------------------------------------
void McuBrowserViewModel::loadPackAsync(const QString &path)
{
    emit loadStarted();

    QtConcurrent::run([this, path]()
    {
        QString errorString;
        bool success = loadPack(path);
        emit loadResult(success, errorString);
    });
}

void McuBrowserViewModel::onLoadResult(bool success, QString errorString)
{
    Q_UNUSED(errorString)

    emit packLoaded(success);
}

//------------------------------------------------------------------------------
// Построение дерева устройств
//------------------------------------------------------------------------------
void McuBrowserViewModel::buildDeviceTree()
{
    m_deviceTree.clear();
    m_mcuCache.clear();

    QStringList vendors = m_pack.vendors().keys();

    for (const QString& vendor : vendors)
    {
        m_deviceTree.append(buildVendorNode(vendor));
    }
}

//------------------------------------------------------------------------------
// Построение ветки "Производитель"
//------------------------------------------------------------------------------
DeviceNode McuBrowserViewModel::buildVendorNode(const QString& vendor)
{
    DeviceNode node;
    node.type = DeviceNode::VendorType;
    node.name = vendor;
    node.displayName = vendor;

    QStringList families = m_pack.vendor(vendor).families().keys();

    for (const QString& family : families)
    {
        node.children.append(buildFamilyNode(vendor, family));
    }

    return node;
}

//------------------------------------------------------------------------------
// Построение ветки "Семейство"
//------------------------------------------------------------------------------
DeviceNode McuBrowserViewModel::buildFamilyNode(const QString& vendor,
                                                const QString& family)
{
    DeviceNode node;
    node.type = DeviceNode::FamilyType;
    node.name = family;
    node.displayName = family;
    node.vendorName = vendor;

    QStringList series = m_pack.vendor(vendor).family(family).seriesMap().keys();

    for (const QString& serie : series)
    {
        node.children.append(buildSeriesNode(vendor, family, serie));
    }

    return node;
}

//------------------------------------------------------------------------------
// Построение ветки "Серия"
//------------------------------------------------------------------------------
DeviceNode McuBrowserViewModel::buildSeriesNode(const QString& vendor,
                                                const QString& family,
                                                const QString& series)
{
    DeviceNode node;
    node.type = DeviceNode::SeriesType;
    node.name = series;
    node.displayName = series;
    node.vendorName = vendor;
    node.familyName = family;

    QStringList mcus = m_pack.vendor(vendor).family(family).series(series).mcuMap().keys();

    for (const QString& mcu : mcus)
    {
        node.children.append(buildMcuNode(vendor, family, series, mcu));
    }

    return node;
}

//------------------------------------------------------------------------------
// Создание узла "Микроконтроллер"
//------------------------------------------------------------------------------
DeviceNode McuBrowserViewModel::buildMcuNode(const QString& vendor,
                                             const QString& family,
                                             const QString& series,
                                             const QString& mcu)
{
    DeviceNode node;
    node.type = DeviceNode::McuType;
    node.name = mcu;
    node.displayName = mcu;
    node.vendorName = vendor;
    node.familyName = family;
    node.seriesName = series;

    // Кэшируем объект MCU для быстрого доступа
    Mcu* mcuObj = &m_pack.vendor(vendor).family(family).series(series).mcu(mcu);
    m_mcuCache[mcu] = mcuObj;

    return node;
}

//------------------------------------------------------------------------------
// Восстановление выбора
//------------------------------------------------------------------------------
void McuBrowserViewModel::restoreSelection()
{
    QString vendor = Settings::instance()->selectedVendor();
    QString family = Settings::instance()->selectedCore();
    QString series = Settings::instance()->selectedSeries();
    QString mcu = Settings::instance()->selectedMcu();

    // Находим узел
    DeviceNode* node = findNode(vendor, family, series, mcu);

    if (node)
    {
        m_selectedNode = *node;
    }
}

//------------------------------------------------------------------------------
// Поиск узла
//------------------------------------------------------------------------------
DeviceNode* McuBrowserViewModel::findNode(const QString& vendor,
                                          const QString& family,
                                          const QString& series,
                                          const QString& mcu)
{
    for (auto& vNode : m_deviceTree)
    {
        if (vNode.name == vendor)
        {
            if (family.isEmpty()) return &vNode;

            for (auto& fNode : vNode.children)
            {
                if (fNode.name == family)
                {
                    if (series.isEmpty()) return &fNode;

                    for (auto& sNode : fNode.children)
                    {
                        if (sNode.name == series)
                        {
                            if (mcu.isEmpty()) return &sNode;

                            for (auto& mNode : sNode.children)
                            {
                                if (mNode.name == mcu)
                                {
                                    return &mNode;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return nullptr;
}

//------------------------------------------------------------------------------
// Начальный адрес Flash
//------------------------------------------------------------------------------
QString McuBrowserViewModel::flashStart() const
{
    if (!m_selectedNode.isMcu()) return QString();

    Mcu* mcu = m_mcuCache.value(m_selectedNode.name);
    if (!mcu) return QString();

    Memory* mem = mcu->getCodeMemory();
    return mem ? mem->startAddrHex() : QString();
}

//------------------------------------------------------------------------------
// Размер Flash
//------------------------------------------------------------------------------
QString McuBrowserViewModel::flashSize() const
{
    if (!m_selectedNode.isMcu()) return QString();

    Mcu* mcu = m_mcuCache.value(m_selectedNode.name);
    if (!mcu) return QString();

    Memory* mem = mcu->getCodeMemory();
    return mem ? mem->sizeHex() : QString();
}

//------------------------------------------------------------------------------
// Начальный адрес Ram
//------------------------------------------------------------------------------
QString McuBrowserViewModel::ramStart() const
{
    if (!m_selectedNode.isMcu()) return QString();

    Mcu* mcu = m_mcuCache.value(m_selectedNode.name);
    if (!mcu) return QString();

    Memory* mem = mcu->getDataMemory();
    return mem ? mem->startAddrHex() : QString();
}

//------------------------------------------------------------------------------
// Размер Ram
//------------------------------------------------------------------------------
QString McuBrowserViewModel::ramSize() const
{
    if (!m_selectedNode.isMcu()) return QString();

    Mcu* mcu = m_mcuCache.value(m_selectedNode.name);
    if (!mcu) return QString();

    Memory* mem = mcu->getDataMemory();
    return mem ? mem->sizeHex() : QString();
}

//------------------------------------------------------------------------------
// Характеристики MCU
//------------------------------------------------------------------------------
QString McuBrowserViewModel::features() const
{
    if (!m_selectedNode.isMcu()) return QString();

    Mcu* mcu = m_mcuCache.value(m_selectedNode.name);
    if (!mcu) return QString();

    return mcu->featuresSummary().join("\n");
}

//------------------------------------------------------------------------------
// Описание MCU
//------------------------------------------------------------------------------
QString McuBrowserViewModel::description() const
{
    if(m_selectedNode.isMcu())
    {
        Mcu* mcu = m_mcuCache.value(m_selectedNode.name);

        if (!mcu)
            return QString();
        else if(!mcu->getDescription().isEmpty())
            return mcu->getDescription();
        else if(!mcu->coDescription().isEmpty())
            return mcu->coDescription();
        else
            return m_pack.description();
    }
    else
        return m_pack.description();
}

//------------------------------------------------------------------------------
// Веб-страница производителя mcu
//------------------------------------------------------------------------------
QString McuBrowserViewModel::webPageUrl() const
{
    if (!m_selectedNode.isMcu()) return QString();

    Mcu* mcu = m_mcuCache.value(m_selectedNode.name);
    if (!mcu) return QString();

    return mcu->getWebPageURL();
}

//------------------------------------------------------------------------------
// Ссылка на даташит
//------------------------------------------------------------------------------
QString McuBrowserViewModel::datasheetUrl() const
{
    if (!m_selectedNode.isMcu()) return QString();

    Mcu* mcu = m_mcuCache.value(m_selectedNode.name);
    if (!mcu) return QString();

    return mcu->getDatasheetURL();
}

//------------------------------------------------------------------------------
// Путь к SVD-файлу внутри пакета
//------------------------------------------------------------------------------
QString McuBrowserViewModel::svdLocalPath() const
{
    if (!m_selectedNode.isMcu()) return QString();

    Mcu* mcu = m_mcuCache.value(m_selectedNode.name);
    if (!mcu) return QString();

    return mcu->svdLocalPath();
}

//------------------------------------------------------------------------------
// Алгоритм отладки
//------------------------------------------------------------------------------
QString McuBrowserViewModel::debugAlgorithm() const
{
    if (!m_selectedNode.isMcu()) return QString();

    Mcu* mcu = m_mcuCache.value(m_selectedNode.name);
    if (!mcu) return QString();

    return mcu->getDebugAlgorithm().name();
}

//------------------------------------------------------------------------------
// Алгоритмы программирования
//------------------------------------------------------------------------------
QStringList McuBrowserViewModel::flashAlgorithms() const
{
    if (!m_selectedNode.isMcu()) return QStringList();

    Mcu* mcu = m_mcuCache.value(m_selectedNode.name);
    if (!mcu) return QStringList();

    QStringList algoList;

    for(auto algo : mcu->algorithms())
    {
        algoList << algo.name();
    }

    return algoList;
}

//------------------------------------------------------------------------------
// Основной алгоритм программирования
//------------------------------------------------------------------------------
QString McuBrowserViewModel::defaultFlashAlgorithm() const
{
    if (!m_selectedNode.isMcu()) return QString();

    Mcu* mcu = m_mcuCache.value(m_selectedNode.name);
    if (!mcu)
        return QString();
    else
    {
        auto codeMem = mcu->getCodeMemory();
        int32_t flashStartAddr = codeMem ? codeMem->startAddr() : -1;
        ProgAlgorithm * flashAlgorithm = mcu->getFlashAlgorithm(flashStartAddr);

        return flashAlgorithm ? flashAlgorithm->name() : QString();
    }
}

//------------------------------------------------------------------------------
// Версия пакета
//------------------------------------------------------------------------------
QString McuBrowserViewModel::releaseVersion() const
{
    return m_pack.release();
}
