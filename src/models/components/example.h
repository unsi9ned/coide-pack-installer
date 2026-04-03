#ifndef EXAMPLE_H
#define EXAMPLE_H

#include <QString>
#include <QList>
#include <QVector>
#include <QMap>
#include "component.h"

class Example
{
private:
    int id;
    int userId;
    int statusId;
    QString name;
    QString description;
    int type;
    QString uuid;
    QString timeuuid;
    QString repo_user;
    QString repo_password;
    QString createDate;
    QString updateDate;
    int hits;

    QMap<int, Component*> parentComponents;

    Component::ComponentStatus status;

public:
    Example();
    int getId() const;
    void setId(int value);
    int getUserId() const;
    void setUserId(int value);
    int getStatusId() const;
    void setStatusId(int value);
    QString getName() const;
    void setName(const QString &value);
    QString getDescription() const;
    void setDescription(const QString &value);
    int getType() const;
    void setType(int value);
    QString getUuid() const;
    void setUuid(const QString &value);
    QString getTimeuuid() const;
    void setTimeuuid(const QString &value);
    QString getRepo_user() const;
    void setRepoUser(const QString &value);
    QString getRepo_password() const;
    void setRepoPassword(const QString &value);
    QString getCreateDate() const;
    void setCreateDate(const QString &value);
    QString getUpdateDate() const;
    void setUpdateDate(const QString &value);
    int getHits() const;
    void setHits(int value);
    QMap<int, Component*> getParentComponents() const;
    void setParentComponents(const QMap<int, Component*> &value);
    void addParentComponent(int componentId);
    void addParentComponent(Component * component);
    Component::ComponentStatus getStatus() const;
    void setStatus(const Component::ComponentStatus &value);

    bool isDownloaded();
};

#endif // EXAMPLE_H
