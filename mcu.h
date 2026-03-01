#ifndef MCU_H
#define MCU_H

#include <QObject>
#include <QString>
#include <QByteArray>
#include <QUuid>
#include <QDateTime>
#include <QList>

#include "debugalgorithm.h"
#include "progalgorithm.h"
#include "memory.h"
#include "devicefeature.h"
#include "featurecontainer.h"
#include "algorithmcontainer.h"
#include "component.h"

class Mcu : public FeatureContainer, public AlgorithmContainer
{

private:

    int id;
    int seriesId;
    int userId;
    int debugAlgorithmId;
    QString name;
    QString description;
    QString keyParameter;
    QString webPageURL;
    QString datasheetURL;
    QString memInfo;
    QString micro;
    QString advertising;
    QString price;
    QString timeuuid;
    int hits;
    QString _svd;

    DebugAlgorithm debugAlgorithm;
    ProgAlgorithm flashAlgorithm;

    QMap<QString, Memory> _memoryMap;
    QStringList _definedSymbols;
    QStringList _compileHeaders;

public:
    Mcu();
    Mcu(int id,
        int seriesId,
        int debugAlgorithmId,
        int userId = CoUser::USER_COOCOX);

    int getId() const {return id;}
    int getSeriesId() const {return seriesId;}
    int getUserId() const {return userId;}
    int getDebugAlgorithmId() const {return debugAlgorithmId;}
    QString getName() const {return name;}
    QString getDescription() const {return description;}
    QString getKeyParameter() const {return keyParameter;}
    QString getWebPageURL() const {return webPageURL;}
    QString coWebPageUrl() const {return "[\"" + webPageURL + "\"]";}
    QString getDatasheetURL() const {return datasheetURL;}
    QString coDatasheetURL() const {return "[\"" + datasheetURL + "\"]";}
    QString getMemInfo() const {return memInfo;}
    QString coMemInfo();
    QString getMicro() const {return micro;}
    QString getAdvertising() const {return advertising;}
    QString getPrice() const {return price;}
    QString getTimeuuid() const {return timeuuid;}
    int getHits() const {return hits;}
    QString svdLocalPath() const { return _svd;}
    DebugAlgorithm& getDebugAlgorithm() {return this->debugAlgorithm;}

    void setId(int id){this->id = id;}
    void setSeriesId(int id){this->seriesId = id;}
    void setUserId(int id){this->userId = id;}
    void setDebugAlgorithmId(int id);
    Mcu& setName(QString s);
    void setDescription(QString s){this->description = s;}
    void setKeyParameter(const QString s){this->keyParameter = s;}
    void setWebPageURL(const QString url){this->webPageURL = url;}
    void setDatasheetURL(const QString& url){this->datasheetURL = url;}
    void setMemInfo(const QString& info){this->memInfo = info;}
    void setMicro(QString s){this->micro = s;}
    void setAdvertising(QString s){this->advertising = s;}
    void setPrice(QString s){this->price = s;}
    void setTimeuuid(QString s){this->timeuuid = s;}
    void setHits(int hits){this->hits = hits;}
    void setSvd(const QString& path){this->_svd = path;}
    void setDebugAlgorithm(DebugAlgorithm da){this->debugAlgorithm = da;}
    void setFlashAlgorithm(ProgAlgorithm da){this->flashAlgorithm = da;}

    Mcu& setCoreDebugAlgorithm(const QString& coreName);

    bool isValid(QString* errorString = nullptr);
    bool isNull();

    QMap<QString, Memory>& memoryRegions();

    Memory& memory(const QString& name);
    Memory& addMemoryRegion(const QString name);
    Memory* getCodeMemory();
    Memory* getDataMemory();

    static QString generateTimeUUID();

    void addDefSymbol(const QString& symbol) {this->_definedSymbols.append(symbol);}
    QStringList definedSymbols() {return this->_definedSymbols;}
    QString defSym2coMicro();
    QString coDescription();

    void addCompileHeader(const QString& header){this->_compileHeaders.append(header);}
    QStringList compileHeaders(){return this->_compileHeaders;}

private:
    bool isValid(QString& errorString);
    Memory& createMemoryRegion(const QString regionName);
};

#endif // MCU_H
