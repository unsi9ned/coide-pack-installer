#include "jdsccomponent.h"

JdscComponent::JdscComponent(const QString& name, const QString& version) : m_external(true)
{
    if(!name.isEmpty())
        m_name = name;

    if(!version.isEmpty())
        m_version = version;
}

void JdscComponent::setGroup(const QString& group)
{
    m_group = group;
}

void JdscComponent::setVersion(const QString& version)
{
    m_version = version;
}

void JdscComponent::addFile(const QString& file)
{
    if(file.isEmpty()) return;
    m_files.append(file);
}

void JdscComponent::addFiles(const QStringList& files)
{
    foreach (const auto& f, files)
    {
        addFile(f);
    }
}

void JdscComponent::addParent(JdscComponent parent)
{
    if(parent.name().isEmpty()) return;
    m_parents.insert(parent.name(), parent);
}

void JdscComponent::addCondition(JdscCondition condition)
{
    if(condition.id().isEmpty()) return;
    m_conditions.insert(condition.id(), condition);
}

void JdscComponent::addDefine(const QString& define)
{
    if(define.isEmpty() || m_defines.contains(define)) return;
    m_defines.append(define);
}

void JdscComponent::setExternal(bool external)
{
    m_external = external;
}

bool JdscComponent::supportDevice(const QString& devName)
{
    bool status = false;

    for(const auto& cond : m_conditions)
    {
        if(cond.supportedDevices().contains(devName, Qt::CaseInsensitive))
        {
            status = true;
            break;
        }
    }

    return status;
}

void JdscComponent::setName(const QString& name)
{
    m_name = name;
}

void JdscComponent::setDescription(const QString& description)
{
    m_description = description;
}

void JdscComponent::setClass(const QString& cclass)
{
    m_class = cclass;
}

QString JdscComponent::name() const
{
    return m_name;
}

QString JdscComponent::description() const
{
    return m_description;
}

QString JdscComponent::cclass() const
{
    return m_class;
}

QString JdscComponent::group() const
{
    return m_group;
}

QString JdscComponent::version() const
{
    return m_version;
}

QStringList JdscComponent::files() const
{
    return m_files;
}

QMap<QString, JdscComponent> JdscComponent::parents() const
{
    return m_parents;
}

QMap<QString, JdscCondition> JdscComponent::conditions() const
{
    return m_conditions;
}

QStringList JdscComponent::defines() const
{
    return m_defines;
}

bool JdscComponent::isExternal() const
{
    return m_external;
}


