#include "pdsccomponent.h"

QString PdscComponent::description() const
{
    return m_description;
}

void PdscComponent::setDescription(const QString &description)
{
    m_description = description;
}

PdscCondition PdscComponent::condition() const
{
    return m_condition;
}

void PdscComponent::setCondition(const PdscCondition &condition)
{
    m_condition = condition;
}

PdscComponentAttributes& PdscComponent::attributes()
{
    return m_attributes;
}

void PdscComponent::setAttributes(const PdscComponentAttributes &attributes)
{
    m_attributes = attributes;
}

QList<PdscFile>& PdscComponent::files()
{
    return m_files;
}

bool PdscComponent::hasCondition()
{
    return !m_condition.isNull();
}

//------------------------------------------------------------------------------
// Вывод списка файлов, входящих в компонент. С соблюдением условий
//------------------------------------------------------------------------------
QStringList PdscComponent::fileList()
{
    QStringList fileList;

    foreach (PdscFile f, m_files)
    {

    }

    return fileList;
}

PdscComponent::PdscComponent() : PdscElement()
{

}
