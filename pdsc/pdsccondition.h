#ifndef PDSCCONDITION_H
#define PDSCCONDITION_H

#include <QString>
#include "pdscelement.h"

class PdscCondition : public PdscElement
{
public:
    enum RequirementType
    {
        Require,  // AND - все должны быть true
        Accept,   // OR - хотя бы один true
        Deny      // NOT - ни одного true
    };

    struct Requirement
    {
        RequirementType type;

        // Device requirements
        QString Dname;
        QString Dfamily;
        QString DsubFamily;
        QString Dcore;
        QString Dfpu;
        QString Dmpu;
        QString Ddsp;
        QString Dvendor;
        QString Dvariant;

        // Component requirements
        QString Cclass;
        QString Cgroup;
        QString Csub;
        QString Cvariant;
        QString Cversion;

        // Compiler requirements
        QString Tcompiler;
        QString Tversion;

        // Pack requirements
        QString Pvendor;
        QString Pname;
        QString Pversion;

        // Reference to another condition
        QString condition;

        //bool matches(const DeviceContext& context) const;
    };

private:
    QString m_id;
    QString m_description;

public:
    PdscCondition();
    QString id() const;
    void setId(const QString &id);
    QString description() const;
    void setDescription(const QString &description);
};

#endif // PDSCCONDITION_H
