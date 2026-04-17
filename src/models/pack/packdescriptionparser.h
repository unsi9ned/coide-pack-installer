#ifndef PACKDESCRIPTIONPARSER_H
#define PACKDESCRIPTIONPARSER_H

#include "packdescription.h"
#include "models/components/component.h"

class PackDescriptionParser
{
protected:
    struct ParentComponentInfo
    {
        QString Jname;
        QString Jversion;
        QString Cclass;
        QString Cgroup;
        QString Cvariant;
        QString Cversion;

        ParentComponentInfo() = default;

        ParentComponentInfo(const QString& jname, const QString& jversion = QString())
        {
            this->Jname = jname;
            this->Jversion = jversion;
        }

        ParentComponentInfo(const QString& Cclass,
                            const QString& Cgroup,
                            const QString& Cversion = QString(),
                            const QString& Cvariant = QString())
        {
            this->Cclass = Cclass;
            this->Cgroup = Cgroup;
            this->Cvariant = Cvariant;
            this->Cversion = Cversion;
        }

        operator==(const ParentComponentInfo& parent)
        {
            return this->Jname == parent.Jname &&
                   this->Jversion == parent.Jversion &&
                   this->Cclass == parent.Cclass &&
                   this->Cgroup == parent.Cgroup &&
                   this->Cversion == parent.Cversion &&
                   this->Cvariant == parent.Cvariant;
        }
    };
public:
    PackDescriptionParser() = default;
    virtual ~PackDescriptionParser() = default;

    virtual bool parse(PackDescription& pack) = 0;
    virtual void reloadComponents(PackDescription& pack) = 0;

protected:

    virtual void linkComponents(const QMap<QString, Component>& coComponentMap,
                                const QMap<QString, QList<ParentComponentInfo> >& parentComponentInfoMap,
                                PackDescription& pack);

    virtual void updateParentComponentMap(QMap<QString, QList<ParentComponentInfo> >& parentComponentInfoMap,
                                          QString componentUuid,
                                          ParentComponentInfo newParent);

    virtual QList<Component*> findParentsComponent(const QMap<QString, Component>& coComponentMap,
                                                   const ParentComponentInfo& parent,
                                                   const Component& child) = 0;
};

#endif // PACKDESCRIPTIONPARSER_H
