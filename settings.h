#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>
#include <QSettings>
#include <QCoreApplication>
#include <QString>
#include <QMutex>
#include <QRect>

class Settings : public QObject
{
    Q_OBJECT

private:
    static constexpr const char* MAIN_SETTINGS_GRP = "Common";
    static constexpr const char* MAIN_PARAMETER_COIDE = "CoIDE";
    static constexpr const char* MAIN_PARAMETER_PACK = "Pack";

    static constexpr const char* DEVICE_SETTINGS_GRP = "Device";
    static constexpr const char* DEVICE_PARAMETER_VENDOR = "Vendor";
    static constexpr const char* DEVICE_PARAMETER_CORE = "Core";
    static constexpr const char* DEVICE_PARAMETER_SERIES = "Series";
    static constexpr const char* DEVICE_PARAMETER_MCU = "Mcu";

    static QMutex settingsAccessMutex;

    explicit Settings(QObject *parent = nullptr);
    ~Settings();

    static Settings* m_instance;

public:
    Settings(const Settings&) = delete;
    Settings& operator=(const Settings&) = delete;

    static Settings* instance();

    QString coIdePath();
    QString lastLoadedPack();
    QString selectedVendor();
    QString selectedCore();
    QString selectedSeries();
    QString selectedMcu();

    void saveCoIdePath(const QString& path);
    void saveLastLoadedPack(const QString& path);
    void saveSelectedVendor(const QString& vendor);
    void saveSelectedCore(const QString& core);
    void saveSelectedSeries(const QString& series);
    void saveSelectedMcu(const QString& mcu);

private:
    static void saveCustomParameter(const QString& group,
                                    const QString& key,
                                    QVariant value = QVariant());

    static QVariant getCustomParameter(const QString& group,
                                       const QString& key,
                                       QVariant defaultValue = QVariant());
signals:

public slots:
};

#endif // SETTINGS_H
