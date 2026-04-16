#include <QFileInfo>
#include "example.h"
#include "common/constants.h"

Example::Example() :
    id(-1),
    userId(CoUser::USER_COOCOX),
    type(Component::EXAMPLE),
    uuid(Example::generateTimeUUID()),
    timeuuid(Example::generateTimeUUID()),
    repo_user("admin"),
    repo_password("C9NOPZXdgT8="),
    createDate(QDateTime::currentDateTime()),
    updateDate(QDateTime::currentDateTime()),
    hits(0),
    m_status(Component::ComponentStatus::ok())
{

}

Example::Example(const Component& component) :
    userId(CoUser::USER_COOCOX),
    type(Component::EXAMPLE)
{
    id = component.getId();
    uuid = component.getUuid();
    name = component.getName();
    description = component.getDescription();
    timeuuid = component.getTimeuuid();
    repo_user = component.getRepoUser();
    repo_password = component.getRepoPass();
    setCreateDate(component.getCreationDate());
    setUpdateDate(component.getUpdateDate());
    hits = component.getHits();
    m_status = component.getStatus();
    m_pdscAttributes = component.pdscAttributes();

    for(const Component* parent : component.getParents())
    {
        m_parentComponents.insert(parent->getId(), parent);
    }

    for(QString f : component.fileListConst())
    {
        m_files.append(f);
    }
}

int Example::getId() const
{
    return id;
}

void Example::setId(int value)
{
    id = value;
}

qint32 Example::getCoMaxId() const
{
    return CoIds::ID_EXAMPLE_LAST;
}

int Example::getUserId() const
{
    return userId;
}

void Example::setUserId(int value)
{
    userId = value;
}

qint32 Example::getStatusId() const
{
    return m_status.statusId;
}

void Example::setStatusId(qint32 value)
{
    m_status.statusId = value;
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

QString Example::getCoDescription() const
{
    QString dscr = this->description;

    for(QString f : m_files)
    {
        //<type>=<path>
        QStringList attr = f.split('=', QString::SkipEmptyParts) << "" << "";
        QString path = attr.at(1);
        QFileInfo file(path);
        CoFileType coType = ANY_FILE;

        if(file.suffix().toLower() == "c" || file.suffix().toLower() == "s")
            coType = SOURCE_FILE;
        else if(file.suffix().toLower() == "h")
            coType = HEADER_FILE;
        else
            coType = ANY_FILE;

        dscr += QString("[%1:%2:%3:%4]").arg(path).arg("").arg(coType).arg("");
    }

    return dscr;
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

QString Example::getCreationDate(QString dtFormat) const
{
    QDateTime dt = this->createDate;

    if(!dt.isValid())
        dt = QDateTime::currentDateTime();

    return dt.toString(dtFormat);
}

void Example::setCreateDate(const QString &value)
{
    this->createDate = QDateTime::fromString(value, "yyyy-MM-dd HH:mm:ss");
}

QString Example::getUpdateDate(QString dtFormat) const
{
    QDateTime dt = this->updateDate;

    if(!dt.isValid())
        dt = QDateTime::currentDateTime();

    return dt.toString(dtFormat);
}

void Example::setUpdateDate(const QString &value)
{
    this->updateDate = QDateTime::fromString(value, "yyyy-MM-dd HH:mm:ss");
}

int Example::getHits() const
{
    return hits;
}

void Example::setHits(int value)
{
    hits = value;
}

QList<const Component*> Example::getParentComponents() const
{
    return m_parentComponents;
}

void Example::setParentComponents(const QList<const Component*>& value)
{
    m_parentComponents = value;
}

void Example::addParentComponent(Component *component)
{
    if(component && !m_parentComponents.contains(component))
        m_parentComponents.append(component);
}

void Example::addSupportComponent(const Component* component)
{
    if(component && !m_supportComponents.contains(component))
        m_supportComponents.append(component);
}

bool Example::hasParents() const
{
    auto parents = m_parentComponents;
    return !parents.isEmpty();
}

QList<const Component *> Example::getConsumerComponents() const
{
    return m_supportComponents;
}

bool Example::hasConsumer() const
{
    auto consumers = m_supportComponents;
    return !consumers.isEmpty();
}

Component::ComponentStatus Example::getStatus() const
{
    return m_status;
}

void Example::setStatus(const Component::ComponentStatus &value)
{
    m_status = value;
}

bool Example::isDownloaded()
{
    return m_status.hasDownloaded;
}

bool Example::isNull() const
{
    return id == -1 || name.isEmpty();
}

QString Example::getPath() const
{
    return m_pdscAttributes.makePath();
}

PdscComponentAttributesEx Example::pdscAttributes() const
{
    return m_pdscAttributes;
}


