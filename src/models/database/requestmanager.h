#ifndef MCUINFO_H
#define MCUINFO_H

#include <QObject>
#include <QList>
#include <QDebug>
#include <QDir>
#include <QFile>

#include "models/mcu/mcu.h"
#include "models/mcu/series.h"
#include "models/mcu/family.h"
#include "models/mcu/manufacturer.h"
#include "models/components/component.h"
#include "database.h"
#include "services/logger.h"
#include "services/paths.h"
#include "componentsinfo.h"

class RequestManager : public ComponentsInfo
{
    Q_OBJECT

private:

    static RequestManager* _m_instance;
    explicit RequestManager();
    ~RequestManager();

    // Запрет копирования
    RequestManager(const RequestManager&) = delete;
    RequestManager& operator=(const RequestManager&) = delete;

public:

    static RequestManager* instance();

    void loadDataFromDb(const QString& vendorName, Manufacturer& vendor);
    void loadDataFromDb(const QStringList& vendors, QMap<QString, Manufacturer>& vendorMap);
    void loadDataFromDb(QMap<QString, Manufacturer>& vendorMap);

    bool createManufacturer(const Manufacturer& vendor);
    bool createFamily(Family& family);
    bool createSeries(Series& series);
    bool createMcu(Mcu& device);

    bool createDebugAlgorithm(DebugAlgorithm& algo);
    bool createFlashAlgorithm(QString name);

    bool removeMcu(Mcu mcu);
    bool removeSerie(Series serie);
    bool removeFamily(Family family);
    bool removeManufacturer(Manufacturer manufacturer);
    bool removeDebugAlgorithm(DebugAlgorithm da);
    bool removeFlashAlgorithm(ProgAlgorithm fa);

    bool updateMcuInfo(Mcu mcu);

    bool fixVendorIDs(QString& errorString);

private:

    bool fixManufacturerTable(QString& errorString);
    bool fixFamilyTable(QString& errorString);
    bool fixBoardManufacturerTable(QString& errorString);

    bool updateVendorName(const Manufacturer manufacturer, QString& errorString);
    bool updateManufacturerTable(int id, int newId, const QString& newName, QString& errorString);
    bool updateFamilyTable(const Family& family, int vendorId, QString& errorString);
    bool updateBoardManufacturerTable(int boardId, int vendorId, QString& errorString);

    Manufacturer requestManufacturer(int id, QString name = QString());
    QList<Manufacturer> requestManufacturerList();
    QList<Family> requestFamilyList(int vendorId);
    QList<Family> requestFamilyList();
    QList<Series> requestSeriesList(int familyId);
    QList<Mcu> requestMcuList(int seriesId);
    DebugAlgorithm requestDebugAlgorithm(int id);
    DebugAlgorithm requestDebugAlgorithm(const QString& name);
    QList<DebugAlgorithm> requestDebugAlgorithmList();
    ProgAlgorithm requestFlashAlgorithm(int id);
    QList<ProgAlgorithm> requestFlashAlgorithmList();
    ProgAlgorithm getMcuFlashAlgorithm(int mcuId);

signals:

    void errorOccured(QString e);

public slots:

};

#endif // MCUINFO_H
