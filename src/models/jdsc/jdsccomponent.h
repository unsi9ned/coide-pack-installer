#ifndef JDSCCOMPONENT_H
#define JDSCCOMPONENT_H

#include <QList>
#include <QStringList>
#include <QMap>
#include "jdsccomponent.h"
#include "jdsccondition.h"

class JdscComponent
{
private:
    QString m_name;
    QString m_description;
    QString m_class;
    QString m_group;
    QString m_version;
    QStringList m_files;
    QMap<QString, JdscComponent> m_parents;
    QMap<QString, JdscCondition> m_conditions;
    QStringList m_defines;
    bool m_external;
public:
    JdscComponent() = default;
    JdscComponent(const QString& name, const QString& version = QString());
    QString name() const;
    QString description() const;
    QString cclass() const;
    QString group() const;
    QString version() const;
    QStringList files() const;
    QMap<QString, JdscComponent> parents() const;
    QMap<QString, JdscCondition> conditions() const;
    QStringList defines() const;
    bool isExternal() const;

    void setName(const QString& name);
    void setDescription(const QString& description);
    void setClass(const QString& cclass);
    void setGroup(const QString& group);
    void setVersion(const QString& version);
    void addFile(const QString& file);
    void addFiles(const QStringList& files);
    void addParent(JdscComponent parent);
    void addCondition(JdscCondition condition);
    void addDefine(const QString& define);
    void setExternal(bool external);

    bool supportDevice(const QString& devName);
};

#endif // JDSCCOMPONENT_H
