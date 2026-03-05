#ifndef COMPONENT_H
#define COMPONENT_H

#include <QString>
#include <QList>
#include <QList>
#include <QDateTime>
#include "category.h"

class Component
{
public:

    enum ComponentType
    {
        DRIVER = 1,
        COMPONENT = 2,
    };

    enum ComponentLayer
    {
        LAYER_DRIVER = 1,
        LAYER_MCU = 2,
        LAYER_USER = 3,
        LAYER_MIDDLEWARE = 4,
        LAYER_DEVICE = 5,
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

        static ComponentStatus ok()
        {
            ComponentStatus status;
            status.hasDownloaded = 1;
            status.auditStatus = 1;
            return status;
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
    int cox; //Зависит ли компонент от драйверов CooCox
    QString version;
    QString publishStatus;
    int hits;
    QDateTime createDate;
    QDateTime updateDate;
    QString tags;

    QList<Component*> dependencies;
    QList<int> mcuListId;
    QList<int> mcuFamilyList;
    QList<int> mcuSeriesList;
    QList<int> mcuManufacturerList;

    QStringList _supportsMcuList;
    QStringList _exampleList;

    ComponentStatus _status;
    QStringList _files;
    Category _category;

    bool _isPersisted;

public:
    Component();

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


    void setCreationDate(const QString &value);
    void setUpdateDate(const QString &value);

    QString getCreationDate(QString dtFormat = QString("yyyy-MM-dd HH:mm:ss")) const;
    QString getUpdateDate(QString dtFormat = QString("yyyy-MM-dd HH:mm:ss")) const;

    QString getTags() const;
    void setTags(const QString &value);
    QList<Component *> getDependencies() const;
    void setDependencies(const QList<Component *> &value);
    void addChild(Component * getChild);
    bool hasChild(const QString& childName);
    bool hasChild(Component* child);
    bool hasChildren();
    Component* getChild(const QString& childName);
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
    QStringList fileListConst() const;

    QStringList headers();
    QStringList includes();
    QStringList libraries();
    QStringList sources();
    QStringList linkerScripts();

    void addSupportedMcu(const QString& mcuName);
    void addSupportedMcuList(const QStringList& mcuList);
    void removeSupportedMcu(const QString& mcuName);
    QStringList supportedMcuList() const;

    Category getCategory() const;
    void setCategory(const Category &category);

    ComponentStatus getStatus();
    void setStatus(ComponentStatus status);

    bool isDownloaded();
    bool isDriver();
    bool isNull();
    bool isPersisted() const;
    void setPersisted(bool state);

    static QString generateTimeUUID();

    operator==(const Component& component);
};

#endif // COMPONENT_H
