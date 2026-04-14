#ifndef COMPONENT_H
#define COMPONENT_H

#include <QString>
#include <QList>
#include <QList>
#include <QDateTime>
#include "category.h"
#include "models/mcu/devicehierarchynode.h"
#include "models/pdsc/pdsccomponentattr.h"

class PdscComponentAttributesEx : public PdscComponentAttributes
{
private:
    QString m_condition;    // optional

public:
    PdscComponentAttributesEx() : m_condition(QString()) {}

    void setPdscCondition(const QString& condition) { m_condition = condition; }
    const QString getPdscCondition() const { return m_condition; }

    operator ==(const PdscComponentAttributesEx& attr) const
    {
        if(m_condition != attr.getPdscCondition()) return false;
        if(m_cvendor != attr.getCvendor()) return false;
        if(m_cbundle != attr.getCbundle()) return false;
        if(m_cclass != attr.getCclass()) return false;
        if(m_cgroup != attr.getCgroup()) return false;
        if(m_csub != attr.getCsub()) return false;
        if(m_cvariant != attr.getCvariant()) return false;
        if(m_cversion != attr.getCversion()) return false;
        if(m_capiversion != attr.getCapiversion()) return false;
        if(m_instances != attr.getInstances()) return false;

        return true;
    }

    operator !=(const PdscComponentAttributesEx& attr) const
    {
        bool compare = *this == attr;
        return !compare;
    }
};

class Component : public DeviceHierarchyNode
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
    QStringList _definedSymbols;
    int cox; //Зависит ли компонент от драйверов CooCox
    QString version;
    QString publishStatus;
    int hits;
    QDateTime createDate;
    QDateTime updateDate;
    QString tags;

    QList<Component*> m_children;
    QList<Component*> m_parents;

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

    // PDSC attributes
    PdscComponentAttributesEx m_pdscAttributes;

    QString m_jdscPath;
    bool m_external;

public:
    Component();

    int getId() const;
    qint32 getCoMaxId() const;
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

    void addDefSymbol(const QString& symbol);
    void addDefSymbols(const QStringList& symbols);
    QStringList definedSymbols() const {return this->_definedSymbols;}
    QString defSym2coMicro() const;

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


    QList<Component *> getChildren() const;
    void setChildren(const QList<Component *> &value);
    void addChild(Component * getChild);
    bool hasChild(const QString& childName);
    bool hasChild(Component* child);
    bool hasChildren();
    Component* getChild(const QString& childName);

    QList<Component *> getParents() const;
    void setParents(const QList<Component *> &value);
    void addParent(Component * parent);
    bool hasParents();


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

    bool isExternal() const;
    void setExternal(bool external);

    QString generateTimeUUID();

    operator==(const Component& component);

    QString getPath() const;

    void setPdscVendor(const QString& vendor) { m_pdscAttributes.setCvendor(vendor); }
    void setPdscClass(const QString& c) { m_pdscAttributes.setCclass(c); }
    void setPdscGroup(const QString& group) { m_pdscAttributes.setCgroup(group); }
    void setPdscSub(const QString& sub) { m_pdscAttributes.setCsub(sub); }
    void setPdscVariant(const QString& variant) { m_pdscAttributes.setCvariant(variant); }
    void setPdscVersion(const QString& version) { m_pdscAttributes.setCversion(version); }
    void setPdscCondition(const QString& condition) { m_pdscAttributes.setPdscCondition(condition); }

    const QString getPdscVendor() { return m_pdscAttributes.getCvendor(); }
    const QString getPdscClass() { return m_pdscAttributes.getCclass(); }
    const QString getPdscGroup() { return m_pdscAttributes.getCgroup(); }
    const QString getPdscSub() { return m_pdscAttributes.getCsub(); }
    const QString getPdscVariant() { return m_pdscAttributes.getCvariant(); }
    const QString getPdscVersion() { return m_pdscAttributes.getCversion(); }
    const QString getPdscCondition() { return m_pdscAttributes.getPdscCondition(); }

    PdscComponentAttributesEx pdscAttributes() const {return m_pdscAttributes; }
    void setPdscAttributes(const PdscComponentAttributesEx& attr) {m_pdscAttributes = attr; }

    void setJdscPath(const QString& path) { m_jdscPath = path; }
    const QString getJdscPath() { return m_jdscPath; }
};

#endif // COMPONENT_H
