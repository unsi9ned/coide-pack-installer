#include "component.h"

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

QString Component::getCreateDate() const
{
    return createDate;
}

void Component::setCreateDate(const QString &value)
{
    createDate = value;
}

QString Component::getUpdateDate() const
{
    return updateDate;
}

void Component::setUpdateDate(const QString &value)
{
    updateDate = value;
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

Category Component::getCategory() const
{
    return _category;
}

void Component::setCategory(const Category &category)
{
    _category = category;
}

Component::Component() : id(-1)
{

}

Component::Component(Component *c)
{
    this->id = c->getId();
    this->authorId = c->getAuthorId();
    this->layerId = c->getLayerId();
    this->componentStatusId = c->getComponentStatusId();
    this->shareDocumentId = c->getShareDocumentId();
    this->type = c->getType();
    this->name = c->getName();
    this->description = c->getDescription();
    this->advertisingWord = c->getAdvertisingWord();
    this->advertisingURL = c->getAdvertisingURL();
    this->uuid = c->getUuid();
    this->timeuuid = c->getTimeuuid();
    this->repoUser = c->getRepoUser();
    this->repoPass = c->getRepoPass();
    this->micro = c->getMicro();
    this->cox = c->getCox();
    this->version = c->getVersion();
    this->publishStatus = c->getPublishStatus();
    this->hits = c->getHits();
    this->createDate = c->getCreateDate();
    this->updateDate = c->getUpdateDate();
    this->tags = c->getTags();
    this->dependencies = c->getDependencies();
    this->mcuListId = c->getMcuListId();
    this->mcuFamilyList = c->getMcuFamilyList();
    this->mcuSeriesList = c->getMcuSeriesList();
    this->mcuManufacturerList = c->getMcuManufacturerList();
}

