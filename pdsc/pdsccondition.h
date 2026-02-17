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

public:
    PdscCondition();
    QString id() const;
    void setId(const QString &id);
    QString description() const;
    void setDescription(const QString &description);
};

#endif // PDSCCONDITION_H
