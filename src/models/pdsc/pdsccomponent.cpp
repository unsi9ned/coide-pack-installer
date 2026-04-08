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
// Возвращает список производителей, для которых создан компонент
//------------------------------------------------------------------------------
QStringList PdscComponent::supportVendors()
{
    QStringList vendors;

    if(!hasCondition()) return QStringList();

    auto requirementMap = condition().requirementsMap();
    QList<PdscRequirement::RequirementType> requireTypes =
    {
        PdscRequirement::Require,
        PdscRequirement::Accept
    };

    foreach(auto rType, requireTypes)
    {
        foreach(PdscRequirement r, requirementMap[rType].value(PdscRequirement::Device))
        {
            if(!r.isValid())
                continue;

            QString vendorName = (r.Dvendor().split(':') << "").first();

            if(!vendors.contains(vendorName, Qt::CaseInsensitive))
                vendors.append(vendorName);
        }
    }

    return vendors;
}

void PdscComponent::addDefSymbol(const QString& symbol)
{
    if(!m_definedSymbols.contains(symbol))
        m_definedSymbols.append(symbol);
}

QStringList PdscComponent::definedSymbols() const
{
    return m_definedSymbols;
}

PdscComponent::PdscComponent() : PdscElement()
{

}
