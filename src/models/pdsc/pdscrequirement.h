#ifndef PDSCREQUIREMENT_H
#define PDSCREQUIREMENT_H

#include <QString>
#include <QList>
#include "pdscelement.h"

class PdscRequirement : public PdscElement
{

public:
    enum RequirementType
    {
        Require,  // AND - все должны быть true
        Accept,   // OR - хотя бы один true
        Deny      // NOT - ни одного true
    };

    enum RequireTarget
    {
        Unknown,
        Device,      // Dname, Dcore, Dfamily, Dvendor...
        Component,   // Cclass, Cgroup, Csub...
        Compiler,    // Tcompiler, Tversion
        Pack         // Pvendor, Pname, Pversion
    };

private:
    RequirementType m_type;
    RequireTarget m_target;

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

public:
    PdscRequirement(RequirementType type = Require);

    bool isDeviceRequirement();
    bool isComponentRequirement();
    bool isCompilerRequirement();
    bool isPackRequirement();

    void setDname(const QString &Dname);
    void setDfamily(const QString &Dfamily);
    void setDsubFamily(const QString &DsubFamily);
    void setDcore(const QString &Dcore);
    void setDfpu(const QString &Dfpu);
    void setDmpu(const QString &Dmpu);
    void setDdsp(const QString &Ddsp);
    void setDvendor(const QString &Dvendor);
    void setDvariant(const QString &Dvariant);

    void setCclass(const QString &Cclass);
    void setCgroup(const QString &Cgroup);
    void setCsub(const QString &Csub);
    void setCvariant(const QString &Cvariant);
    void setCversion(const QString &Cversion);

    void setTcompiler(const QString &Tcompiler);
    void setTversion(const QString &Tversion);

    void setPvendor(const QString &Pvendor);
    void setPname(const QString &Pname);
    void setPversion(const QString &Pversion);

    void setType(const RequirementType &type);
    RequirementType type() const;
    RequireTarget target() const;

    QString Dname() const;
    QString Dfamily() const;
    QString DsubFamily() const;
    QString Dcore() const;
    QString Dfpu() const;
    QString Dmpu() const;
    QString Ddsp() const;
    QString Dvendor() const;
    QString Dvariant() const;

    QString Cclass() const;
    QString Cgroup() const;
    QString Csub() const;
    QString Cvariant() const;
    QString Cversion() const;

    QString Tcompiler() const;
    QString Tversion() const;

    QString Pvendor() const;
    QString Pname() const;
    QString Pversion() const;

    bool isValid();

    bool operator==(const PdscRequirement& require);

};

#endif // PDSCREQUIREMENT_H
