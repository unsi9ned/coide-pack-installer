#ifndef DEVICEHIERARCHYNODE_H
#define DEVICEHIERARCHYNODE_H

#include <QString>

class DeviceHierarchyNode
{
public:
    explicit DeviceHierarchyNode() = default;
    virtual ~DeviceHierarchyNode() = default;

    virtual QString getPath() const { return QString(); }
};

#endif // DEVICEHIERARCHYNODE_H
