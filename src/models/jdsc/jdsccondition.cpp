#include "jdsccondition.h"

JdscCondition::JdscCondition(const QString& id)
{
    if(!id.isEmpty())
        m_id = id;
}

QString JdscCondition::id() const
{
    return m_id;
}

QString JdscCondition::description() const
{
    return m_description;
}

QStringList JdscCondition::supportedDevices() const
{
    return m_devices;
}

void JdscCondition::setId(const QString& id)
{
    m_id = id;
}

void JdscCondition::setDescription(const QString& description)
{
    m_description = description;
}

void JdscCondition::addSupportedDevice(const QString& device)
{
    if(device.isEmpty()) return;

    if(!m_devices.contains(device, Qt::CaseInsensitive))
        m_devices.append(device);
}

void JdscCondition::addSupportedDevices(const QStringList& devices)
{
    for(const QString& dev : devices)
    {
        addSupportedDevice(dev);
    }
}
