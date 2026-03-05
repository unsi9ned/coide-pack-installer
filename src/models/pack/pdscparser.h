#ifndef PDSCPARSER_H
#define PDSCPARSER_H

#include <QString>
#include <QDomDocument>
#include "packdescription.h"
#include "models/pdsc/pdsccomponent.h"

class PdscParser
{
private:
    struct ParentComponentInfo
    {
        QString Cclass;
        QString Cgroup;
        QString Cversion;

        ParentComponentInfo() = default;

        ParentComponentInfo(const QString& Cclass,
                            const QString& Cgroup,
                            const QString& Cversion = QString())
        {
            this->Cclass = Cclass;
            this->Cgroup = Cgroup;
            this->Cversion = Cversion;
        }

        operator==(const ParentComponentInfo& parent)
        {
            return this->Cclass == parent.Cclass &&
                   this->Cgroup == parent.Cgroup &&
                   this->Cversion == parent.Cversion;
        }
    };
private:
    QString _lastErrorStr;

public:
    PdscParser();

    bool parse(PackDescription& pack);

private:
    void parseDomDocument(QDomDocument * doc, PackDescription& pack);
    void parseDevFamilies(const QDomNode& node, PackDescription& pack);
    Mcu& parseDevice(const QDomNode& deviceNode,
                     const QString& vendor,
                     const QString& processor,
                     const QString& series,
                     PackDescription& pack);
    DeviceFeature parseFeature(const QDomElement &featureElem);
    ProgAlgorithm parseAlgorithm(const QDomElement &algorithmElement);
    PdscCondition parseCondition(const QDomNode &condition);
    PdscRequirement parseRequirement(const QDomNode &requireNode);
    PdscComponent parseComponent(const QDomNode& componentNode,
                                 const QList<PdscCondition>& conditionList);
    PdscFile parseFile(const QDomNode& fileNode,
                       const QList<PdscCondition>& conditionList);
    void loadComponents(const QList<PdscComponent>& componentList, PackDescription& pack);
    void loadComponents(QMap<QString, Component>& coComponentMap,
                        QMap<QString, QList<ParentComponentInfo> >& parentComponentInfoMap,
                        const QList<PdscComponent>& componentList,
                        PackDescription& pack);

    void linkComponents(const QMap<QString, Component>& coComponentMap,
                        const QMap<QString, QList<ParentComponentInfo> >& parentComponentInfoMap,
                        PackDescription& pack);

    bool checkRequirements(const PackDescription& pack,
                           const Manufacturer& vendor,
                           const Family& family,
                           const Series& series,
                           const Mcu& device,
                           const PdscComponent& component,
                           const QList<PdscComponent> &componentList);

    bool checkRequirements(const PackDescription& pack,
                           const PdscComponent& component,
                           const QList<PdscComponent> &componentList);

    QStringList getFilteredFiles(const PackDescription& pack,
                                 const Manufacturer& vendor,
                                 const Family& family,
                                 const Series& series,
                                 const Mcu& device,
                                 PdscComponent& component,
                                 const QList<PdscComponent> &componentList);

    void updateParentComponentMap(QMap<QString, QList<ParentComponentInfo> >& parentComponentInfoMap,
                                  QString componentUuid,
                                  ParentComponentInfo parentInfo);
    QList<Component*> findParentsComponent(const QMap<QString, Component>& coComponentMap,
                                           const ParentComponentInfo& parent);
};

#endif // PDSCPARSER_H
