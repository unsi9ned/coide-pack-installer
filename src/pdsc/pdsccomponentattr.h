#ifndef PDSCCOMPONENTATTR_H
#define PDSCCOMPONENTATTR_H

#include <QString>
#include "pdscelement.h"

class PdscComponentAttributes : public PdscElement
{
private:
    QString m_cvendor;      // optional
    QString m_cbundle;      // optional
    QString m_cclass;       // required
    QString m_cgroup;       // optional
    QString m_csub;         // optional
    QString m_cvariant;     // optional
    QString m_cversion;     // optional
    QString m_capiversion;  // optional
    QString m_instances;    // optional, default "1"

public:

    // Конструкторы
    PdscComponentAttributes();

    // Геттеры
    QString getCvendor() const { return m_cvendor; }
    QString getCbundle() const { return m_cbundle; }
    QString getCclass() const { return m_cclass; }
    QString getCgroup() const { return m_cgroup; }
    QString getCsub() const { return m_csub; }
    QString getCvariant() const { return m_cvariant; }
    QString getCversion() const { return m_cversion; }
    QString getCapiversion() const { return m_capiversion; }
    QString getInstances() const { return m_instances; }

    // Сеттеры
    void setCvendor(const QString& value) { m_cvendor = value; }
    void setCbundle(const QString& value) { m_cbundle = value; }
    void setCclass(const QString& value) { m_cclass = value; }
    void setCgroup(const QString& value) { m_cgroup = value; }
    void setCsub(const QString& value) { m_csub = value; }
    void setCvariant(const QString& value) { m_cvariant = value; }
    void setCversion(const QString& value) { m_cversion = value; }
    void setCapiversion(const QString& value) { m_capiversion = value; }
    void setInstances(const QString& value) { m_instances = value; }

    // Проверка наличия значений (для optional полей)
    bool hasCvendor() const { return !m_cvendor.isEmpty(); }
    bool hasCbundle() const { return !m_cbundle.isEmpty(); }
    bool hasCgroup() const { return !m_cgroup.isEmpty(); }
    bool hasCsub() const { return !m_csub.isEmpty(); }
    bool hasCvariant() const { return !m_cvariant.isEmpty(); }
    bool hasCversion() const { return !m_cversion.isEmpty(); }
    bool hasCapiversion() const { return !m_capiversion.isEmpty(); }
};

#endif // PDSCCOMPONENTATTR_H
