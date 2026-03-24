#include <QtConcurrent/QtConcurrent>
#include "mcubrowserviewmodel.h"
#include "services/settings.h"
#include "models/database/dbgarbagecollector.h"

#define COMPONENT_BOTTOM_UP_HIERACHY  1
#define HIDE_UNRELATED_COMPONENTS     0

//------------------------------------------------------------------------------
// Конструктор
//------------------------------------------------------------------------------
McuBrowserViewModel::McuBrowserViewModel(QObject *parent) : QObject(parent)
{
    connect(this, &McuBrowserViewModel::loadResult,
            this, &McuBrowserViewModel::onLoadResult,
            Qt::QueuedConnection);

    connect(this, &McuBrowserViewModel::installResult,
            this, &McuBrowserViewModel::onInstallResult,
            Qt::QueuedConnection);

    connect(&m_packManager,
            &PackManager::eventOccured,
            this,
            &McuBrowserViewModel::loadLogMessage);

    connect(&m_packManager,
            &PackManager::errorOccured,
            this,
            &McuBrowserViewModel::loadLogMessage);
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
    return m_selectedNode.getVendorName();
}

//------------------------------------------------------------------------------
// Возвращает выбранное семейство
//------------------------------------------------------------------------------
QString McuBrowserViewModel::selectedFamily() const
{
    return m_selectedNode.getFamilyName();
}

//------------------------------------------------------------------------------
// Возвращает выбранную серию
//------------------------------------------------------------------------------
QString McuBrowserViewModel::selectedSeries() const
{
    return m_selectedNode.getSeriesName();
}

//------------------------------------------------------------------------------
// Возвращает выбранной чип
//------------------------------------------------------------------------------
QString McuBrowserViewModel::selectedMcu() const
{
    return m_selectedNode.getMcuName();
}

//------------------------------------------------------------------------------
// Выбор узла
//------------------------------------------------------------------------------
void McuBrowserViewModel::selectNode(const DeviceNode& node)
{
    m_selectedNode = node;
}

//------------------------------------------------------------------------------
// Выбор узла компонента
//------------------------------------------------------------------------------
void McuBrowserViewModel::selectComponentNode(const ComponentNode& node)
{
    if(node.isValid())
        m_selectedComponentNode = node;
}

