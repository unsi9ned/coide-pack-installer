#ifndef DEVICEHIERARCHYNODE_H
#define DEVICEHIERARCHYNODE_H

#include <QString>
#include <QByteArray>
#include <QDebug>

class DeviceHierarchyNode
{
public:
    explicit DeviceHierarchyNode() = default;
    virtual ~DeviceHierarchyNode() = default;

    virtual qint32 getUniqueId() const
    {
        QString path = getPath();
        qint32 offset = getCoMaxId();

        if(path.isEmpty())
            return -1;
        else
        {
            qint32 mcuId = -1;
            qint32 counter = 1;

            while(mcuId <= offset && counter < INT32_MAX)
            {
                qInfo() << path;
                mcuId = (qChecksum(path.toUtf8(), path.toUtf8().length()) & 0x7FFFFFFF);
                path = QString("%1/%2").arg(getPath()).arg(counter++);
            }

            return mcuId <= 0 ? -1 : mcuId;
        }
    }
    virtual QString getPath() const { return QString(); }
    virtual qint32 getCoMaxId() const { return 0; }
};

#endif // DEVICEHIERARCHYNODE_H
