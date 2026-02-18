#include "pdscrequirement.h"

void PdscRequirement::setDname(const QString &Dname)
{
    m_Dname = Dname;

    if(!Dname.isEmpty())
        m_target = PdscRequirement::Device;
}

void PdscRequirement::setDfamily(const QString &Dfamily)
{
    m_Dfamily = Dfamily;

    if(!Dfamily.isEmpty())
        m_target = PdscRequirement::Device;
}

void PdscRequirement::setDsubFamily(const QString &DsubFamily)
{
    m_DsubFamily = DsubFamily;

    if(!DsubFamily.isEmpty())
        m_target = PdscRequirement::Device;
}

void PdscRequirement::setDcore(const QString &Dcore)
{
    m_Dcore = Dcore;

    if(!Dcore.isEmpty())
        m_target = PdscRequirement::Device;
}

void PdscRequirement::setDfpu(const QString &Dfpu)
{
    m_Dfpu = Dfpu;

    if(!Dfpu.isEmpty())
        m_target = PdscRequirement::Device;
}

void PdscRequirement::setDmpu(const QString &Dmpu)
{
    m_Dmpu = Dmpu;

    if(!Dmpu.isEmpty())
        m_target = PdscRequirement::Device;
}

void PdscRequirement::setDdsp(const QString &Ddsp)
{
    m_Ddsp = Ddsp;

    if(!Ddsp.isEmpty())
        m_target = PdscRequirement::Device;
}

void PdscRequirement::setDvendor(const QString &Dvendor)
{
    m_Dvendor = Dvendor;

    if(!Dvendor.isEmpty())
        m_target = PdscRequirement::Device;
}

void PdscRequirement::setDvariant(const QString &Dvariant)
{
    m_Dvariant = Dvariant;

    if(!Dvariant.isEmpty())
        m_target = PdscRequirement::Device;
}

bool PdscRequirement::isDeviceRequirement()
{
    return m_target == PdscRequirement::Device;
}

bool PdscRequirement::isComponentRequirement()
{
    return m_target == PdscRequirement::Component;
}

bool PdscRequirement::isCompilerRequirement()
{
    return m_target == PdscRequirement::Compiler;
}

bool PdscRequirement::isPackRequirement()
{
    return m_target == PdscRequirement::Pack;
}

void PdscRequirement::setCclass(const QString &Cclass)
{
    m_Cclass = Cclass;

    if(!Cclass.isEmpty())
        m_target = PdscRequirement::Component;
}

void PdscRequirement::setCgroup(const QString &Cgroup)
{
    m_Cgroup = Cgroup;

    if(!Cgroup.isEmpty())
        m_target = PdscRequirement::Component;
}

void PdscRequirement::setCsub(const QString &Csub)
{
    m_Csub = Csub;

    if(!Csub.isEmpty())
        m_target = PdscRequirement::Component;
}

void PdscRequirement::setCvariant(const QString &Cvariant)
{
    m_Cvariant = Cvariant;

    if(!Cvariant.isEmpty())
        m_target = PdscRequirement::Component;
}

void PdscRequirement::setCversion(const QString &Cversion)
{
    m_Cversion = Cversion;

    if(!Cversion.isEmpty())
        m_target = PdscRequirement::Component;
}

void PdscRequirement::setTcompiler(const QString &Tcompiler)
{
    m_Tcompiler = Tcompiler;

    if(!Tcompiler.isEmpty())
        m_target = PdscRequirement::Compiler;
}

void PdscRequirement::setTversion(const QString &Tversion)
{
    m_Tversion = Tversion;

    if(!Tversion.isEmpty())
        m_target = PdscRequirement::Compiler;
}

void PdscRequirement::setPvendor(const QString &Pvendor)
{
    m_Pvendor = Pvendor;

    if(!Pvendor.isEmpty())
        m_target = PdscRequirement::Pack;
}

void PdscRequirement::setPname(const QString &Pname)
{
    m_Pname = Pname;

    if(!Pname.isEmpty())
        m_target = PdscRequirement::Pack;
}

void PdscRequirement::setPversion(const QString &Pversion)
{
    m_Pversion = Pversion;

    if(!Pversion.isEmpty())
        m_target = PdscRequirement::Pack;
}

PdscRequirement::RequirementType PdscRequirement::type() const
{
    return m_type;
}

PdscRequirement::RequireTarget PdscRequirement::target() const
{
    return m_target;
}

QString PdscRequirement::Dname() const
{
    return m_Dname;
}

QString PdscRequirement::Dfamily() const
{
    return m_Dfamily;
}

QString PdscRequirement::DsubFamily() const
{
    return m_DsubFamily;
}

QString PdscRequirement::Dcore() const
{
    return m_Dcore;
}

QString PdscRequirement::Dfpu() const
{
    return m_Dfpu;
}

QString PdscRequirement::Dmpu() const
{
    return m_Dmpu;
}

QString PdscRequirement::Ddsp() const
{
    return m_Ddsp;
}

QString PdscRequirement::Dvendor() const
{
    return m_Dvendor;
}

QString PdscRequirement::Dvariant() const
{
    return m_Dvariant;
}

QString PdscRequirement::Cclass() const
{
    return m_Cclass;
}

QString PdscRequirement::Cgroup() const
{
    return m_Cgroup;
}

QString PdscRequirement::Csub() const
{
    return m_Csub;
}

QString PdscRequirement::Cvariant() const
{
    return m_Cvariant;
}

QString PdscRequirement::Cversion() const
{
    return m_Cversion;
}

QString PdscRequirement::Tcompiler() const
{
    return m_Tcompiler;
}

QString PdscRequirement::Tversion() const
{
    return m_Tversion;
}

QString PdscRequirement::Pvendor() const
{
    return m_Pvendor;
}

QString PdscRequirement::Pname() const
{
    return m_Pname;
}

QString PdscRequirement::Pversion() const
{
    return m_Pversion;
}

bool PdscRequirement::isValid()
{
    bool valid = true;

    switch(m_target)
    {
        case PdscRequirement::Component:
            valid = (!m_Cclass.isEmpty() && !m_Cgroup.isEmpty());
        break;

        case PdscRequirement::Pack:
            valid = (!m_Pvendor.isEmpty() && !m_Pname.isEmpty() && !m_Pversion.isEmpty());
        break;

        case PdscRequirement::Compiler:
            valid = (!m_Tcompiler.isEmpty());
        break;

        case PdscRequirement::Device:
            valid = (!m_Dname.isEmpty());
        break;

        default:
            valid = true;
        break;
    }

    return valid;
}

void PdscRequirement::setType(const RequirementType &type)
{
    m_type = type;
}

PdscRequirement::PdscRequirement(RequirementType type) : PdscElement()
{
    m_type = type;
    m_target = PdscRequirement::Unknown;
}
