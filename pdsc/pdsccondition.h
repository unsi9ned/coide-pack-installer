#ifndef PDSCCONDITION_H
#define PDSCCONDITION_H

#include <QString>
#include <QList>
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
    QList<PdscCondition> conditions() const;
    QList<PdscRequirement> requirements() const;
    bool isNull();

    void setId(const QString &id);
    void setDescription(const QString &description);
    void addCondition(const PdscCondition& condition);
    void addRequirement(const PdscRequirement& requirement);
};

#endif // PDSCCONDITION_H
