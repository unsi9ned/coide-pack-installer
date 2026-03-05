#ifndef MAINVIEWMODEL_H
#define MAINVIEWMODEL_H

#include <QObject>
#include "models/pack/packdescription.h"
#include "models/pack/packmanager.h"
#include "models/pack/pdscparser.h"

class MainViewModel : public QObject
{
    Q_OBJECT

private:
    PackDescription m_pack;
    PackManager     m_packManager;

    // Состояние навигации
    QStringList m_vendors;
    QStringList m_families;
    QStringList m_series;
    QStringList m_mcus;

    QString m_currentVendor;
    QString m_currentFamily;
    QString m_currentSeries;
    QString m_currentMcu;

    // Кэшированные данные текущего MCU
    Mcu* m_currentMcuObj = nullptr;
    Memory* m_codeMemory = nullptr;
    Memory* m_dataMemory = nullptr;

public:
    explicit MainViewModel(QObject *parent = 0);

    // Геттеры для свойств
    QStringList vendors() const { return m_vendors; }
    QStringList families() const { return m_families; }
    QStringList series() const { return m_series; }
    QStringList mcus() const { return m_mcus; }

    QString flashStart() const;
    QString flashSize() const;
    QString ramStart() const;
    QString ramSize() const;
    QString features() const;
    QString description() const;

public slots:

    void loadDeviceFamilyPack();

    // Команды для UI
    void selectVendor(int index);
    void selectFamily(int index);
    void selectSeries(int index);
    void selectMcu(int index);

private:
    void loadDeviceFamilyPack(const QString& path);

    void updateFamilies();
    void updateSeries();
    void updateMcus();
    void updateMcuDetails();

    void restoreSelection();

private slots:

signals:
    // Сигналы для UI
    void vendorsChanged();
    void familiesChanged();
    void seriesChanged();
    void mcusChanged();
    void mcuChanged();

    void loadStarted();
    void loadFinished();
    void loadFailed(const QString& error);

    void statusMessage(const QString& message, int timeout = 3000);

};

#endif // MAINVIEWMODEL_H
