#include <QUuid>
#include <QDateTime>
#include "component.h"
#include "constants.h"

int Component::getId() const
{
    return id;
}

void Component::setId(int value)
{
    id = value;
}

int Component::getAuthorId() const
{
    return authorId;
}

void Component::setAuthorId(int value)
{
    authorId = value;
}

int Component::getLayerId() const
{
    return layerId;
}

void Component::setLayerId(int value)
{
    layerId = value;
}

int Component::getComponentStatusId() const
{
    return componentStatusId;
}

void Component::setComponentStatusId(int value)
{
    componentStatusId = value;
}

int Component::getShareDocumentId() const
{
    return shareDocumentId;
}

void Component::setShareDocumentId(int value)
{
    shareDocumentId = value;
}

int Component::getType() const
{
    return type;
}

void Component::setType(int value)
{
    type = value;
}

QString Component::getName() const
{
    return name;
}

void Component::setName(const QString &value)
{
    name = value;
}

QString Component::getDescription() const
{
    return description;
}

void Component::setDescription(const QString &value)
{
    description = value;
}

QString Component::getAdvertisingWord() const
{
    return advertisingWord;
}

void Component::setAdvertisingWord(const QString &value)
{
    advertisingWord = value;
}

QString Component::getAdvertisingURL() const
{
    return advertisingURL;
}

void Component::setAdvertisingURL(const QString &value)
{
    advertisingURL = value;
}

QString Component::getUuid() const
{
    return uuid;
}

void Component::setUuid(const QString &value)
{
    uuid = value;
}

QString Component::getTimeuuid() const
{
    return timeuuid;
}

void Component::setTimeuuid(const QString &value)
{
    timeuuid = value;
}

QString Component::getRepoUser() const
{
    return repoUser;
}

void Component::setRepoUser(const QString &value)
{
    repoUser = value;
}

QString Component::getRepoPass() const
{
    return repoPass;
}

void Component::setRepoPass(const QString &value)
{
    repoPass = value;
}

QString Component::getMicro() const
{
    return micro;
}

void Component::setMicro(const QString &value)
{
    micro = value;
}

int Component::getCox() const
{
    return cox;
}

void Component::setCox(int value)
{
    cox = value;
}

QString Component::getVersion() const
{
    return version;
}

void Component::setVersion(const QString &value)
{
    version = value;
}

QString Component::getPublishStatus() const
{
    return publishStatus;
}

void Component::setPublishStatus(const QString &value)
{
    publishStatus = value;
}

int Component::getHits() const
{
    return hits;
}

void Component::setHits(int value)
{
    hits = value;
}

QString Component::getCreationDate(QString dtFormat) const
{
    QDateTime dt = this->createDate;

    if(!dt.isValid())
        dt = QDateTime::currentDateTime();

    return dt.toString(dtFormat);
}

QString Component::getUpdateDate(QString dtFormat) const
{
    QDateTime dt = this->updateDate;

    if(!dt.isValid())
        dt = QDateTime::currentDateTime();

    return dt.toString(dtFormat);
}

void Component::setCreationDate(const QString &value)
{
    this->createDate = QDateTime::fromString(value, "yyyy-MM-dd HH:mm:ss");
}

void Component::setUpdateDate(const QString &value)
{
    this->updateDate = QDateTime::fromString(value, "yyyy-MM-dd HH:mm:ss");
}

QString Component::getTags() const
{
    return tags;
}

void Component::setTags(const QString &value)
{
    tags = value;
}

QList<Component *> Component::getDependencies() const
{
    return dependencies;
}

void Component::setDependencies(const QList<Component *> &value)
{
    dependencies = value;
}

void Component::addDependence(Component *child)
{
    dependencies.append(child);
}

QList<int> Component::getMcuListId() const
{
    return mcuListId;
}

void Component::setMcuListId(const QList<int> &value)
{
    mcuListId = value;
}

void Component::appendMcuId(int id)
{
    mcuListId.append(id);
}

QList<int> Component::getMcuFamilyList() const
{
    return mcuFamilyList;
}

void Component::setMcuFamilyList(const QList<int> &value)
{
    mcuFamilyList = value;
}

void Component::appendMcuFamily(int id)
{
    mcuFamilyList.append(id);
}

QList<int> Component::getMcuSeriesList() const
{
    return mcuSeriesList;
}

void Component::setMcuSeriesList(const QList<int> &value)
{
    mcuSeriesList = value;
}

void Component::appendMcuSerie(int id)
{
    mcuSeriesList.append(id);
}

QList<int> Component::getMcuManufacturerList() const
{
    return mcuManufacturerList;
}

void Component::setMcuManufacturerList(const QList<int> &value)
{
    mcuManufacturerList = value;
}

void Component::appendMcuManufacturer(int id)
{
    mcuManufacturerList.append(id);
}

QStringList &Component::files()
{
    return _files;
}

Component::ComponentStatus Component::getStatus()
{
    return _status;
}

void Component::setStatus(Component::ComponentStatus status)
{
    _status = status;
}

bool Component::isDownloaded()
{
    return _status.hasDownloaded;
}

bool Component::isDriver()
{
    return type == ComponentType::DRIVER;
}

bool Component::isNull()
{
    return id == -1;
}

QString Component::generateTimeUUID()
{
    QString str = QUuid::createUuid().toString();

    // Удаляем фигурные скобки в начале и конце
    if (str.startsWith('{') && str.endsWith('}'))
    {
        str = str.mid(1, str.length() - 2);
    }

    return str;
}

Category Component::getCategory() const
{
    return _category;
}

void Component::setCategory(const Category &category)
{
    _category = category;
}

Component::Component() :
    id(-1),
    authorId(CoUser::USER_COOCOX),
    layerId(ComponentLayer::LAYER_USER),
    componentStatusId(0),
    shareDocumentId(0),
    type(ComponentType::COMPONENT),
    uuid(Component::generateTimeUUID()),
    timeuuid(Component::generateTimeUUID()),
    repoUser("admin"),
    repoPass("omvmUMnmnac="),
    cox(0),
    version("1.0.0"),
    publishStatus("stable"),
    hits(0),
    createDate(QDateTime::currentDateTime()),
    updateDate(QDateTime::currentDateTime()),
    _status(ComponentStatus::ok())
{

}


