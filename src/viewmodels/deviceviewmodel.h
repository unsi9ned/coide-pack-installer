#ifndef DEVICEVIEWMODEL_H
#define DEVICEVIEWMODEL_H

#include <QList>
#include <QObject>
#include <QString>
#include <QVariant>

#include "models/pack/packdescription.h"
#include "models/pack/packmanager.h"
#include "mcudetailsviewmodel.h"

//------------------------------------------------------------------------------
// Структура для представления узла дерева
//------------------------------------------------------------------------------
struct DeviceTreeNode
{
    enum NodeType
    {
        VendorType,
        FamilyType,
        SeriesType,
        McuType
    };

    NodeType type;
    QString name;
    QString displayName; // для отображения с дополнительной инфой
    QVariant data; // для хранения ID или других данных
    QList<DeviceTreeNode> children;

    // Вспомогательные методы
    bool isVendor() const
    {
        return type == VendorType;
    }

    bool isFamily() const
    {
        return type == FamilyType;
    }

    bool isSeries() const
    {
        return type == SeriesType;
    }

    bool isMcu() const
    {
        return type == McuType;
    }
};

//------------------------------------------------------------------------------
// Контроллер устройств
//------------------------------------------------------------------------------
class DeviceViewModel: public QObject
{
    Q_OBJECT

private:
    PackDescription m_pack;
    PackManager m_packManager;

    QList<DeviceTreeNode> m_deviceTree;
    DeviceTreeNode m_selectedNode;

    QString m_currentVendor;
    QString m_currentFamily;
    QString m_currentSeries;
    QString m_currentMcu;

    McuDetailsViewModel* m_mcuDetailsViewModel;

public:
    explicit DeviceViewModel(QObject *parent = 0);

    QString currentVendor() const {return m_currentVendor;}
    QString currentFamily() const {return m_currentFamily;}
    QString currentSeries() const {return m_currentSeries;}
    QString currentMcu() const {return m_currentMcu;}

    McuDetailsViewModel* mcuDetails()
    {
        return m_mcuDetailsViewModel;
    }

    // Геттеры
    QList<DeviceTreeNode> deviceTree() const
    {
        return m_deviceTree;
    }

    // Текущий выбранный элемент
    DeviceTreeNode selectedNode() const
    {
        return m_selectedNode;
    }

    QString releaseVersion() const;

public slots:
    // Команды для UI
    void loadDeviceFamilyPack();
    void loadDeviceFamilyPack(const QString &path);
    void selectNodeByPath(const QString &vendor,
                          const QString &family = QString(),
                          const QString &series = QString(),
                          const QString &mcu = QString());

    void installCurrentPack();
    void optimizeDatabase();

private:
    void buildDeviceTree();
    DeviceTreeNode *findNode(const QString &vendor,
                             const QString &family = QString(),
                             const QString &series = QString(),
                             const QString &mcu = QString());

private slots:
    void onLoadSuccess();
    void onLoadError(const QString& error);

signals:
    void deviceTreeChanged(); // дерево обновлено
    void nodeSelected(); // выбран узел
    void mcuSelected(); // выбран MCU (специфичный сигнал)
    void mcuChanged();

    void mcuLoadDetailsStarted();
    void mcuLoadDetailsFinished();
    void mcuLoadDetailsFailed();

    // Сигналы для процесса загрузки
    void loadStarted();
    void loadFinished(bool success, const QString &message);
    void loadError(const QString &error);

    // Сигналы для установки
    void installStarted();
    void installFinished(bool success, const QString &message);
    void installError(const QString &error);

    // Сигналы для логов
    void logMessage(const QString &message);
    void errorMessage(const QString &error);
};

#endif // DEVICEVIEWMODEL_H
