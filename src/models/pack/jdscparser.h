#ifndef JDSCPARSER_H
#define JDSCPARSER_H

#include "packdescription.h"
#include "packdescriptionparser.h"
#include "models/jdsc/jdsccondition.h"
#include "models/jdsc/jdsccomponent.h"

class JdscParser : public PackDescriptionParser
{
private:
    QString m_lastError;
public:
    JdscParser();

    bool parse(PackDescription& pack);
    void reloadComponents(PackDescription& pack);

private:
    QList<JdscCondition> loadConditions(const QJsonArray& conditions);
    QList<JdscComponent> loadComponents(const QJsonArray& components,
                                        const QList<JdscCondition>& conditions);
    QList<JdscComponent> linkNestedComponents(const QList<JdscComponent>& components);

    void loadComponents(QMap<QString, Component>& coComponentMap,
                        QMap<QString, QList<ParentComponentInfo> >& parentComponentInfoMap,
                        const QList<JdscComponent>& componentList,
                        PackDescription& pack);

    QStringList addMetaDataToFiles(const QStringList& files);

    QList<Component*> findParentsComponent(const QMap<QString, Component>& coComponentMap,
                                           const PackDescriptionParser::ParentComponentInfo& parent);
};

#endif // JDSCPARSER_H
