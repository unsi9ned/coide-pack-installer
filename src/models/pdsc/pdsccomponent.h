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
    QStringList m_definedSymbols;

public:
    PdscComponent();

    QString description() const;
    PdscCondition condition() const;
    PdscComponentAttributes& attributes();
    const PdscComponentAttributes& attributesConst() const;
    QList<PdscFile>& files();
    const QList<PdscFile> filesConst() const;
    bool hasCondition() const;

    void setDescription(const QString &description);
    void setCondition(const PdscCondition &condition);
    void setAttributes(const PdscComponentAttributes &attributes);

    QStringList supportVendors();

    void addDefSymbol(const QString& symbol);
    QStringList definedSymbols() const;

};

#endif // PDSCCOMPONENT_H
