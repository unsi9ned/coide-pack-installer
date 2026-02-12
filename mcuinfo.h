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

class McuInfo : public QObject
{
    Q_OBJECT

private:

    QString idePath;
    DataBase * db;
    QList<Manufacturer> manufacturers;
    QList<DebugAlgorithm> debugAlgList;
    QList<ProgAlgorithm> flashAlgList;

    QList<DebugAlgorithm> newDebugAlgList;
    QList<ProgAlgorithm> newFlashAlgList;

public:
    explicit McuInfo(DataBase * db, QObject *parent = 0);
    ~McuInfo();

    int getManufacturerCount(){return this->manufacturers.length();}

    int getDebugAlgCount(){return this->debugAlgList.length();}
    void addDebugAlgList(QList<DebugAlgorithm> l){this->debugAlgList.append(l);}
    void addDebugAlg(DebugAlgorithm a){this->debugAlgList.append(a);}
    DebugAlgorithm getDebugAlg(int i)
    {
        if(i >= 0 && i < debugAlgList.length())
        {
            return this->debugAlgList.at(i);
        }
        return DebugAlgorithm();
    }

    DebugAlgorithm getDebugAlg(QString name)
    {
        DebugAlgorithm da;

        foreach(da, debugAlgList)
        {
            if(da.name().trimmed().toLower() == name.trimmed().toLower())
            {
                break;
            }
        }

        return da;
    }

    int getFlashAlgCount(){return this->flashAlgList.length();}
    void addFlashAlgList(QList<ProgAlgorithm> l){this->flashAlgList.append(l);}
    void addFlashAlg(ProgAlgorithm a){this->flashAlgList.append(a);}
    ProgAlgorithm getFlashAlg(int i)
    {
        if(i >= 0 && i < flashAlgList.length())
        {
            return this->flashAlgList.at(i);
        }
        return ProgAlgorithm();
    }

    ProgAlgorithm getFlashAlg(QString name)
    {
        ProgAlgorithm fa;

        foreach(fa, flashAlgList)
        {
            if(fa.name().trimmed().toLower() == name.trimmed().toLower())
            {
                break;
            }
        }

        return fa;
    }


    Manufacturer getManufacturer(int i)
    {
        return (i < manufacturers.length() && i >= 0) ? manufacturers.at(i) : Manufacturer();
    }

    int getManufacturerId(int i)
    {
        if(i < 0 || i >= manufacturers.length())
        {
            return -1;
        }
        Manufacturer man = manufacturers.at(i);
        return man.getId();
    }

    Manufacturer getManufacturer(QString name)
    {
        Manufacturer m;

        for(int i = 0; i < manufacturers.length(); i++)
        {
            m = manufacturers.at(i);

            if(m.getName() == name)
            {
                break;
            }
        }
        return m;
    }

    void addManufacturer(Manufacturer m){this->manufacturers.append(m);}
    void addManufacturerList(QList<Manufacturer> list)
    {
        this->manufacturers.append(list);
    }

    QList<Manufacturer> * getManufacturList()
    {
        return &this->manufacturers;
    }

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

    DebugAlgorithm getNewDebugAlg(int i)
    {
        if(i >= 0 && i < newDebugAlgList.length())
        {
            return this->newDebugAlgList.at(i);
        }
        return DebugAlgorithm();
    }

    void searchNewFlashAlgorithm();

    int getNewFlashAlgCount(){return this->newFlashAlgList.length();}

    ProgAlgorithm getNewFlashAlg(int i)
    {
        if(i >= 0 && i < newFlashAlgList.length())
        {
            return this->newFlashAlgList.at(i);
        }
        return ProgAlgorithm();
    }

    void changeIdePath(QString idePath)
    {
        Logger::instance()->addEvent(QString("The path to the IDE is set '%1'").arg(idePath));

        this->idePath = idePath;
        db->setDbPath(Paths::instance()->coIdeDatabaseFile());
        Paths::instance()->setCoIdeDir(idePath);
    }


    QString getIdePath() const;

private:

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
