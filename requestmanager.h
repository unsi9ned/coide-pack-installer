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

    QList<Manufacturer> manufacturers;
    QList<DebugAlgorithm> debugAlgList;
    QList<ProgAlgorithm> flashAlgList;

    QList<DebugAlgorithm> newDebugAlgList;
    QList<ProgAlgorithm> newFlashAlgList;

    static RequestManager* _m_instance;

    explicit RequestManager();
    ~RequestManager();

public:

    RequestManager(const RequestManager&) = delete;
    RequestManager& operator=(const RequestManager&) = delete;

    static RequestManager* instance();

    int getManufacturerCount(){return this->manufacturers.length();}

    int getDebugAlgCount(){return this->debugAlgList.length();}
    void addDebugAlgList(QList<DebugAlgorithm> l){this->debugAlgList.append(l);}
    void addDebugAlg(DebugAlgorithm a){this->debugAlgList.append(a);}
    DebugAlgorithm getDebugAlg(int i);

    DebugAlgorithm getDebugAlg(QString name);

    int getFlashAlgCount(){return this->flashAlgList.length();}
    void addFlashAlgList(QList<ProgAlgorithm> l){this->flashAlgList.append(l);}
    void addFlashAlg(ProgAlgorithm a){this->flashAlgList.append(a);}
    ProgAlgorithm getFlashAlg(int i);

    ProgAlgorithm getFlashAlg(QString name);


    Manufacturer getManufacturer(int i);

    int getManufacturerId(int i);

    Manufacturer getManufacturer(QString name);

    void addManufacturer(Manufacturer m){this->manufacturers.append(m);}
    void addManufacturerList(QList<Manufacturer> list);

    QList<Manufacturer> * getManufacturList();

    bool createManufacturer(QString m);
    bool createFamily(Manufacturer manufacturer, QString f);
    bool createSerie(Manufacturer man,
                     Family fam,
                     QString s);
    bool createMcu(Manufacturer man,
                   Family fam,
                   Series ser,
                   QString m);

    bool createDebugAlgorithm(QString name);
    bool createFlashAlgorithm(QString name);

    bool removeMcu(Mcu mcu);
    bool removeSerie(Series serie);
    bool removeFamily(Family family);
    bool removeManufacturer(Manufacturer manufacturer);
    bool removeDebugAlgorithm(DebugAlgorithm da);
    bool removeFlashAlgorithm(ProgAlgorithm fa);

    bool updateMcuInfo(Mcu mcu);


    void searchNewDebugAlgorithm();

    int getNewDebugAlgCount(){return this->newDebugAlgList.length();}

    DebugAlgorithm getNewDebugAlg(int i);

    void searchNewFlashAlgorithm();

    int getNewFlashAlgCount(){return this->newFlashAlgList.length();}

    ProgAlgorithm getNewFlashAlg(int i);


    bool fixVendor(QString& errorString);


private:

    bool fixVendorIDs(QString& errorString);
    bool fixVendorNames(QString &errorString);
    bool fixVendorId(QString &errorString);
    bool updateVendorName(const Manufacturer manufacturer, QString& errorString);
    bool updateFamilyTable(const Family& family, int vendorId, QString& errorString);
    bool fixBoardVendorIDs(QString& errorString);
    bool fixFamilyVendorIDs(QString& errorString);

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
    void infoReady();
    void algInfoReady();

public slots:

    void loadDataFromDb();
    void loadAlgorithmFromDb();
};

#endif // MCUINFO_H
