#include "pdsccomponent.h"

QString PdscComponent::description() const
{
    return m_description;
}

void PdscComponent::setDescription(const QString &description)
{
    m_description = description;
}

PdscComponent::PdscComponent() : PdscElement()
{

}
