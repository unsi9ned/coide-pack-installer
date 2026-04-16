#include "progalgorithm.h"
#include "common/constants.h"
#include "services/paths.h"

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

void ProgAlgorithm::setDescription(const QString& dscr)
{
    _description = dscr;
}

void ProgAlgorithm::parseCoName(const QString &coName)
{
    QString path = Paths::instance()->coIdeFlashAlgorithmDir() + "/" + coName;

    m_vendor = Paths::instance()->getVendorFromPackDir(path);
    m_version = Paths::instance()->getVersionFromPackDir(path);
    _name = Paths::instance()->getRelativePathInPack(path);
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
    this->_description = "";
    this->_timeUuid = generateTimeUUID();
    this->_start = 0;
    this->_size = -1;
    this->_RAMstart = 0;
    this->_RAMsize = -1;
    this->_isDefault = false;
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

int ProgAlgorithm::coId() const
{
    return this->_coId;
}

QString ProgAlgorithm::name() const
{
    return this->_name;
}

bool ProgAlgorithm::isNull()
{
    return this->_coId <= 0 ||
           this->_name.isEmpty();
}

QString ProgAlgorithm::getPath() const
{
    QString flashDir = Paths::instance()->coIdeFlashAlgorithmDir();
    QString path = Paths::instance()->coIdePackDirRelative(m_vendor, m_version, flashDir);

    if(!path.isEmpty() && !path.endsWith('/')) path += "/";

    path += _name;
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

QString ProgAlgorithm::description() const
{
    return _description;
}

bool ProgAlgorithm::isValid(QString* errorString) const
{
    QString e;

    if(errorString)
        return isValid(*errorString);
    else
        return isValid(e);
}

bool ProgAlgorithm::isValid(QString& errorString) const
{
    qint32 uniqueId = getUniqueId();
    QString uniquePath = getPath();

    if(uniqueId <= 0 || uniqueId <= getCoMaxId())
        errorString = QString("Invalid Flash Algorithm ID = %1").arg(uniqueId);
    else if(_name.isEmpty())
        errorString = QString("Flash Algorithm name is not defined");
    else if(uniquePath.isEmpty())
        errorString = QString("Flash Algorithm path is not defined");
    else if(_timeUuid.isEmpty())
        errorString = QString("Uuid for Flash Algorithm is not defined");
    else if(_size != -1 && _RAMsize != -1 && _start == _RAMstart)
        errorString = QString("FLASH and RAM memory have the same addresses");
    else if(_size == 0)
        errorString = QString("The FLASH region has a size of zero");
    else if(_RAMsize == 0)
        errorString = QString("The RAM region has a size of zero");
    else return true;

    return false;
}
