#ifndef COMPONENT_H
#define COMPONENT_H

#include <QString>
#include <QList>
#include <QList>
#include "category.h"

class Component
{
public:

    enum ComponentType
    {
        DRIVER = 1,
        COMPONENT = 2,
    };

    struct ComponentStatus
    {
        int statusId;
        int shouldUpdate;
        int hasDownloaded;
        int hasDeleted;
        int auditStatus;

        ComponentStatus()
        {
            statusId = 0;
            shouldUpdate = 0;
            hasDownloaded = 0;
            hasDeleted = 0;
            auditStatus = 0;
        }

        bool isNull()
        {
            return statusId == 0;
        }
    };

private:

    int id;
    int authorId;
    int layerId;
    int componentStatusId;
    int shareDocumentId;
    int type;
    QString name;
    QString description;
    QString advertisingWord;
    QString advertisingURL;
    QString uuid;
    QString timeuuid;
    QString repoUser;
    QString repoPass;
    QString micro;
    int cox;
    QString version;
    QString publishStatus;
    int hits;
    QString createDate;
    QString updateDate;
    QString tags;

    QList<Component*> dependencies;
    QList<int> mcuListId;
    QList<int> mcuFamilyList;
    QList<int> mcuSeriesList;
    QList<int> mcuManufacturerList;

    ComponentStatus _status;
    QStringList _files;
    Category _category;

public:
    Component();
    Component(Component * c);

    int getId() const;
    void setId(int value);
    int getAuthorId() const;
    void setAuthorId(int value);
    int getLayerId() const;
    void setLayerId(int value);
    int getComponentStatusId() const;
    void setComponentStatusId(int value);
    int getShareDocumentId() const;
    void setShareDocumentId(int value);
    int getType() const;
    void setType(int value);
    QString getName() const;
    void setName(const QString &value);
    QString getDescription() const;
    void setDescription(const QString &value);
    QString getAdvertisingWord() const;
    void setAdvertisingWord(const QString &value);
    QString getAdvertisingURL() const;
    void setAdvertisingURL(const QString &value);
    QString getUuid() const;
    void setUuid(const QString &value);
    QString getTimeuuid() const;
    void setTimeuuid(const QString &value);
    QString getRepoUser() const;
    void setRepoUser(const QString &value);
    QString getRepoPass() const;
    void setRepoPass(const QString &value);
    QString getMicro() const;
    void setMicro(const QString &value);
    int getCox() const;
    void setCox(int value);
    QString getVersion() const;
    void setVersion(const QString &value);
    QString getPublishStatus() const;
    void setPublishStatus(const QString &value);
    int getHits() const;
    void setHits(int value);
    QString getCreateDate() const;
    void setCreateDate(const QString &value);
    QString getUpdateDate() const;
    void setUpdateDate(const QString &value);
    QString getTags() const;
    void setTags(const QString &value);
    QList<Component *> getDependencies() const;
    void setDependencies(const QList<Component *> &value);
    void addDependence(Component * child);
    QList<int> getMcuListId() const;
    void setMcuListId(const QList<int> &value);
    void appendMcuId(int id);
    QList<int> getMcuFamilyList() const;
    void setMcuFamilyList(const QList<int> &value);
    void appendMcuFamily(int id);
    QList<int> getMcuSeriesList() const;
    void setMcuSeriesList(const QList<int> &value);
    void appendMcuSerie(int id);
    QList<int> getMcuManufacturerList() const;
    void setMcuManufacturerList(const QList<int> &value);
    void appendMcuManufacturer(int id);
    QStringList& files();

    Category getCategory() const;
    void setCategory(const Category &category);

    ComponentStatus getStatus();
    void setStatus(ComponentStatus status);

    bool isDownloaded();
    bool isDriver();
    bool isNull();

};

#endif // COMPONENT_H
