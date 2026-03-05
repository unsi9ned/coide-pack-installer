#include "example.h"

int Example::getId() const
{
    return id;
}

void Example::setId(int value)
{
    id = value;
}

int Example::getUserId() const
{
    return userId;
}

void Example::setUserId(int value)
{
    userId = value;
}

int Example::getStatusId() const
{
    return statusId;
}

void Example::setStatusId(int value)
{
    statusId = value;
}

QString Example::getName() const
{
    return name;
}

void Example::setName(const QString &value)
{
    name = value;
}

QString Example::getDescription() const
{
    return description;
}

void Example::setDescription(const QString &value)
{
    description = value;
}

int Example::getType() const
{
    return type;
}

void Example::setType(int value)
{
    type = value;
}

QString Example::getUuid() const
{
    return uuid;
}

void Example::setUuid(const QString &value)
{
    uuid = value;
}

QString Example::getTimeuuid() const
{
    return timeuuid;
}

void Example::setTimeuuid(const QString &value)
{
    timeuuid = value;
}

QString Example::getRepo_user() const
{
    return repo_user;
}

void Example::setRepoUser(const QString &value)
{
    repo_user = value;
}

QString Example::getRepo_password() const
{
    return repo_password;
}

void Example::setRepoPassword(const QString &value)
{
    repo_password = value;
}

QString Example::getCreateDate() const
{
    return createDate;
}

void Example::setCreateDate(const QString &value)
{
    createDate = value;
}

QString Example::getUpdateDate() const
{
    return updateDate;
}

void Example::setUpdateDate(const QString &value)
{
    updateDate = value;
}

int Example::getHits() const
{
    return hits;
}

void Example::setHits(int value)
{
    hits = value;
}

QMap<int, Component *> Example::getParentComponents() const
{
    return parentComponents;
}

void Example::setParentComponents(const QMap<int, Component *> &value)
{
    parentComponents = value;
}

void Example::addParentComponent(int componentId)
{
    if(parentComponents.contains(componentId))
        return;
    else
        parentComponents.insert(componentId, nullptr);
}

void Example::addParentComponent(Component *component)
{
    if(component)
    {
        if(parentComponents.contains(component->getId()))
            parentComponents[component->getId()] = component;
        else
            parentComponents.insert(component->getId(), component);
    }
}

Component::ComponentStatus Example::getStatus() const
{
    return status;
}

void Example::setStatus(const Component::ComponentStatus &value)
{
    status = value;
}

bool Example::isDownloaded()
{
    return status.hasDownloaded;
}

Example::Example()
{

}
