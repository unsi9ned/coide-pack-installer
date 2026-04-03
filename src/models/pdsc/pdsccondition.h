#ifndef PDSCCONDITION_H
#define PDSCCONDITION_H

#include <QString>
#include <QList>
#include <QMap>
#include "pdscelement.h"
#include "pdscrequirement.h"

class PdscCondition : public PdscElement
{

private:
    QString m_id;
    QString m_description;
    QList<PdscRequirement> m_requirements;
    QList<PdscCondition> m_conditions;

public:
    PdscCondition();

    QString id() const;
    QString description() const;
    QList<PdscCondition> nestedConditions() const;
    QList<PdscRequirement> requirements();
    QMap<PdscRequirement::RequirementType, QMap<PdscRequirement::RequireTarget, QList<PdscRequirement> > > requirementsMap();
    bool isNull();

    void setId(const QString &id);
    void setDescription(const QString &description);
    void addCondition(const PdscCondition& condition);
    void addRequirement(const PdscRequirement& requirement);

private:
    void combineRequirements(QList<PdscRequirement>& requirements,
                             const PdscCondition& condition);

    QList<PdscRequirement> currRequirements() const;
};

#endif // PDSCCONDITION_H
