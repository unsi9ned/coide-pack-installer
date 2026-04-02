#include "progalgorithm.h"
#include "common/constants.h"

uint32_t ProgAlgorithm::size() const
{
    return _size;
}

void ProgAlgorithm::setSize(const uint32_t &size)
{
    _size = size;
}

uint32_t ProgAlgorithm::RAMstart() const
{
    return _RAMstart;
}

void ProgAlgorithm::setRAMstart(const uint32_t &RAMstart)
{
    _RAMstart = RAMstart;
}

uint32_t ProgAlgorithm::RAMsize() const
{
    return _RAMsize;
}

void ProgAlgorithm::setRAMsize(const uint32_t &RAMsize)
{
    _RAMsize = RAMsize;
}

bool ProgAlgorithm::isDefault() const
{
    return _isDefault;
}

void ProgAlgorithm::setDefault(bool isDefault)
{
    _isDefault = isDefault;
}

void ProgAlgorithm::setDocumentId(int id)
{
    _documentId = id;
}

void ProgAlgorithm::setTimeUUID(const QString &uuid)
{
    _timeUuid = uuid;
}

void ProgAlgorithm::setCreateDate(const QString &dt)
{
    _createDate = QDateTime::fromString(dt, "yyyy-MM-dd HH:mm:ss.z");
}

void ProgAlgorithm::setUpdateDate(const QString &dt)
{
    _updateDate = QDateTime::fromString(dt, "yyyy-MM-dd HH:mm:ss.z");
}

uint32_t ProgAlgorithm::start() const
{
    return _start;
}

void ProgAlgorithm::setStart(const uint32_t &start)
{
    _start = start;
}

ProgAlgorithm::ProgAlgorithm(const QString &name)
{
    this->_coId = -1;
    this->_name = name;
    this->_timeUuid = generateTimeUUID();
    this->_createDate = QDateTime::currentDateTime();
    this->_updateDate = QDateTime::currentDateTime();
    this->_documentId = static_cast<int>(CoDocument::DOC_FLASH_ALGO);
}

void ProgAlgorithm::setCoId(int id)
{
    this->_coId = id;
}

void ProgAlgorithm::setName(QString name)
{
    this->_name = name;
}

void ProgAlgorithm::setInstallPath(const QString &path)
{
    _installPath = path;
}

int ProgAlgorithm::coId() const
{
    return this->_coId;
}

QString ProgAlgorithm::name() const
{
    return this->_name;
}

QString ProgAlgorithm::installPath() const
{
    return _installPath;
}

bool ProgAlgorithm::isNull()
{
    return this->_coId <= 0 || this->_name.isEmpty();
}

QString ProgAlgorithm::getPath() const
{
    QString path = _installPath;
    return path.replace('\\', '/');
}

int ProgAlgorithm::documentId() const
{
    return _documentId;
}

QString ProgAlgorithm::timeUUID() const
{
    return _timeUuid;
}

QString ProgAlgorithm::creationDate(QString dtFormat) const
{
    QDateTime dt = this->_createDate;

    if(!dt.isValid())
        dt = QDateTime::currentDateTime();

    return dt.toString(dtFormat);
}

QString ProgAlgorithm::updateDate(QString dtFormat) const
{
    QDateTime dt = this->_updateDate;

    if(!dt.isValid())
        dt = QDateTime::currentDateTime();

    return dt.toString(dtFormat);
}
