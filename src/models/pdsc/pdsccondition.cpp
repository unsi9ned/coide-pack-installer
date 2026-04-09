#include <QDebug>
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

QList<PdscCondition> PdscCondition::nestedConditions() const
{
    return m_conditions;
}

void PdscCondition::addCondition(const PdscCondition &condition)
{
    m_conditions.append(condition);
}

QList<PdscRequirement> PdscCondition::requirements()
{
    QList<PdscRequirement> requirements;;// = m_requirements;
    combineRequirements(requirements, *this);
    return requirements;
}

//------------------------------------------------------------------------------
// Возвращает структурированную карту условий
//------------------------------------------------------------------------------
QMap<PdscRequirement::RequirementType, QMap<PdscRequirement::RequireTarget, QList<PdscRequirement> > > PdscCondition::requirementsMap()
{
    QList<PdscRequirement> requirements = this->requirements();
    QMap<PdscRequirement::RequirementType, QMap<PdscRequirement::RequireTarget, QList<PdscRequirement> > > map;

    foreach (PdscRequirement r, requirements)
    {
        if(map.contains(r.type()))
        {
            QMap<PdscRequirement::RequireTarget, QList<PdscRequirement> >& nestedMap = map[r.type()];
            QList<PdscRequirement>& nestedList = nestedMap[r.target()];
            nestedList.append(r);
        }
        else
        {
            QList<PdscRequirement> nestedList;
            QMap<PdscRequirement::RequireTarget, QList<PdscRequirement> > nestedMap;
            nestedList.append(r);
            nestedMap.insert(r.target(), nestedList);
            map.insert(r.type(), nestedMap);
        }
    }

    return map;
}

bool PdscCondition::isNull()
{
    return requirements().isEmpty();
}

void PdscCondition::addRequirement(const PdscRequirement &requirement)
{
    m_requirements.append(requirement);
}

//------------------------------------------------------------------------------
// Объединение требований вложенных Condition
//------------------------------------------------------------------------------
void PdscCondition::combineRequirements(QList<PdscRequirement> &requirements,
                                        const PdscCondition &condition)
{
#if 1
    for(auto r : condition.currRequirements())
    {
        if(!requirements.contains(r))
        {
            requirements.append(r);
        }
    }
#else
    requirements.append(condition.currRequirements());
#endif

    if(!condition.nestedConditions().isEmpty())
    {
        foreach (PdscCondition nested, condition.nestedConditions())
        {
            combineRequirements(requirements, nested);
        }
    }
}

//------------------------------------------------------------------------------
// Требования только текущего Condition без вложенностей
//------------------------------------------------------------------------------
QList<PdscRequirement> PdscCondition::currRequirements() const
{
    return m_requirements;
}