//------------------------------------------------------------------------------
// Сохранение информации о выбранном устройстве
//------------------------------------------------------------------------------
void McuBrowserViewModel::saveSelection()
{
    if(!m_selectedNode.isValid()) return;

    // Сохраняем в настройках
    Settings::instance()->saveSelectedVendor(m_selectedNode.getVendorName());
    Settings::instance()->saveSelectedCore(m_selectedNode.getFamilyName());
    Settings::instance()->saveSelectedSeries(m_selectedNode.getSeriesName());
    Settings::instance()->saveSelectedMcu(m_selectedNode.getMcuName());
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

    buildComponentTree();

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
// Установка пакета
//------------------------------------------------------------------------------
void McuBrowserViewModel::installCurrentPack()
{
    if (m_pack.pathToArchive().isEmpty())
    {
        emit installLogMessage("Нет загруженного пакета");
        return;
    }

    emit installStarted();

    // Асинхронная установка
    QtConcurrent::run([this]()
    {
        QString errorString;
        PackDescription localPack = m_pack;
        PackManager localManager;

        QObject::connect(&localManager, &PackManager::errorOccured,[this](const QString& e)
        {
            emit installLogMessage(e);
        });

        QObject::connect(&localManager, &PackManager::eventOccured,[this](const QString& e)
        {
            emit installLogMessage(e);
        });

        bool success = localManager.packInstall(localPack, errorString);
        emit installResult(success, errorString);
    });
}

void McuBrowserViewModel::onInstallResult(bool success, QString errorString)
{
    if (success)
    {
        emit packInstalled(true, QString());
    }
    else
    {
        emit packInstalled(false, "Ошибка установки: " + errorString);
    }
}

//------------------------------------------------------------------------------
// Очистка БД от мусора
//------------------------------------------------------------------------------
void McuBrowserViewModel::optimizeDatabase()
{
    emit dbOptimizeStarted();

    QtConcurrent::run([this]()
    {
        DBGarbageCollector collector;

        // Соединяем сигналы (Qt::DirectConnection, т.к. мы в одном потоке)
        QObject::connect(&collector, &DBGarbageCollector::errorOccured,
                        [this](const QString& e) {
            emit dbOptimizeError(e);
        });

        QObject::connect(&collector, &DBGarbageCollector::eventOccured,
                        [this](const QString& e) {
            emit dbLogMessage(e);
        });

        collector.deleteUnnecessaryTables();
        collector.deleteObsoleteData();

        QMetaObject::invokeMethod(this, "dbOptimizeFinished", Qt::QueuedConnection);
    });
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
    node.hierarchyNode = &m_pack.constVendor(vendor);

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
    node.hierarchyNode = &m_pack.constVendor(vendor).constFamily(family);

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
    node.hierarchyNode = &m_pack.constVendor(vendor).constFamily(family).constSeries(series);

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
    node.hierarchyNode = &m_pack.
                         constVendor(vendor).
                         constFamily(family).
                         constSeries(series).
                         constMcu(mcu);

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
// Построение дерева компонентов
//------------------------------------------------------------------------------
void McuBrowserViewModel::buildComponentTree()
{
    m_componentTree.clear();

    for(auto& component : m_pack.coComponentMap())
    {
#if HIDE_UNRELATED_COMPONENTS
        if(!component.hasChildren() && !component.hasParents())
            continue;
#endif

#if COMPONENT_BOTTOM_UP_HIERACHY
        if(!component.hasChildren())
#else
        if(!component.hasParents())
#endif
        {
            m_componentTree.append(buildComponentNode(component));
        }
    }
}

//------------------------------------------------------------------------------
// Создание узла компонента
//------------------------------------------------------------------------------
ComponentNode McuBrowserViewModel::buildComponentNode(const Component &component,
                                                      ComponentNode* parent)
{
    ComponentNode node;
    node.name = component.getName();
    node.description = component.getDescription();
    node.level = parent ? parent->level + 1 : 0;
    node.hierarchyNode = &component;

    if(node.level < 5)
    {
#if COMPONENT_BOTTOM_UP_HIERACHY
        foreach(auto parentComponent, component.getParents())
        {
            node.children.append(buildComponentNode(*parentComponent, &node));
        }
#else
        foreach(auto child, component.getChildren())
        {
            node.children.append(buildComponentNode(*child, &node));
        }
#endif
    }
    else
    {
        ComponentNode pseudoNode;
        pseudoNode.name = "...";
        pseudoNode.description = "...";
        pseudoNode.level = node.level + 1;
        node.children.append(pseudoNode);
    }

    return node;
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

//------------------------------------------------------------------------------
// Уникальный идентификатор производителя
//------------------------------------------------------------------------------
QString McuBrowserViewModel::vendorId() const
{
    qint32 id = m_pack.constVendor(m_selectedNode.getVendorName()).getUniqueId();
    return id == -1 ? QString() : QString::number(id);
}

//------------------------------------------------------------------------------
// Уникальный идентификатор семейства
//------------------------------------------------------------------------------
QString McuBrowserViewModel::familyId() const
{
    if(m_selectedNode.isValid() && !m_selectedNode.isVendor())
    {
        qint32 id = m_pack.
                    constVendor(m_selectedNode.getVendorName()).
                    constFamily(m_selectedNode.getFamilyName()).
                    getUniqueId();
        return id == -1 ? QString() : QString::number(id);
    }

    return QString();
}

//------------------------------------------------------------------------------
// Уникальный ID серии
//------------------------------------------------------------------------------
QString McuBrowserViewModel::seriesId() const
{
    if(m_selectedNode.isSeries() || m_selectedNode.isMcu())
    {
        qint32 id = m_pack.
                    constVendor(m_selectedNode.getVendorName()).
                    constFamily(m_selectedNode.getFamilyName()).
                    constSeries(m_selectedNode.getSeriesName()).
                    getUniqueId();
        return id == -1 ? QString() : QString::number(id);
    }

    return QString();
}

//------------------------------------------------------------------------------
// Уникальный ID устройства
//------------------------------------------------------------------------------
QString McuBrowserViewModel::mcuId() const
{
    if(m_selectedNode.isMcu())
    {
        qint32 id = m_pack.
                    constVendor(m_selectedNode.getVendorName()).
                    constFamily(m_selectedNode.getFamilyName()).
                    constSeries(m_selectedNode.getSeriesName()).
                    constMcu(m_selectedNode.getMcuName()).
                    getUniqueId();
        return id == -1 ? QString() : QString::number(id);
    }

    return QString();
}

//------------------------------------------------------------------------------
// Возвращает путь узла в формате vendor/family/series/mcu
//------------------------------------------------------------------------------
QString McuBrowserViewModel::devNodePath() const
{
    return m_selectedNode.hierarchyNode->getPath();
}

//------------------------------------------------------------------------------
// Уникальный ID компонента
//------------------------------------------------------------------------------
QString McuBrowserViewModel::componentId() const
{
    qint32 id = m_selectedComponentNode.hierarchyNode->getUniqueId();
    return id == -1 ? QString() : QString::number(id);
}
