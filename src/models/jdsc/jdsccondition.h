#ifndef JDSCCONDITION_H
#define JDSCCONDITION_H

#include <QString>
#include <QStringList>

class JdscCondition
{
private:
    QString m_id;
    QString m_description;
    QStringList m_devices;
public:
    JdscCondition(const QString& id = QString());

    QString id() const;
    QString description() const;
    QStringList supportedDevices() const;

    void setId(const QString& id);
    void setDescription(const QString& description);
    void addSupportedDevice(const QString& device);
    void addSupportedDevices(const QStringList& devices);
};

#endif // JDSCCONDITION_H
