#ifndef PDSCCOMPONENTATTREX_H
#define PDSCCOMPONENTATTREX_H

#include "pdsccomponentattr.h"

class PdscComponentAttributesEx : public PdscComponentAttributes
{
private:
    QString m_condition;    // optional

public:
    PdscComponentAttributesEx() : m_condition(QString()) {}
    PdscComponentAttributesEx(const QString& cclass,
                              const QString& cgroup,
                              const QString& cversion,
                              const QString& cvariant) : m_condition(QString())
    {
        m_cclass = cclass;
        m_cgroup = cgroup;
        m_cversion = cversion;
        m_cvariant = cvariant;
    }

    void setPdscCondition(const QString& condition) { m_condition = condition; }
    const QString getPdscCondition() const { return m_condition; }

    operator ==(const PdscComponentAttributesEx& attr) const
    {
        if(m_condition != attr.getPdscCondition()) return false;
        if(m_cvendor != attr.getCvendor()) return false;
        if(m_cbundle != attr.getCbundle()) return false;
        if(m_cclass != attr.getCclass()) return false;
        if(m_cgroup != attr.getCgroup()) return false;
        if(m_csub != attr.getCsub()) return false;
        if(m_cvariant != attr.getCvariant()) return false;
        if(m_cversion != attr.getCversion()) return false;
        if(m_capiversion != attr.getCapiversion()) return false;
        if(m_instances != attr.getInstances()) return false;

        return true;
    }

    operator !=(const PdscComponentAttributesEx& attr) const
    {
        bool compare = *this == attr;
        return !compare;
    }

    QString makeName() const
    {
        if(m_cclass.toUpper() == "CMSIS" && m_cgroup.toUpper() == "CORE")
        {
            return QString("CMSIS_CORE_%1").arg(m_cversion);
        }
        else if(!m_cvariant.isEmpty())
        {
            return QString("%1_%2_%3").arg(m_cgroup).arg(m_cvariant).arg(m_cversion);
        }
        else
        {
            return QString("%1_%2").arg(m_cgroup).arg(m_cversion);
        }
    }

    QString makePath() const
    {
        QString path = m_cvendor.isEmpty() ? "" : m_cvendor + "/";
        path += m_cclass.isEmpty() ? "" : m_cclass + "/";
        path += m_cgroup.isEmpty() ? "" : m_cgroup + "/";
        path += m_csub.isEmpty() ? "" : m_csub + "/";
        path += m_cvariant.isEmpty() ? "" : m_cvariant + "/";
        path += m_cversion.isEmpty() ? "" : m_cversion + "/";
        path += m_condition.isEmpty() ? "" : m_condition + "/";

        if(path.endsWith('/'))
            path.chop(1);

        return path;
    }
};

#endif // PDSCCOMPONENTATTREX_H
