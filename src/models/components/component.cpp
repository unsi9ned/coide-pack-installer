#include <QUuid>
#include <QDateTime>
#include <QStringList>
#include "component.h"
#include "common/constants.h"
#include "models/mcu/manufacturer.h"
#include "services/paths.h"
#include "models/pdsc/pdscfile.h"

int Component::getId() const
{
    return id;
}

qint32 Component::getCoMaxId() const
{
    return ID_COMPONENT_LAST;
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

void Component::addDefSymbol(const QString& symbol)
{
    if(!_definedSymbols.contains(symbol))
        _definedSymbols.append(symbol);
}

void Component::addDefSymbols(const QStringList& symbols)
{
    for(QString s : symbols)
    {
        addDefSymbol(s);
    }
}

QString Component::defSym2coMicro() const
{
    QString micro;

    foreach(QString d, _definedSymbols)
    {
        micro += d;
        micro += ",";
    }

    if(micro.endsWith(','))
        micro.chop(1);

    return micro;
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

QList<Component *> Component::getChildren() const
{
    return m_children;
}

void Component::setChildren(const QList<Component *> &value)
{
    m_children = value;
}

void Component::addChild(Component *child)
{
    if(!m_children.contains(child))
        m_children.append(child);
}

//bool Component::hasChild(const QString &childName)
//{
//    Component * c = getChild(childName);
//    return c != nullptr;
//}

//bool Component::hasChild(Component *child)
//{
//    return m_children.contains(child);
//}

bool Component::hasChildren()
{
    return !m_children.isEmpty();
}

Component *Component::getChild(const QString &childName)
{
    Component * child = nullptr;

    if(!m_children.isEmpty())
    {
        foreach (Component* c, m_children)
        {
            if(c && c->getName() == childName)
            {
                child = c;
                return child;
            }
        }
    }

    return child;
}

QList<Component *> Component::getParents() const
{
    return m_parents;
}

void Component::setParents(const QList<Component *> &value)
{
    m_parents = value;
}

void Component::addParent(Component *parent)
{
    if(!m_parents.contains(parent))
        m_parents.append(parent);
}

bool Component::hasParents()
{
    return !m_parents.isEmpty();
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

//------------------------------------------------------------------------------
// Возвращает общий список файлов, входящих в компонент в формате:
// <category>=<filepath>
//------------------------------------------------------------------------------
QStringList &Component::files()
{
    return _files;
}

QStringList Component::fileListConst() const
{
    return _files;
}

//------------------------------------------------------------------------------
// Возвращает только список файлов, относящихся к категории header (обычно *.h):
// Формат: <filepath>
//------------------------------------------------------------------------------
QStringList Component::headers()
{
    QStringList list;

    foreach (QString f, _files)
    {
        if(f.startsWith("header="))
            list.append(f.mid(sizeof("header=") - 1));
    }

    return list;
}

//------------------------------------------------------------------------------
// Возвращает только список каталогов, относящихся к категории include (каталог,
// который содержит *.h - файлы)
// Формат: <filepath>
//------------------------------------------------------------------------------
QStringList Component::includes()
{
    QStringList list;

    foreach (QString f, _files)
    {
        if(f.startsWith("include="))
            list.append(f.mid(sizeof("include=") - 1));
    }

    return list;
}

//------------------------------------------------------------------------------
// Возвращает только список статических библиотек (*.a)
// Формат: <filepath>
//------------------------------------------------------------------------------
QStringList Component::libraries()
{
    QStringList list;

    foreach (QString f, _files)
    {
        if(f.startsWith("library=") || f.startsWith("object="))
            list.append(f.mid(f.indexOf("=") + 1));
    }

    return list;
}

//------------------------------------------------------------------------------
// Возвращает только список исходников (*.с, *.cpp, *.h, *.s, *.asm и т.д.)
// Формат: <filepath>
//------------------------------------------------------------------------------
QStringList Component::sources()
{
    QStringList list;

    foreach (QString f, _files)
    {
        if(f.startsWith("source=") ||
           f.startsWith("sourceC=") ||
           f.startsWith("sourceCpp=") ||
           f.startsWith("sourceAsm=") ||
           f.startsWith("header="))
        {
            list.append(f.mid(f.indexOf("=") + 1));
        }
    }

    return list;
}

//------------------------------------------------------------------------------
// Возвращает только список скриптов линкера (*.ld)
// Формат: <filepath>
//------------------------------------------------------------------------------
QStringList Component::linkerScripts()
{
    QStringList list;

    foreach (QString f, _files)
    {
        if(f.startsWith("linkerScript="))
            list.append(f.mid(sizeof("linkerScript=") - 1));
    }

    return list;
}

void Component::addSupportedMcu(const QString &mcuName)
{
    if(!_supportsMcuList.contains(mcuName))
        _supportsMcuList.append(mcuName);
}

void Component::addSupportedMcuList(const QStringList &mcuList)
{
    foreach (QString mcuName, mcuList)
    {
        addSupportedMcu(mcuName);
    }
}

void Component::removeSupportedMcu(const QString &mcuName)
{
    if(_supportsMcuList.contains(mcuName))
        _supportsMcuList.removeOne(mcuName);
}

QStringList Component::supportedMcuList() const
{
    return _supportsMcuList;
}

Component::ComponentStatus Component::getStatus() const
{
    return _status;
}

void Component::setStatus(Component::ComponentStatus status)
{
    _status = status;
    componentStatusId = status.statusId;
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

bool Component::isPersisted() const
{
    return _isPersisted;
}

void Component::setPersisted(bool state)
{
    _isPersisted = state;
}

bool Component::isExternal() const
{
    return m_external;
}

void Component::setExternal(bool external)
{
    m_external = external;
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

Component::operator==(const Component &component)
{
    bool equal = (this->layerId == component.getLayerId() &&
                  this->type == component.getType() &&
                  this->name == component.getName() &&
                  this->description == component.getDescription() &&
                  this->version == component.getVersion() &&
                  this->_category.getName() == component.getCategory().getName());

    if(_category.hasSubCategory() && component.getCategory().hasSubCategory())
    {
        if(_category.getSubCategoryName() != component.getCategory().getSubCategoryName())
            equal = false;
    }
    else if(!_category.hasSubCategory() && !component.getCategory().hasSubCategory())
        equal = equal;
    else
        equal = false;

    QStringList currFileList = _files;

    foreach (QString f, component.fileListConst())
    {
        if(currFileList.contains(f))
            currFileList.removeOne(f);
        else
        {
            equal = false;
            break;
        }
    }

    equal = equal && currFileList.isEmpty();

    return equal;
}

QString Component::getPath() const
{
    if(!m_jdscPath.isEmpty())
        return m_jdscPath;
    else
        return m_pdscAttributes.makePath();
}

//------------------------------------------------------------------------------
// Преобразует компонент к формату PDSC
//------------------------------------------------------------------------------
PdscComponent Component::toPdscComponent() const
{
    PdscComponentAttributes attr = m_pdscAttributes;
    PdscComponent pdscComponent;
    PdscCondition requireComponentCond;
    PdscCondition acceptDevicesCond;

    pdscComponent.setAttributes(attr);
    pdscComponent.setDescription(description);

    if(!m_parents.isEmpty())
    {
        QString condId = m_pdscAttributes.getPdscCondition().isEmpty() ?
                         name:
                         m_pdscAttributes.getPdscCondition();

        requireComponentCond.setId(condId);

        for(Component* parent : m_parents)
        {
            PdscRequirement require(PdscRequirement::Require);

            QString Cclass = parent->getPdscClass();
            QString Cgroup = parent->getPdscGroup();
            QString Csub = parent->getPdscSub();
            QString Cvariant = parent->getPdscVariant();
            QString Cversion = parent->getPdscVersion();

            require.setCclass(Cclass);
            require.setCgroup(Cgroup);
            require.setCsub(Csub);
            require.setCvariant(Cvariant);
            require.setCversion(Cversion);

            requireComponentCond.addRequirement(require);
        }
    }

    if(!_supportsMcuList.isEmpty() &&
       m_pdscAttributes.getCclass().toUpper() != "CMSIS" &&
       m_pdscAttributes.getCgroup().toUpper() != "CORE")
    {
        QString deviceVendor = Manufacturer::makeKeilVendor(m_pdscAttributes.getCvendor());

        // Искусственно создает атрибут condition
        acceptDevicesCond.setId(makeCondition());

        for(QString mcu : _supportsMcuList)
        {
            PdscRequirement accept(PdscRequirement::Accept);

            accept.setDvendor(deviceVendor);
            accept.setDname(mcu);
            acceptDevicesCond.addRequirement(accept);
        }
    }

    // Восстанавливаем список файлов
    QList<QFileInfo> files = Paths::instance()->componentFilesVerbose(id, name);

    if(!files.isEmpty())
    {
        for(QFileInfo f : files)
        {
            PdscFile pFile;

            if(f.suffix().toLower() == "h")
            {
                pFile.setCategory(FileCategory("header"));
            }
            else if(f.suffix().toLower() == "c" || f.suffix().toLower() == "s")
            {
                pFile.setCategory(FileCategory("source"));
            }
            else if(f.suffix().toLower() == "ls" || f.suffix().toLower() == "ld")
            {
                pFile.setCategory(FileCategory("linkerScript"));
            }

            pFile.setName(f.filePath());
            pdscComponent.files().append(pFile);
        }
    }

    if(!requireComponentCond.isNull() && !acceptDevicesCond.isNull())
    {
        requireComponentCond.addCondition(acceptDevicesCond);
        pdscComponent.setCondition(requireComponentCond);
    }
    else if(!requireComponentCond.isNull())
    {
        pdscComponent.setCondition(requireComponentCond);
    }
    else if(!acceptDevicesCond.isNull())
    {
        pdscComponent.setCondition(acceptDevicesCond);
    }

    return pdscComponent;
}

//------------------------------------------------------------------------------
// Если компонент не содержит поля condition, то можно сгенерировать его
// на основе списка поддерживаемых устройств
//------------------------------------------------------------------------------
QString Component::makeCondition() const
{
    QStringList mcuList = _supportsMcuList;
    std::sort(mcuList.begin(), mcuList.end());

    QString conditionId = mcuList.join("+") + "/";

    if(conditionId.endsWith('/'))
        conditionId.chop(1);

    return conditionId;
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
    _status(ComponentStatus::ok()),
    _isPersisted(false),
    m_external(false)
{

}


