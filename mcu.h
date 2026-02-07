#ifndef MCU_H
#define MCU_H

#include <QObject>
#include <QString>
#include <QByteArray>

#include "debugalgorithm.h"
#include "flashalgorithm.h"
#include "memory.h"

#define USER_ID_COOCOX  (2)

class Mcu
{

private:

    int id;
    int seriesId;
    int userId;
    int debugAlgorithmId;
    QString name;
    QString description;
    QByteArray keyParameter;
    QByteArray webPageURL;
    QByteArray datasheetURL;
    QByteArray memInfo;
    QString micro;
    QString advertising;
    QString price;
    QString timeuuid;
    int hits;

    DebugAlgorithm debugAlgorithm;
    FlashAlgorithm flashAlgorithm;

    QMap<QString, Memory> _memoryMap;

public:
    Mcu()
    {
        this->id = -1;
    }

    Mcu(int id,
        int seriesId,
        int debugAlgorithmId,
        int userId = USER_ID_COOCOX)
    {
        this->id = id;
        this->seriesId = seriesId;
        this->debugAlgorithmId = debugAlgorithmId;
        this->userId = userId;
    }

    int getId() const {return id;}
    int getSeriesId() const {return seriesId;}
    int getUserId() const {return userId;}
    int getDebugAlgorithmId() const {return debugAlgorithmId;}
    QString getName() const {return name;}
    QString getDescription() const {return description;}
    QByteArray getKeyParameter() const {return keyParameter;}
    QByteArray getWebPageURL() const {return webPageURL;}
    QByteArray getDatasheetURL() const {return datasheetURL;}
    QByteArray getMemInfo() const {return memInfo;}
    QString getMicro() const {return micro;}
    QString getAdvertising() const {return advertising;}
    QString getPrice() const {return price;}
    QString getTimeuuid() const {return timeuuid;}
    int getHits() const {return hits;}
    DebugAlgorithm getDebugAlgorithm() const {return this->debugAlgorithm;}
    FlashAlgorithm getFlashAlgorithm() const {return this->flashAlgorithm;}

    void setId(int id){this->id = id;}
    void setSeriesId(int id){this->seriesId = id;}
    void setUserId(int id){this->userId = id;}
    void setDebugAlgorithmId(int id){this->debugAlgorithmId = id;}
    Mcu& setName(QString s)
    {
        this->name = s;
        return *this;
    }
    void setDescription(QString s){this->description = s;}
    void setKeyParameter(QByteArray s){this->keyParameter = s;}
    void setWebPageURL(QByteArray s){this->webPageURL = s;}
    void setDatasheetURL(QByteArray s){this->datasheetURL = s;}
    void setMemInfo(QByteArray i){this->memInfo = i;}
    void setMicro(QString s){this->micro = s;}
    void setAdvertising(QString s){this->advertising = s;}
    void setPrice(QString s){this->price = s;}
    void setTimeuuid(QString s){this->timeuuid = s;}
    void setHits(int hits){this->hits = hits;}
    void setDebugAlgorithm(DebugAlgorithm da){this->debugAlgorithm = da;}
    void setFlashAlgorithm(FlashAlgorithm da){this->flashAlgorithm = da;}

    bool isValid()
    {
        if(this->id <= 0 ||
           this->seriesId <= 0 ||
           this->name.isEmpty() ||
           this->memInfo.isEmpty())
        {
            return false;
        }
        else
            return true;
    }

    QMap<QString, Memory>& memoryRegions()
    {
        return this->_memoryMap;
    }

    Memory& memory(const QString& name)
    {
        if(_memoryMap.contains(name))
            return _memoryMap[name];
        else
            return createMemoryRegion(name);
    }

    Memory& addMemoryRegion(const QString name)
    {
        return memory(name);
    }

    Memory* getCodeMemory()
    {
        for(auto it = _memoryMap.begin(); it != _memoryMap.end(); ++it)
        {
            if(it.value().isCodeMemory())
                return &_memoryMap[it.key()];
        }

        return nullptr;
    }

    Memory* getDataMemory()
    {
        for(auto it = _memoryMap.begin(); it != _memoryMap.end(); ++it)
        {
            if(it.value().isDataMemory())
                return &_memoryMap[it.key()];
        }

        return nullptr;
    }

private:
    Memory& createMemoryRegion(const QString regionName)
    {
        _memoryMap.insert(regionName, Memory());
        return _memoryMap[regionName].setName(regionName);
    }
};

#endif // MCU_H
