#include "pdscrequirement.h"

void PdscRequirement::setDname(const QString &Dname)
{
    m_Dname = Dname;
    m_target = PdscRequirement::Device;
}

void PdscRequirement::setDfamily(const QString &Dfamily)
{
    m_Dfamily = Dfamily;
    m_target = PdscRequirement::Device;
}

void PdscRequirement::setDsubFamily(const QString &DsubFamily)
{
    m_DsubFamily = DsubFamily;
    m_target = PdscRequirement::Device;
}

void PdscRequirement::setDcore(const QString &Dcore)
{
    m_Dcore = Dcore;
    m_target = PdscRequirement::Device;
}

void PdscRequirement::setDfpu(const QString &Dfpu)
{
    m_Dfpu = Dfpu;
    m_target = PdscRequirement::Device;
}

void PdscRequirement::setDmpu(const QString &Dmpu)
{
    m_Dmpu = Dmpu;
    m_target = PdscRequirement::Device;
}

void PdscRequirement::setDdsp(const QString &Ddsp)
{
    m_Ddsp = Ddsp;
    m_target = PdscRequirement::Device;
}

void PdscRequirement::setDvendor(const QString &Dvendor)
{
    m_Dvendor = Dvendor;
    m_target = PdscRequirement::Device;
}

void PdscRequirement::setDvariant(const QString &Dvariant)
{
    m_Dvariant = Dvariant;
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
    m_target = PdscRequirement::Component;
}

void PdscRequirement::setCgroup(const QString &Cgroup)
{
    m_Cgroup = Cgroup;
    m_target = PdscRequirement::Component;
}

void PdscRequirement::setCsub(const QString &Csub)
{
    m_Csub = Csub;
    m_target = PdscRequirement::Component;
}

void PdscRequirement::setCvariant(const QString &Cvariant)
{
    m_Cvariant = Cvariant;
    m_target = PdscRequirement::Component;
}

void PdscRequirement::setCversion(const QString &Cversion)
{
    m_Cversion = Cversion;
    m_target = PdscRequirement::Component;
}

void PdscRequirement::setTcompiler(const QString &Tcompiler)
{
    m_Tcompiler = Tcompiler;
    m_target = PdscRequirement::Compiler;
}

void PdscRequirement::setTversion(const QString &Tversion)
{
    m_Tversion = Tversion;
    m_target = PdscRequirement::Compiler;
}

void PdscRequirement::setPvendor(const QString &Pvendor)
{
    m_Pvendor = Pvendor;
    m_target = PdscRequirement::Pack;
}

void PdscRequirement::setPname(const QString &Pname)
{
    m_Pname = Pname;
    m_target = PdscRequirement::Pack;
}

void PdscRequirement::setPversion(const QString &Pversion)
{
    m_Pversion = Pversion;
    m_target = PdscRequirement::Pack;
}

PdscRequirement::RequirementType PdscRequirement::type() const
{
    return m_type;
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

void PdscRequirement::setType(const RequirementType &type)
{
    m_type = type;
}

PdscRequirement::PdscRequirement(RequirementType type) : PdscElement()
{
    m_type = type;
    m_target = PdscRequirement::Unknown;
}
