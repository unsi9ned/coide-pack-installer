#include "packdescriptionparser.h"

//------------------------------------------------------------------------------
// Установить связь между компонентами и сформировать окончательную карту
//------------------------------------------------------------------------------
void PackDescriptionParser::linkComponents(
        const QMap<QString, Component>& coComponentMap,
        const QMap<QString, QList<PackDescriptionParser::ParentComponentInfo> >& parentComponentInfoMap,
        PackDescription& pack)
{
    QMap<QString, Component>& componentMap = pack.coComponentMap();
    componentMap.unite(coComponentMap);

    for(auto it = componentMap.begin(); it != componentMap.end(); ++it)
    {
        QString uuid = it.key();
        Component& component = it.value();
        auto parents = parentComponentInfoMap.value(uuid);

        foreach(auto parentInfo, parents)
        {
            QList<Component*> parentComponents = findParentsComponent(componentMap, parentInfo, component);

            foreach(auto parent, parentComponents)
            {
                component.addParent(parent);
                parent->addChild(&component);
            }
        }
    }

    // Связывание компонентов CMSIS со всеми Mcu
    for(auto it = pack.cmsisComponents().begin(); it != pack.cmsisComponents().end(); ++it)
    {
        if(it.value() == nullptr)
            continue;

        Component * cmsisComponent = it.value();
        QString cmsisUuid = cmsisComponent->getUuid();

        for(auto cIt = componentMap.begin(); cIt != componentMap.end(); ++cIt)
        {
            if(cIt.key() == cmsisUuid)
                continue;

            cmsisComponent->addSupportedMcuList(cIt.value().supportedMcuList());
        }
    }
    return;
}

//------------------------------------------------------------------------------
// Добавляет в карту новую информацию о связи между компонентами
//------------------------------------------------------------------------------
void PackDescriptionParser::updateParentComponentMap(
        QMap<QString, QList<PackDescriptionParser::ParentComponentInfo> >& parentComponentInfoMap,
        QString componentUuid,
        PackDescriptionParser::ParentComponentInfo newParent)
{
    QList<ParentComponentInfo> * parentList = nullptr;

    if(parentComponentInfoMap.contains(componentUuid))
    {
        parentList = &parentComponentInfoMap[componentUuid];
    }
    else
    {
        parentComponentInfoMap.insert(componentUuid, QList<ParentComponentInfo>());
        parentList = &parentComponentInfoMap[componentUuid];
    }

    if(!parentList->contains(newParent))
        parentList->append(newParent);
}


