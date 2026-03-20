#ifndef MCUBROWSERVIEWMODEL_H
#define MCUBROWSERVIEWMODEL_H

#include <functional>
#include <QObject>
#include "models/pack/packdescription.h"
#include "models/pack/packmanager.h"
#include "models/mcu/devicehierarchynode.h"

//------------------------------------------------------------------------------
// Структура для представления узла дерева
//------------------------------------------------------------------------------
struct DeviceNode
{
    enum Type
    {
        InvalidType,
        VendorType,
        FamilyType,
        SeriesType,
        McuType
    };

    Type type;
    QString name;
    QString displayName;
    QList<DeviceNode> children;
    const DeviceHierarchyNode* hierarchyNode;

    // Для навигации
    QString vendorName;
    QString familyName;
    QString seriesName;

    DeviceNode() : type(InvalidType)
    {
        static DeviceHierarchyNode nullHierarchyNode;
        hierarchyNode = &nullHierarchyNode;
    }

    QString getVendorName() const { return type == VendorType ? name : vendorName; }
    QString getFamilyName() const { return type == FamilyType ? name : familyName; }
    QString getSeriesName() const { return type == SeriesType ? name : seriesName; }
    QString getMcuName()    const { return type == McuType ? name : QString(); }

    // Вспомогательные методы
    bool isVendor() const { return type == VendorType; }
    bool isFamily() const { return type == FamilyType; }
    bool isSeries() const { return type == SeriesType; }
    bool isMcu() const { return type == McuType; }
    bool isValid() const { return type == InvalidType || !name.isEmpty(); }
};

//------------------------------------------------------------------------------
// Структура для представления компонента в дереве
//------------------------------------------------------------------------------
struct ComponentNode
{
    int level;
    QString name;
    QString description;
    QList<ComponentNode> children;

    ComponentNode() : level(0){}

    bool hasChildren() const { return !children.isEmpty(); }
};

Q_DECLARE_METATYPE(DeviceNode)
Q_DECLARE_METATYPE(ComponentNode)

//------------------------------------------------------------------------------
// Контроллер устройств
//------------------------------------------------------------------------------
class McuBrowserViewModel : public QObject
{
    Q_OBJECT

private:
    PackDescription m_pack;
    PackManager m_packManager;

    QList<DeviceNode> m_deviceTree;
    DeviceNode m_selectedNode;

    // Для быстрого доступа
    QMap<QString, Mcu*> m_mcuCache;

    QList<ComponentNode> m_componentTree;

public:
    explicit McuBrowserViewModel(QObject *parent = 0);
    ~McuBrowserViewModel();

    // Загрузка данных
    bool loadPack();
    bool loadPack(const QString& path);
    void loadPackAsync();
    void loadPackAsync(const QString& path);
    void installCurrentPack();
    void optimizeDatabase();

    // Доступ к дереву
    const QList<DeviceNode>& deviceTree() const { return m_deviceTree; }
    const QList<ComponentNode>& componentTree() const { return m_componentTree; }

    // Навигация (без сигналов!)
    void selectNode(const DeviceNode& node);

    // Сохранение информации о выбранном устройстве
    void saveSelection();

    // Текущее состояние
    DeviceNode selectedNode() const { return m_selectedNode; }
    QString selectedVendor() const;
    QString selectedFamily() const;
    QString selectedSeries() const;
    QString selectedMcu() const;

    // Данные для отображения (UI сам запрашивает когда нужно)
    QString flashStart() const;
    QString flashSize() const;
    QString ramStart() const;
    QString ramSize() const;
    QString features() const;
    QString description() const;
    QString webPageUrl() const;
    QString datasheetUrl() const;
    QString svdLocalPath() const;

    QString debugAlgorithm() const;
    QStringList flashAlgorithms() const;
    QString defaultFlashAlgorithm() const;

    QString releaseVersion() const;

    QString vendorId() const;

    QString devNodePath() const;

private:
    void buildDeviceTree();
    DeviceNode buildVendorNode(const QString& vendor);
    DeviceNode buildFamilyNode(const QString& vendor, const QString& family);
    DeviceNode buildSeriesNode(const QString& vendor, const QString& family, const QString& series);
    DeviceNode buildMcuNode(const QString& vendor, const QString& family,
                            const QString& series, const QString& mcu);
    void restoreSelection();
    DeviceNode* findNode(const QString &vendor,
                         const QString &family = QString(),
                         const QString &series = QString(),
                         const QString &mcu = QString());

    void buildComponentTree();
    ComponentNode buildComponentNode(const Component& component, ComponentNode * parent = nullptr);
signals:
    void loadStarted();
    void loadResult(bool success, QString errorString);
    void packLoaded(bool success);

    void installStarted();
    void installResult(bool success, QString errorString);
    void installLogMessage(const QString& message);
    void packInstalled(bool success, QString errorString);

    void dbOptimizeStarted();
    void dbOptimizeFinished();
    void dbOptimizeError(const QString& error);
    void dbLogMessage(const QString& message);

private slots:
    void onLoadResult(bool success, QString errorString);
    void onInstallResult(bool success, QString errorString);
};

#endif // MCUBROWSERVIEWMODEL_H
