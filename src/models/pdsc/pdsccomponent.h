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
    QList<PdscFile> m_files;

public:
    PdscComponent();

    QString description() const;
    PdscCondition condition() const;
    PdscComponentAttributes& attributes();
    QList<PdscFile>& files();
    bool hasCondition();

    void setDescription(const QString &description);
    void setCondition(const PdscCondition &condition);
    void setAttributes(const PdscComponentAttributes &attributes);

    QStringList supportVendors();

};

#endif // PDSCCOMPONENT_H
