#ifndef EXAMPLE_H
#define EXAMPLE_H

#include <QString>
#include <QList>
#include <QVector>
#include <QMap>
#include <QDateTime>

#include "component.h"
#include "models/pdsc/pdsccomponentattrex.h"
#include "models/mcu/devicehierarchynode.h"

class Example : public DeviceHierarchyNode
{
private:
    enum CoFileType
    {
        ANY_FILE = 0,
        HEADER_FILE = 0,
        SOURCE_FILE = 1
    };

private:
    int id;
    int userId;
    QString name;
    QString description;
    int type;
    QString uuid;
    QString timeuuid;
    QString repo_user;
    QString repo_password;
    QDateTime createDate;
    QDateTime updateDate;
    int hits;

    QList<const Component*> m_parentComponents;
    QList<const Component*> m_supportComponents;
    Component::ComponentStatus m_status;
    PdscComponentAttributesEx m_pdscAttributes;

    QStringList m_files;

public:
    Example();
    Example(const Component& component);
    int getId() const;
    void setId(int value);
    qint32 getCoMaxId() const;
    int getUserId() const;
    void setUserId(int value);
    qint32 getStatusId() const;
    void setStatusId(qint32 value);
    QString getName() const;
    void setName(const QString &value);
    QString getDescription() const;
    QString getCoDescription() const;
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
    QString getCreationDate(QString dtFormat = QString("yyyy-MM-dd HH:mm:ss")) const;
    void setCreateDate(const QString &value);
    QString getUpdateDate(QString dtFormat = QString("yyyy-MM-dd HH:mm:ss")) const;
    void setUpdateDate(const QString &value);
    int getHits() const;
    void setHits(int value);

    QList<const Component*> getParentComponents() const;
    void setParentComponents(const QList<const Component*>& value);
    void addParentComponent(Component * component);
    void addSupportComponent(const Component * component);
    bool hasParents() const;

    QList<const Component*> getConsumerComponents() const;
    bool hasConsumer() const;

    Component::ComponentStatus getStatus() const;
    void setStatus(const Component::ComponentStatus &value);

    bool isDownloaded();
    bool isNull() const;

    QString getPath() const;
    PdscComponentAttributesEx pdscAttributes() const;
};

#endif // EXAMPLE_H
