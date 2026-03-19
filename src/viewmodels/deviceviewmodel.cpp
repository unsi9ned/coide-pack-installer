#include <QtConcurrent/QtConcurrent>
#include "deviceviewmodel.h"
#include "services/settings.h"

//------------------------------------------------------------------------------
// Конструктор
//------------------------------------------------------------------------------
DeviceViewModel::DeviceViewModel(QObject *parent):
    QObject(parent),
    m_mcuDetailsViewModel(new McuDetailsViewModel(this))
{
    m_mcuDetailsViewModel->setPack(&m_pack);

    connect(m_mcuDetailsViewModel, &McuDetailsViewModel::mcuChanged, this, &DeviceViewModel::mcuChanged);
    connect(m_mcuDetailsViewModel, &McuDetailsViewModel::loadDataStarted, this, &DeviceViewModel::mcuLoadDetailsStarted);
    connect(m_mcuDetailsViewModel, &McuDetailsViewModel::loadDataFinished, this, &DeviceViewModel::mcuLoadDetailsFinished);
    connect(m_mcuDetailsViewModel, &McuDetailsViewModel::loadDataFailed, this, &DeviceViewModel::mcuLoadDetailsFailed);
}

//------------------------------------------------------------------------------
// Версия пакета
//------------------------------------------------------------------------------
QString DeviceViewModel::releaseVersion() const
{
    return m_pack.release();
}

//------------------------------------------------------------------------------
// Загрузка пакета
//------------------------------------------------------------------------------
void DeviceViewModel::loadDeviceFamilyPack()
{
    loadDeviceFamilyPack(Settings::instance()->lastLoadedPack());
}

//------------------------------------------------------------------------------
// Загрузка пакета
//------------------------------------------------------------------------------
void DeviceViewModel::loadDeviceFamilyPack(const QString &path)
{
    if (path.isEmpty())
    {
        emit loadError("Путь не указан");
        return;
    }

    emit loadStarted();
    emit logMessage("Загрузка пакета: " + path);

    // Сохраняем путь
    Settings::instance()->saveLastLoadedPack(path);

#if 0
    // Асинхронная загрузка
    QtConcurrent::run([this, path]()
    {
        try
        {
            m_pack.clear();
            m_pack.setPathToArchive(path);
            m_packManager.readPackDescription(m_pack);

            buildDeviceTree();

            QMetaObject::invokeMethod(this, "onLoadSuccess", Qt::QueuedConnection);
        }
        catch (const std::exception& e)
        {
            QMetaObject::invokeMethod(this, "onLoadError",
                                      Qt::QueuedConnection,
                                      Q_ARG(QString, QString(e.what())));
        }
    });
#else
    m_pack.clear();
    m_pack.setPathToArchive(path);
    m_packManager.readPackDescription(m_pack);

    buildDeviceTree();
    onLoadSuccess();
#endif
}

//------------------------------------------------------------------------------
// Обработчик успешной загрузки пакета
//------------------------------------------------------------------------------
void DeviceViewModel::onLoadSuccess()
{
    emit deviceTreeChanged();

    QString vendor = Settings::instance()->selectedVendor();

    if (!vendor.isEmpty())
    {
        selectNodeByPath(vendor, Settings::instance()->selectedCore(),
                         Settings::instance()->selectedSeries(),
                         Settings::instance()->selectedMcu());
    }

    emit loadFinished(true, "Пакет загружен");
    emit logMessage("Загрузка завершена");
}

//------------------------------------------------------------------------------
// Обработчик ошибки загрузки пакета
//------------------------------------------------------------------------------
void DeviceViewModel::onLoadError(const QString& error)
{
    emit loadError(error);
    emit loadFinished(false, "Ошибка загрузки");
}

//------------------------------------------------------------------------------
// Построение дерева устройств
//------------------------------------------------------------------------------
void DeviceViewModel::buildDeviceTree()
{
    m_deviceTree.clear();

    // Получаем всех производителей
    QStringList vendors = m_pack.vendors().keys();

    for (const QString &vendor : vendors)
    {
        DeviceTreeNode vendorNode;
        vendorNode.type = DeviceTreeNode::VendorType;
        vendorNode.name = vendor;
        vendorNode.displayName = vendor;

        // Получаем семейства для производителя
        QStringList families = m_pack.vendor(vendor).families().keys();

        for (const QString &family : families)
        {
            DeviceTreeNode familyNode;
            familyNode.type = DeviceTreeNode::FamilyType;
            familyNode.name = family;
            familyNode.displayName = family;

            // Получаем серии для семейства
            QStringList series = m_pack.vendor(vendor).family(family).seriesMap().keys();

            for (const QString &serie : series)
            {
                DeviceTreeNode seriesNode;
                seriesNode.type = DeviceTreeNode::SeriesType;
                seriesNode.name = serie;
                seriesNode.displayName = serie;

                // Получаем MCU для серии
                QStringList mcus = m_pack.vendor(vendor).family(family).series(serie).mcuMap().keys();

                for (const QString &mcu : mcus)
                {
                    DeviceTreeNode mcuNode;
                    mcuNode.type = DeviceTreeNode::McuType;
                    mcuNode.name = mcu;
                    mcuNode.displayName = mcu;

                    // Можно добавить дополнительную информацию
                    Mcu *mcuObj = &m_pack.
                                  vendor(vendor).
                                  family(family).
                                  series(serie).
                                  mcu(mcu);

                    if (mcuObj)
                    {
                        mcuNode.data = QVariant::fromValue(mcuObj->getName());
                    }

                    seriesNode.children.append(mcuNode);
                }

                familyNode.children.append(seriesNode);
            }

            vendorNode.children.append(familyNode);
        }

        m_deviceTree.append(vendorNode);
    }
}

//------------------------------------------------------------------------------
// Поиск узла
//------------------------------------------------------------------------------
DeviceTreeNode*DeviceViewModel::findNode(const QString& vendor,
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
// Выбор узла дерева
//------------------------------------------------------------------------------
void DeviceViewModel::selectNodeByPath(const QString& vendor,
                                       const QString& family,
                                       const QString& series,
                                       const QString& mcu)
{
    m_currentVendor = vendor;
    m_currentFamily = family;
    m_currentSeries = series;
    m_currentMcu = mcu;

    // Сохраняем в настройках
    Settings::instance()->saveSelectedVendor(vendor);
    Settings::instance()->saveSelectedCore(family);
    Settings::instance()->saveSelectedSeries(series);
    Settings::instance()->saveSelectedMcu(mcu);

    // Находим узел
    DeviceTreeNode* node = findNode(vendor, family, series, mcu);

    if (node)
    {
        m_selectedNode = *node;
        emit nodeSelected();

        if (node->type == DeviceTreeNode::McuType)
        {
            m_mcuDetailsViewModel->selectMcu(vendor, family, series, mcu);
            emit mcuSelected();
        }
    }
}

void DeviceViewModel::installCurrentPack()
{

}

void DeviceViewModel::optimizeDatabase()
{

}
