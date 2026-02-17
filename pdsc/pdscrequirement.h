#ifndef PDSCREQUIREMENT_H
#define PDSCREQUIREMENT_H

#include <QString>
#include <QList>
#include "pdscelement.h"

class PdscCondition;

class PdscRequirement : public PdscElement
{

public:
    enum RequirementType
    {
        Require,  // AND - все должны быть true
        Accept,   // OR - хотя бы один true
        Deny      // NOT - ни одного true
    };

private:

    // Device requirements
    QString m_Dname;
    QString m_Dfamily;
    QString m_DsubFamily;
    QString m_Dcore;
    QString m_Dfpu;
    QString m_Dmpu;
    QString m_Ddsp;
    QString m_Dvendor;
    QString m_Dvariant;

    // Component requirements
    QString m_Cclass;
    QString m_Cgroup;
    QString m_Csub;
    QString m_Cvariant;
    QString m_Cversion;

    // Compiler requirements
    QString m_Tcompiler;
    QString m_Tversion;

    // Pack requirements
    QString m_Pvendor;
    QString m_Pname;
    QString m_Pversion;

    // Reference to another condition
    QList<PdscCondition*> m_conditions;

public:
    PdscRequirement();
};

#endif // PDSCREQUIREMENT_H
