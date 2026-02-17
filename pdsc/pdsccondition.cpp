#include "pdsccondition.h"

PdscCondition::PdscCondition() : PdscElement()
{

}

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

QList<PdscCondition> PdscCondition::conditions() const
{
    return m_conditions;
}

void PdscCondition::addCondition(const PdscCondition &condition)
{
    m_conditions.append(condition);
}

QList<PdscRequirement> PdscCondition::requirements() const
{
    return m_requirements;
}

bool PdscCondition::isNull()
{
    return m_requirements.isEmpty() && m_conditions.isEmpty();
}

void PdscCondition::addRequirement(const PdscRequirement &requirement)
{
    m_requirements.append(requirement);
}


