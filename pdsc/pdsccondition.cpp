#include "pdsccondition.h"

QString PdscCondition::id() const
{
    return m_id;
}

void PdscCondition::setId(const QString &id)
{
    m_id = id;
}

QString PdscCondition::description() const
{
    return m_description;
}

void PdscCondition::setDescription(const QString &description)
{
    m_description = description;
}

PdscCondition::PdscCondition() : PdscElement()
{

}
