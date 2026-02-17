#ifndef PDSCCOMPONENT_H
#define PDSCCOMPONENT_H

#include <QList>
#include "pdscfile.h"
#include "pdsccomponentattr.h"
#include "pdsccondition.h"

class PdscComponent : public PdscElement
{
private:
    PdscComponentAttributes m_attributes;
    PdscCondition m_condition;
    QString m_description;
    QList<PdscFile> files;

public:
    PdscComponent();
    QString description() const;
    void setDescription(const QString &description);
    PdscCondition condition() const;
    void setCondition(const PdscCondition &condition);
    PdscComponentAttributes attributes() const;
    void setAttributes(const PdscComponentAttributes &attributes);
};

#endif // PDSCCOMPONENT_H
