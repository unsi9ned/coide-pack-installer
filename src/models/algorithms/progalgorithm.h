#ifndef FLASHALGORITHM_H
#define FLASHALGORITHM_H

#include <QObject>
#include <QString>
#include <QByteArray>
#include <QDateTime>

#include "common/constants.h"
#include "models/mcu/devicehierarchynode.h"

class ProgAlgorithm : public DeviceHierarchyNode
{

private:

    int      _coId;

    QString  _name;
    //QString  _installDir;
    QString  _description;
    QString  _timeUuid;
    uint32_t _start;
    int32_t  _size;
    uint32_t _RAMstart;
    int32_t  _RAMsize;
    bool     _isDefault;
    int      _documentId;
    QDateTime _createDate;
    QDateTime _updateDate;

    QString m_vendor;
    QString m_version;
    bool m_installed;

public:
    ProgAlgorithm(const QString& name = QString());

    void setCoId(int id);
    void setName(QString name);
    void setStart(const uint32_t &start);
    void setSize(const uint32_t &size);
    void setRAMstart(const uint32_t &RAMstart);
    void setRAMsize(const uint32_t &RAMsize);
    void setDefault(bool isDefault);
    void setDocumentId(int id);
    void setTimeUUID(const QString& uuid);
    void setCreateDate(const QString& dt);
    void setUpdateDate(const QString& dt);
    void setDescription(const QString& dscr);
    void parseCoName(const QString& coName);
    void setInstalled(bool state) { m_installed = state; }

    void setVendor(const QString& vendor) {m_vendor = vendor;}
    void setVersion(const QString& version) {m_version = version;}
    QString getVendor() const {return m_vendor;}
    QString getVersion() const {return m_version;}

    int coId() const;
    qint32 getCoMaxId() const {return ID_FLASH_ALGO_LAST; }
    QString name() const;
    uint32_t start() const;
    uint32_t size() const;
    uint32_t RAMstart() const;
    uint32_t RAMsize() const;
    bool isDefault() const;
    bool isNull();
    QString getPath() const;
    int documentId() const;
    QString timeUUID() const;
    QString creationDate(QString dtFormat = QString("yyyy-MM-dd HH:mm:ss.z")) const;
    QString updateDate(QString dtFormat = QString("yyyy-MM-dd HH:mm:ss.z")) const;
    QString description() const;

    bool isRamDefined() const { return _RAMsize != -1; }
    bool isRomDefined() const { return _size != -1; }
    bool isValid(QString* errorString = nullptr) const;
    bool isInstalled() const {return m_installed;}
private:
    bool isValid(QString& errorString) const;
};

#endif // FLASHALGORITHM_H
