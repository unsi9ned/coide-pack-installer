#ifndef MCUINFO_H
#define MCUINFO_H

#include <QObject>
#include <QList>
#include <QDebug>
#include <QDir>
#include <QFile>

#include "mcu.h"
#include "series.h"
#include "family.h"
#include "manufacturer.h"
#include "database.h"
#include "logger.h"
#include "paths.h"
#include "component.h"

class RequestManager : public QObject
{
    Q_OBJECT

private:

    static RequestManager* _m_instance;
    explicit RequestManager();
    ~RequestManager();

public:

    RequestManager(const RequestManager&) = delete;
    RequestManager& operator=(const RequestManager&) = delete;

    static RequestManager* instance();

    void loadDataFromDb(QMap<QString, Manufacturer>& vendorMap);

    bool createManufacturer(QString m);
    bool createFamily(Manufacturer manufacturer, QString f);
    bool createSerie(Manufacturer man, Family fam,QString s);
    bool createMcu(Manufacturer man, Family fam, Series ser, QString m);

    bool createDebugAlgorithm(QString name);
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

    QList<Manufacturer> requestManufacturerList();
    QList<Family> requestFamilyList(Manufacturer man);
    QList<Family> requestFamilyList();
    QList<Series> requestSeriesList(Family fam);
    QList<Mcu> requestMcuList(Series serie);
    DebugAlgorithm requestDebugAlgorithm(int id);
    QList<DebugAlgorithm> requestDebugAlgorithmList();
    ProgAlgorithm requestFlashAlgorithm(int id);
    QList<ProgAlgorithm> requestFlashAlgorithmList();
    ProgAlgorithm getMcuFlashAlgorithm(int mcuId);

signals:

    void errorOccured(QString e);

public slots:

};

#endif // MCUINFO_H
