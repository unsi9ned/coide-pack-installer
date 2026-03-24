#include "componentsinfo.h"
#include "database.h"
#include "models/mcu/manufacturer.h"

#define USE_UNIQUE_ID 1

QMap<int, Component> *ComponentsInfo::components()
{
    return &this->componentsMap;
}

QMap<int, Category> *ComponentsInfo::categories()
{
    return &this->categoriesMap;
}

QMap<int, Category> *ComponentsInfo::subcategories()
{
    return &this->subcategoriesMap;
}

//------------------------------------------------------------------------------
// Запросить список категорий
//------------------------------------------------------------------------------
QMap<int, Category> ComponentsInfo::requestCategoryMap()
{
    QMap<int, Category> categoriesMap;
    QSqlQuery result = DataBase::instance()->sendQuery("SELECT * FROM category");

    while(result.next())
    {
        Category newCategory;

        newCategory.setId(result.value(0).toInt());
        newCategory.setName(result.value(1).toString());

        categoriesMap.insert(newCategory.getId(), newCategory);
    }

    return categoriesMap;
}

//------------------------------------------------------------------------------
// Запросить список субкатегорий
//------------------------------------------------------------------------------
QMap<int, Category> ComponentsInfo::requestSubcategoryMap()
{
    QMap<int, Category> subcategoriesMap;
    QSqlQuery result = DataBase::instance()->sendQuery("SELECT * FROM subcategory");

    while(result.next())
    {
        Category newCategory;

        newCategory.setId(result.value(0).toInt());
        newCategory.setName(result.value(1).toString());
        newCategory.setParentId(result.value(2).toInt());

        subcategoriesMap.insert(newCategory.getId(), newCategory);
    }

    return subcategoriesMap;
}

//------------------------------------------------------------------------------
// Привести индентификаторы вендора в таблице `component_supports_mcumanufacturer`
// к стандарту Keil
//------------------------------------------------------------------------------
bool ComponentsInfo::fixComponentManufacturerTable(QString &errorString)
{
    auto components = requestComponentMap();

    for(auto it = components.begin(); it != components.end(); ++it)
    {
        Component c = it.value();
        QList<int> mList = c.getMcuManufacturerList();

        // Если все ID обновлены, то пропускаем
        bool needUpdate = false;

        foreach (int m, mList)
        {
            if(m <= 1000)
            {
                needUpdate = true;
                break;
            }
        }

        if(!needUpdate) continue;

        // Временно удаляем пару Компонент-Производитель из базы
        if(!deleteManufacturerList(c.getId(), errorString)) return false;

        // Нечего добавлять в базу
        if(mList.isEmpty()) continue;

        // Исправляем ID внутри каждого компонента
        for(int i = 0; i < mList.count(); i++)
        {
            if(!addManufacturer(c.getId(), Manufacturer(mList[i]).toKeilId(), errorString))
                return false;
        }
    }

    return true;
}

bool ComponentsInfo::fixComponentManufacturerTable(QString *errorString)
{
    QString e;

    if(errorString)
        return fixComponentManufacturerTable(*errorString);
    else
        return fixComponentManufacturerTable(e);
}

//------------------------------------------------------------------------------
// Удаляет связи между компонентами в таблице component_depends_component
//------------------------------------------------------------------------------
bool ComponentsInfo::removeComponentsRelation(int parentId, int childId, QString * errorString)
{
    bool status = true;
    QString sql = QString("DELETE FROM component_depends_component "
                          "WHERE parentComponentId = '%1' "
                          "AND childComponentId = '%2';").
                  arg(parentId).
                  arg(childId);
    QSqlQuery result = DataBase::instance()->sendQuery(sql, &status);

    if(!status)
    {
        if(errorString)
            *errorString = result.lastError().text();
        return false;
    }

    return true;
}

//------------------------------------------------------------------------------
// Удаляет связи между компонентами в таблице component_depends_component
// key = parentId,
// value = childId
//------------------------------------------------------------------------------
bool ComponentsInfo::removeComponentsRelation(QMap<int, int> pairs, QString *errorString)
{
    for(auto it = pairs.begin(); it != pairs.end(); ++it)
    {
        if(!removeComponentsRelation(it.key(), it.value(), errorString))
            return false;
    }

    return true;
}

//------------------------------------------------------------------------------
// Удаляет связи между компонентами и примерами в таблице example_depends_component
//------------------------------------------------------------------------------
bool ComponentsInfo::removeExampleRelation(int exampleId, int componentId, QString *errorString)
{
    bool status = true;
    QString sql = QString("DELETE FROM example_depends_component "
                          "WHERE exampleId = '%1' "
                          "AND componentId = '%2';").
                  arg(exampleId).
                  arg(componentId);
    QSqlQuery result = DataBase::instance()->sendQuery(sql, &status);

    if(!status)
    {
        if(errorString)
            *errorString = result.lastError().text();
        return false;
    }

    return true;
}

//------------------------------------------------------------------------------
// Удаляет связи между компонентами и примерами в таблице example_depends_component
// key = exampleId,
// value = componentId
//------------------------------------------------------------------------------
bool ComponentsInfo::removeExampleRelation(QMap<int, int> pairs, QString *errorString)
{
    for(auto it = pairs.begin(); it != pairs.end(); ++it)
    {
        if(!removeExampleRelation(it.key(), it.value(), errorString))
            return false;
    }

    return true;
}

//------------------------------------------------------------------------------
// Удаляет из базы данных информацию о компоненте
//------------------------------------------------------------------------------
bool ComponentsInfo::removeComponent(int componentId, QString *errorString)
{
    bool status = true;
    QString sql = QString("DELETE FROM component WHERE id = '%1';").arg(componentId);
    QSqlQuery result = DataBase::instance()->sendQuery(sql, &status);

    if(!status)
    {
        if(errorString)
            *errorString = result.lastError().text();
        return false;
    }

    QStringList tables =
    {
        "component_depends_coxinterface",
        "component_has_category",
        "component_has_example",
        "component_has_keyword",
        "component_has_subcategory",
        "component_implements_coxinterface",
        "component_supports_externaldevice",
        "component_supports_mcu",
        "component_supports_mcufamily",
        "component_supports_mcumanufacturer",
        "component_supports_mcuseries",
        "component_uses_communicationinterface",
    };

    foreach(QString t, tables)
    {
        sql = QString("DELETE FROM `%1` WHERE componentId = '%2';").arg(t).arg(componentId);
        result = DataBase::instance()->sendQuery(sql, &status);

        if(!status)
        {
            if(errorString)
                *errorString = result.lastError().text();
            return false;
        }
    }

    return true;
}

bool ComponentsInfo::removeComponent(const Component &component, QString *errorString)
{
    if(removeComponent(component.getId(), errorString))
        if(removeComponentStatus(component.getComponentStatusId(), errorString))
            return true;

    return false;
}

//------------------------------------------------------------------------------
// Удаляет из базы данных информацию о примере
//------------------------------------------------------------------------------
bool ComponentsInfo::removeExample(int exampleId, QString *errorString)
{
    bool status = true;
    QString sql = QString("DELETE FROM example WHERE id = '%1';").arg(exampleId);
    QSqlQuery result = DataBase::instance()->sendQuery(sql, &status);

    if(!status)
    {
        if(errorString)
            *errorString = result.lastError().text();
        return false;
    }

    QStringList tables =
    {
        "board_has_example",
        "component_has_example",
    };

    foreach(QString t, tables)
    {
        sql = QString("DELETE FROM `%1` WHERE exampleId = '%2';").arg(t).arg(exampleId);
        result = DataBase::instance()->sendQuery(sql, &status);

        if(!status)
        {
            if(errorString)
                *errorString = result.lastError().text();
            return false;
        }
    }

    return true;
}

bool ComponentsInfo::removeExample(const Example &example, QString *errorString)
{
    if(removeExample(example.getId(), errorString))
        if(removeComponentStatus(example.getStatusId(), errorString))
            return true;

    return false;
}

//------------------------------------------------------------------------------
// Удаляет из базы данных информацию о нескольких компонентах
//------------------------------------------------------------------------------
bool ComponentsInfo::removeComponents(QVector<int> componentIds, QString *errorString)
{
    QStringList idList;

    foreach(int id, componentIds)
    {
        idList << QString::number(id, 10);
    }

    bool status = true;
    QString sql = QString("DELETE FROM component WHERE id IN (%1);").arg(idList.join(", "));
    QSqlQuery result = DataBase::instance()->sendQuery(sql, &status);

    if(!status)
    {
        if(errorString)
            *errorString = result.lastError().text();
        return false;
    }

    QStringList tables =
    {
        "component_depends_coxinterface",
        "component_has_category",
        "component_has_example",
        "component_has_keyword",
        "component_has_subcategory",
        "component_implements_coxinterface",
        "component_supports_externaldevice",
        "component_supports_mcu",
        "component_supports_mcufamily",
        "component_supports_mcumanufacturer",
        "component_supports_mcuseries",
        "component_uses_communicationinterface",
    };

    foreach(QString t, tables)
    {
        sql = QString("DELETE FROM `%1` WHERE componentId IN (%2);").arg(t).arg(idList.join(", "));
        result = DataBase::instance()->sendQuery(sql, &status);

        if(!status)
        {
            if(errorString)
                *errorString = result.lastError().text();
            return false;
        }
    }

    return true;
}

//------------------------------------------------------------------------------
// Удаляет статус компонента из БД
//------------------------------------------------------------------------------
bool ComponentsInfo::removeComponentStatus(int statusId, QString *errorString)
{
    bool status = true;
    QString sql = QString("DELETE FROM status WHERE id = %1;").arg(statusId);
    QSqlQuery result = DataBase::instance()->sendQuery(sql, &status);

    if(!status)
    {
        if(errorString)
            *errorString = result.lastError().text();
        return false;
    }

    return true;
}

//------------------------------------------------------------------------------
// Удаляет несколько статусов компонентов из БД
//------------------------------------------------------------------------------
bool ComponentsInfo::removeComponentStatuses(QVector<int> statusIdList, QString *errorString)
{
    QStringList idList;

    foreach(int id, statusIdList)
    {
        idList << QString::number(id, 10);
    }

    bool status = true;
    QString sql = QString("DELETE FROM status WHERE id IN (%1);").arg(idList.join(", "));
    QSqlQuery result = DataBase::instance()->sendQuery(sql, &status);

    if(!status)
    {
        if(errorString)
            *errorString = result.lastError().text();
        return false;
    }

    return true;
}

//------------------------------------------------------------------------------
// Удаление фантомных связей между компонентами
//------------------------------------------------------------------------------
bool ComponentsInfo::removeComponentPhantomRelations(QString *errorString)
{
    bool status = false;
    QString sql = QString("SELECT parentComponentId, childComponentId "
                          "FROM component_depends_component "
                          "WHERE parentComponentId NOT IN (SELECT id FROM component) "
                          "OR childComponentId NOT IN (SELECT id FROM component);");
    QSqlQuery result = DataBase::instance()->sendQuery(sql, &status);

    if(!status)
    {
        if(errorString)
            *errorString = result.lastError().text();
        return false;
    }

    QMap<int, int> pairs;

    while(result.next())
    {
        pairs.insert(result.value("parentComponentId").toInt(),
                     result.value("childComponentId").toInt());
    }

    return removeComponentsRelation(pairs, errorString);
}

//------------------------------------------------------------------------------
// Удаление фантомных связей между примерами и компонентами
//------------------------------------------------------------------------------
bool ComponentsInfo::removeExamplePhantomRelations(QString *errorString)
{
    bool status = false;
    QString sql = QString("SELECT exampleId, componentId "
                          "FROM example_depends_component "
                          "WHERE exampleId NOT IN (SELECT id FROM example) "
                          "OR componentId NOT IN (SELECT id FROM component);");
    QSqlQuery result = DataBase::instance()->sendQuery(sql, &status);

    if(!status)
    {
        if(errorString)
            *errorString = result.lastError().text();
        return false;
    }

    QMap<int, int> pairs;

    while(result.next())
    {
        pairs.insert(result.value("exampleId").toInt(),
                     result.value("componentId").toInt());
    }

    return removeExampleRelation(pairs, errorString);
}

//------------------------------------------------------------------------------
// Обновить поля статуса компонента в БД
//------------------------------------------------------------------------------
bool ComponentsInfo::updateComponentStatus(int statusId,
                                           const Component::ComponentStatus &upd,
                                           QString *errorString)
{
    bool status;
    QString queryStr = QString("UPDATE status SET "
                               "id = '%1', "
                               "shouldupdate = '%2', "
                               "hasdownloaded = '%3', "
                               "hasdeleted = '%4', "
                               "auditstatus = '%5' "
                               "WHERE id = %1;").
                       arg(statusId).
                       arg(upd.shouldUpdate).
                       arg(upd.hasDownloaded).
                       arg(upd.hasDeleted).
                       arg(upd.auditStatus);

    QSqlQuery result = DataBase::instance()->sendQuery(queryStr, &status);

    if(!status)
    {
        if(errorString)
            *errorString = result.lastError().text();
        return false;
    }

    return true;
}

bool ComponentsInfo::setComponentStatusOK(int statusId, QString *errorString)
{
    Component::ComponentStatus upd;
    upd.statusId = statusId;
    upd.shouldUpdate = 0;
    upd.hasDownloaded = 1;
    upd.hasDeleted = 0;
    upd.auditStatus = 1;

    return updateComponentStatus(statusId, upd, errorString);
}

bool ComponentsInfo::removeStatusPhantomRelations(QString *errorString)
{
    bool status = false;
    QString sql = QString("SELECT id "
                          "FROM status "
                          "WHERE id NOT IN (SELECT component.Component_Status_id FROM component) "
                          "AND id NOT IN (SELECT example.statusId FROM example) "
                          "AND id NOT IN (SELECT document.Component_Status_id FROM document);");
    QSqlQuery result = DataBase::instance()->sendQuery(sql, &status);

    if(!status)
    {
        if(errorString)
            *errorString = result.lastError().text();
        return false;
    }

    QVector<int> statusIDs;

    while(result.next())
    {
        statusIDs.append(result.value("id").toInt());
    }

    return removeComponentStatuses(statusIDs, errorString);
}

//------------------------------------------------------------------------------
// Добавление компонента в базу данных
//------------------------------------------------------------------------------
bool ComponentsInfo::createComponent(Component &component, QString *errorString)
{
#define RECURSIVE_CREATION 0

#if RECURSIVE_CREATION
    // Сначала создаем компоненты, от которых зависим
    QList<Component *> children = component.getChildren();

    foreach (Component* child, children)
    {
        if(child && !createComponent(*child, errorString))
            return false;
    }
#endif

    Component foundComponent = findComponent(component);

    // Создаем новый
    if(foundComponent.isNull())
    {
        //Поиск последнего айди
        bool status = false;
#if !USE_UNIQUE_ID
        int lastId = -1;
        QString queryStr = QString("SELECT MAX(id) FROM component");
        QSqlQuery result = DataBase::instance()->sendQuery(queryStr, &status);

        if(!status || !result.next())
        {
            if(errorString)
                *errorString = result.lastError().text();
            return false;
        }
        else
        {
            lastId = result.value(0).toInt();
            component.setId(lastId + 1);
#else
        QString queryStr;
        QSqlQuery result;

        {
#endif
            // Создаем положительный статус компонента
            Component::ComponentStatus costatus = component.getStatus();

            if(!createComponentStatus(costatus, errorString))
                return false;
            else
                component.setStatus(costatus);

            queryStr = QString("INSERT INTO component "
                               "(id, authorId, Layer_id, Component_Status_id, Share_Document_id,"
                               "type, name, description, advertisingWord, advertisingURL, uuid,"
                               "timeuuid, repo_user, repo_password, micro, cox, version, publish_status,"
                               "hits, create_date, update_date, tags) "
                               "VALUES ('%1','%2','%3','%4','%5','%6','%7','%8','%9','%10','%11','%12',"
                               "'%13','%14','%15','%16','%17','%18','%19','%20','%21','%22')").
#if USE_UNIQUE_ID
                               arg(component.getUniqueId()).
#else
                               arg(component.getId()).
#endif
                               arg(component.getAuthorId()).
                               arg(component.getLayerId()).
                               arg(component.getComponentStatusId()).
                               arg(component.getShareDocumentId()).
                               arg(component.getType()).
                               arg(component.getName()).
                               arg(component.getDescription()).
                               arg(component.getAdvertisingWord()).
                               arg(component.getAdvertisingURL()).
                               arg(component.getUuid()).
                               arg(component.getTimeuuid()).
                               arg(component.getRepoUser()).
                               arg(component.getRepoPass()).
                               arg(component.getMicro()).
                               arg(component.getCox()).
                               arg(component.getVersion()).
                               arg(component.getPublishStatus()).
                               arg(component.getHits()).
                               arg(component.getCreationDate()).
                               arg(component.getUpdateDate()).
                               arg(component.getTags());

            result = DataBase::instance()->sendQuery(queryStr, &status);

            if(!status)
            {
                if(errorString)
                    *errorString = result.lastError().text();
                return false;
            }
#if USE_UNIQUE_ID
            else
            {
                component.setId(component.getUniqueId());
            }
#endif

            // Создаем категорию или находим ее, если она создана
            Category newCategory = findCategory(component.getCategory(), errorString);

            if(newCategory.isNull())
            {
                newCategory.setName(component.getCategory().getName());

                if(!createCategory(newCategory, errorString))
                    return false;
            }
            newCategory.setSubCategoryName(component.getCategory().getSubCategoryName());
            component.setCategory(newCategory);

            // Создаем подкатегорию (у нас она всегда есть)
            Category newSubCategory = findSubCategory(component.getCategory(), errorString);

            if(newSubCategory.getSubCategoryName().isEmpty())
            {
                newSubCategory.setId(newCategory.getId());
                newSubCategory.setSubCategoryName(component.getCategory().getSubCategoryName());

                if(!createSubCategory(newSubCategory, errorString))
                    return false;

                newCategory.setSubCategoryId(newSubCategory.getSubCategoryId());
            }
            else
            {
                newCategory.setSubCategoryId(newSubCategory.getSubCategoryId());
                newCategory.setSubCategoryName(newSubCategory.getSubCategoryName());
            }

            component.setCategory(newCategory);

            // Создаем связь между компонентом и устройствами
            foreach(QString dev, component.supportedMcuList())
            {
                int mcuId = -1;

                if(hasComponentMcuLink(component.getId(), dev, &mcuId, &status, errorString))
                {
                    continue;
                }
                else if(!status)
                {
                    return false;
                }
                else if(!createComponentMcuLink(component.getId(), dev, errorString))
                    return false;
            }

            // Создаем связь между компонентом и категорией
            int catId = -1;

            if(!hasComponentCategoryLink(component.getId(), component.getCategory().getName(), &catId, &status, errorString))
            {
                if(!createComponentCategoryLink(component.getId(), component.getCategory().getName(), errorString))
                    return false;
            }
            else if(!status)
                return false;

            // Создаем связь между компонентом и подкатегорией
            if(!hasComponentSubCategoryLink(component.getId(),
                                            component.getCategory().getName(),
                                            component.getCategory().getSubCategoryName(),
                                            &status,
                                            errorString))
            {
                if(!createComponentSubCategoryLink(component.getId(),
                                                   component.getCategory().getName(),
                                                   component.getCategory().getSubCategoryName(),
                                                   errorString))
                {
                    return false;
                }
            }
            else if(!status)
                return false;

            // Создаем связь между компонентами
            if(component.hasChildren())
            {
                foreach (Component* child, component.getChildren())
                {
#if RECURSIVE_CREATION
                    int childId = child->getId();
#else
                    int childId = child->getUniqueId();
#endif
                    bool hasLink = hasComponentsLink(childId, component.getId(), &status, errorString);

                    if(status && !hasLink)
                    {
                        if(!createComponentsLink(childId, component.getId(), errorString))
                            return false;
                    }
                    else if(!status)
                        return false;
                }
            }
        }
    }
#if 0
    // Обновляем существующий
    else if(!updateComponent(component, errorString))
    {
        return false;
    }
#else
    else
    {
        bool status = false;

        // Обновляем ID существующего в БД компонента
        if(component.isNull())
            component.setId(foundComponent.getId());

        // Создаем связи между существующими и новыми компонентами
        foreach (Component* child, component.getChildren())
        {
#if RECURSIVE_CREATION
            int childId = child->getId();
#else
            int childId = child->getUniqueId();
#endif
            bool hasLink = hasComponentsLink(childId, component.getId(), &status, errorString);

            if(status && !hasLink)
            {
                if(!createComponentsLink(childId, component.getId(), errorString))
                    return false;
            }
            else if(!status)
                return false;
        }

        // Помечаем, что компонент не требует установки
        component.setPersisted(true);
    }
#endif

    return true;

#undef RECURSIVE_CREATION
}

//------------------------------------------------------------------------------
// Обновление компонента в базе данных
//------------------------------------------------------------------------------
bool ComponentsInfo::updateComponent(Component &component, QString *errorString)
{
    return false;
}

//------------------------------------------------------------------------------
// Создать запись в таблице status
//------------------------------------------------------------------------------
bool ComponentsInfo::createComponentStatus(Component::ComponentStatus &status, QString *errorString)
{
    //Поиск последнего айди
    int lastId = -1;
    bool opstatus = false;
    QString queryStr = QString("SELECT MAX(id) FROM status");
    QSqlQuery result = DataBase::instance()->sendQuery(queryStr, &opstatus);

    if(!opstatus || !result.next())
    {
        if(errorString)
            *errorString = result.lastError().text();
        return false;
    }
    else
    {
        lastId = result.value(0).toInt();
        status.statusId = lastId + 1;

        queryStr = QString("INSERT INTO status ("
                           "id, shouldupdate, hasdownloaded, hasdeleted, auditstatus"
                           ") VALUES ("
                           "'%1', '%2', '%3', '%4', '%5'"
                           ");").
                   arg(status.statusId).
                   arg(status.shouldUpdate).
                   arg(status.hasDownloaded).
                   arg(status.hasDeleted).
                   arg(status.auditStatus);

        result = DataBase::instance()->sendQuery(queryStr, &opstatus);

        if(!opstatus)
        {
            if(errorString)
                *errorString = result.lastError().text();
            return false;
        }
    }

    return true;
}

//------------------------------------------------------------------------------
// Создать категорию компонента
//------------------------------------------------------------------------------
bool ComponentsInfo::createCategory(Category &category, QString *errorString)
{
    //Поиск последнего айди
    int lastId = -1;
    bool opstatus = false;
    QString queryStr = QString("SELECT MAX(id) FROM category");
    QSqlQuery result = DataBase::instance()->sendQuery(queryStr, &opstatus);

    if(!opstatus || !result.next())
    {
        if(errorString)
            *errorString = result.lastError().text();
        return false;
    }
    else
    {
        lastId = result.value(0).toInt();
        category.setId(lastId + 1);
        queryStr = QString("INSERT INTO category ("
                           "id, name "
                           ") VALUES ("
                           "'%1', '%2'"
                           ");").
                   arg(category.getId()).
                   arg(category.getName());

        result = DataBase::instance()->sendQuery(queryStr, &opstatus);

        if(!opstatus)
        {
            if(errorString)
                *errorString = result.lastError().text();
            return false;
        }
    }

    return true;
}

//------------------------------------------------------------------------------
// Создать подкатегорию компонента
//------------------------------------------------------------------------------
bool ComponentsInfo::createSubCategory(Category &category, QString *errorString)
{
    //Поиск последнего айди
    int lastId = -1;
    bool opstatus = false;
    QString queryStr = QString("SELECT MAX(id) FROM subcategory");
    QSqlQuery result = DataBase::instance()->sendQuery(queryStr, &opstatus);

    if(!opstatus || !result.next())
    {
        if(errorString)
            *errorString = result.lastError().text();
        return false;
    }
    else
    {
        lastId = result.value(0).toInt();
        category.setSubCategoryId(lastId + 1);

        queryStr = QString("INSERT INTO subcategory ("
                           "id, name, categoryId "
                           ") VALUES ("
                           "'%1', '%2', '%3'"
                           ");").
                   arg(category.getSubCategoryId()).
                   arg(category.getSubCategoryName()).
                   arg(category.getId());

        result = DataBase::instance()->sendQuery(queryStr, &opstatus);

        if(!opstatus)
        {
            if(errorString)
                *errorString = result.lastError().text();
            return false;
        }
    }

    return true;
}

//------------------------------------------------------------------------------
// Проверить связь компонента с MCU
//------------------------------------------------------------------------------
bool ComponentsInfo::hasComponentMcuLink(int componentId,
                                         const QString &mcuName,
                                         int *mcuId, bool *status,
                                         QString *errorString)
{
    bool opstatus = false;
    QString sql;

    sql = QString("SELECT "
                  "component_supports_mcu.componentId, "
                  "component.name AS componentName, "
                  "component_supports_mcu.mcuId, "
                  "mcu.name AS mcuName "
                  "FROM component_supports_mcu "
                  "INNER JOIN mcu ON mcu.id = component_supports_mcu.mcuId "
                  "INNER JOIN component ON component.id = component_supports_mcu.componentId "
                  "WHERE mcu.name = '%1' AND component_supports_mcu.componentId = '%2';").
                  arg(mcuName.toUpper()).
                  arg(componentId);

    QSqlQuery result = DataBase::instance()->sendQuery(sql, &opstatus);

    if(!opstatus)
    {
        if(errorString)
            *errorString = result.lastError().text();
        if(status)
            *status = false;
        return false;
    }
    else
    {
        opstatus = false;

        if(status)
            *status = true;

        while (result.next())
        {
#if 0
            int componentId = result.value("componentId").toInt();
            QString componentName = result.value("componentName").toString();
#endif
            if(mcuId)
                *mcuId = result.value("mcuId").toInt();
            opstatus = true;
            break;
        }
    }

    return opstatus;
}

//------------------------------------------------------------------------------
// Связать компонент с MCU
//------------------------------------------------------------------------------
bool ComponentsInfo::createComponentMcuLink(int componentId, const QString &mcuName, QString *errorString)
{
    //Поиск последнего айди
    bool opstatus = false;
    QString queryStr = QString("INSERT INTO component_supports_mcu ("
                               "componentId, mcuId "
                               ") VALUES ("
                               "'%1', (SELECT id FROM mcu WHERE name = '%2' LIMIT 1)"
                               ");").
                       arg(componentId).
                       arg(mcuName.toUpper());

    QSqlQuery result = DataBase::instance()->sendQuery(queryStr, &opstatus);

    if(!opstatus)
    {
        if(errorString)
            *errorString = result.lastError().text();
        return false;
    }

    return true;
}

//------------------------------------------------------------------------------
// Проверить принадлежности компонента категории
//------------------------------------------------------------------------------
bool ComponentsInfo::hasComponentCategoryLink(int componentId,
                                              const QString &categoryName,
                                              int *categoryId,
                                              bool *status,
                                              QString *errorString)
{
    bool opstatus = false;
    QString sql;

    sql = QString("SELECT "
                  "component_has_category.componentId, "
                  "component_has_category.categoryId "
                  "FROM component_has_category "
                  "WHERE component_has_category.categoryId = (SELECT id FROM category WHERE name = '%1' LIMIT 1) "
                  "AND component_has_category.componentId = '%2' LIMIT 1;").
                  arg(categoryName).
                  arg(componentId);

    QSqlQuery result = DataBase::instance()->sendQuery(sql, &opstatus);

    if(!opstatus)
    {
        if(errorString)
            *errorString = result.lastError().text();
        if(status)
            *status = false;
        return false;
    }
    else
    {
        opstatus = false;

        if(status)
            *status = true;

        while (result.next())
        {
#if 0
            int componentId = result.value("componentId").toInt();
            QString componentName = result.value("componentName").toString();
#endif
            if(categoryId)
                *categoryId = result.value("categoryId").toInt();
            opstatus = true;
            break;
        }
    }

    return opstatus;
}

//------------------------------------------------------------------------------
// Проверить принадлежности компонента подкатегории
//------------------------------------------------------------------------------
bool ComponentsInfo::createComponentCategoryLink(int componentId,
                                                 const QString &categoryName,
                                                 QString *errorString)
{
    //Поиск последнего айди
    bool opstatus = false;
    QString queryStr = QString("INSERT INTO component_has_category ("
                               "componentId, categoryId "
                               ") VALUES ("
                               "'%1', (SELECT id FROM category WHERE name = '%2' LIMIT 1)"
                               ");").
                       arg(componentId).
                       arg(categoryName);

    QSqlQuery result = DataBase::instance()->sendQuery(queryStr, &opstatus);

    if(!opstatus)
    {
        if(errorString)
            *errorString = result.lastError().text();
        return false;
    }

    return true;
}

//------------------------------------------------------------------------------
// Проверить принадлежности компонента подкатегории
//------------------------------------------------------------------------------
bool ComponentsInfo::hasComponentSubCategoryLink(int componentId,
                                                 const QString &categoryName,
                                                 const QString &subCategoryName,
                                                 bool *status,
                                                 QString *errorString)
{
    bool opstatus = false;
    QString sql;

    sql = QString("SELECT "
                  "component_has_subcategory.componentId, "
                  "component_has_subcategory.subcategoryId "
                  "FROM component_has_subcategory "
                  "WHERE component_has_subcategory.subcategoryId = "
                    "(SELECT id FROM subcategory WHERE name = '%1' AND categoryId = "
                        "(SELECT id FROM category WHERE name = '%3' LIMIT 1) LIMIT 1) "
                  "AND component_has_subcategory.componentId = '%2' LIMIT 1;").
                  arg(subCategoryName).
                  arg(componentId).
                  arg(categoryName);

    QSqlQuery result = DataBase::instance()->sendQuery(sql, &opstatus);

    if(!opstatus)
    {
        if(errorString)
            *errorString = result.lastError().text();
        if(status)
            *status = false;
        return false;
    }
    else
    {
        opstatus = false;

        if(status)
            *status = true;

        while (result.next())
        {
            opstatus = true;
            break;
        }
    }

    return opstatus;
}

//------------------------------------------------------------------------------
// Связать компонент с подкатегорией
//------------------------------------------------------------------------------
bool ComponentsInfo::createComponentSubCategoryLink(int componentId,
                                                    const QString &categoryName,
                                                    const QString &subCategoryName,
                                                    QString *errorString)
{
    //Поиск последнего айди
    bool opstatus = false;
    QString queryStr = QString("INSERT INTO component_has_subcategory ("
                               "componentId, subcategoryId "
                               ") VALUES ("
                               "'%1', (SELECT id FROM subcategory WHERE name = '%2' "
                                      "AND categoryId = "
                                      "(SELECT id FROM category WHERE name = '%3' LIMIT 1) LIMIT 1)"
                               ");").
                       arg(componentId).
                       arg(subCategoryName).
                       arg(categoryName);

    QSqlQuery result = DataBase::instance()->sendQuery(queryStr, &opstatus);

    if(!opstatus)
    {
        if(errorString)
            *errorString = result.lastError().text();
        return false;
    }

    return true;
}

//------------------------------------------------------------------------------
// Проверить наличие связи между компонентами
//------------------------------------------------------------------------------
bool ComponentsInfo::hasComponentsLink(int parentId,
                                       int childId,
                                       bool *status,
                                       QString *errorString)
{
    bool opstatus = false;
    QString sql;

    sql = QString("SELECT "
                  "component_depends_component.parentComponentId, "
                  "component_depends_component.childComponentId "
                  "FROM component_depends_component "
                  "WHERE component_depends_component.parentComponentId = '%1' "
                  "AND component_depends_component.childComponentId = '%2';").
                  arg(parentId).
                  arg(childId);

    QSqlQuery result = DataBase::instance()->sendQuery(sql, &opstatus);

    if(!opstatus)
    {
        if(errorString)
            *errorString = result.lastError().text();
        if(status)
            *status = false;
        return false;
    }
    else
    {
        opstatus = false;

        if(status)
            *status = true;

        while (result.next())
        {
            opstatus = true;
            break;
        }
    }

    return opstatus;
}

//------------------------------------------------------------------------------
// Связать компоненты между собой
//------------------------------------------------------------------------------
bool ComponentsInfo::createComponentsLink(int parentId,
                                          int childId,
                                          QString *errorString)
{
    //Поиск последнего айди
    bool opstatus = false;
    QString queryStr = QString("INSERT INTO component_depends_component ("
                               "parentComponentId, childComponentId "
                               ") VALUES ("
                               "'%1', '%2'"
                               ");").
                       arg(parentId).
                       arg(childId);

    QSqlQuery result = DataBase::instance()->sendQuery(queryStr, &opstatus);

    if(!opstatus)
    {
        if(errorString)
            *errorString = result.lastError().text();
        return false;
    }

    return true;
}

//------------------------------------------------------------------------------
// Удаляет из базы данных информацию о производителях, к которым применим компонент
//------------------------------------------------------------------------------
bool ComponentsInfo::deleteManufacturerList(int componentId, QString &errorString)
{
    bool status;
    QString queryStr = QString("DELETE FROM component_supports_mcumanufacturer "
                               "WHERE componentId = '%1';").arg(componentId);
    QSqlQuery result = DataBase::instance()->sendQuery(queryStr, &status);

    if(!status)
    {
        errorString = result.lastError().text();
    }

    return status;
}

//------------------------------------------------------------------------------
// Связать компонент с производителем чипа
//------------------------------------------------------------------------------
bool ComponentsInfo::addManufacturer(int componentId, int vendorId, QString &errorString)
{
    bool status;
    QString queryStr = QString("INSERT INTO component_supports_mcumanufacturer "
                               "VALUES ('%1','%2')").
                        arg(componentId).
                        arg(vendorId);

    QSqlQuery result = DataBase::instance()->sendQuery(queryStr, &status);

    if(!status)
    {
        errorString = result.lastError().text();
    }

    return status;
}

//------------------------------------------------------------------------------
// Обновить список производителей, которые имеют доступ к компоненту
//------------------------------------------------------------------------------
bool ComponentsInfo::updateManufacturerList(const Component &component, QString& errorString)
{
    if(!deleteManufacturerList(component.getId(), errorString))
        return false;

    foreach (int vendorId, component.getMcuManufacturerList())
    {
        if(!addManufacturer(component.getId(), vendorId, errorString))
            return false;
    }

    return true;
}

//------------------------------------------------------------------------------
// Поиск компонента в базе данных по косвенным признакам
//------------------------------------------------------------------------------
Component ComponentsInfo::findComponent(const Component component, QString *errorString)
{
    bool status = true;
    Component foundComponent;
    QList<Component> foundComponentList;
    QString sql;

    sql = QString("SELECT "
                    "component.id, "
                    "component.type, "
                    "component.Layer_id AS layerId, "
                    "component_has_category.categoryId, "
                    "category.name AS category, "
                    "COALESCE(component_has_subcategory.subcategoryId, -1) AS subcategoryId, "
                    "COALESCE(subcategory.name, NULL) AS subcategory, "
                    "component.name, "
                    "component.description, "
                    "component.uuid, "
                    "component.version, "
                    "component_supports_mcu.mcuId, "
                    "mcu.name AS mcu, "
                    "mcuseries.seriesName AS series, "
                    "mcufamily.familyName AS family, "
                    "mcumanufacturer.name AS vendor "
                "FROM component "
                "INNER JOIN component_supports_mcu ON component.id = component_supports_mcu.componentId "
                "INNER JOIN mcu ON mcu.id = component_supports_mcu.mcuId "
                "INNER JOIN mcuseries ON mcuseries.id = mcu.seriesId "
                "INNER JOIN mcufamily ON mcufamily.id = mcuseries.familyId "
                "INNER JOIN mcumanufacturer ON mcumanufacturer.id = mcufamily.manufacturerId "
                "INNER JOIN component_has_category ON component_has_category.componentId = component.id "
                "INNER JOIN category ON category.id = component_has_category.categoryId "
                "LEFT JOIN component_has_subcategory ON component_has_subcategory.componentId = component.id "
                "LEFT JOIN subcategory ON subcategory.id = component_has_subcategory.subcategoryId "
                "WHERE component.name = '%1' "
                "AND component.version = '%2';").
            arg(component.getName()).
            arg(component.getVersion());

    QSqlQuery result = DataBase::instance()->sendQuery(sql, &status);

    if(!status)
    {
        if(errorString)
            *errorString = result.lastError().text();
        return foundComponent;
    }
    else
    {
        while (result.next())
        {
            int componentId = result.value("id").toInt();
            int type = result.value("type").toInt();
            int layerId = result.value("layerId").toInt();
            int categoryId = result.value("categoryId").toInt();
            QString category = result.value("category").toString();
            int subCategoryId = result.value("subcategoryId").toInt();
            QString subCategory = result.value("subcategory").toString();
            QString name = result.value("name").toString();
            QString description = result.value("description").toString();
            QString uuid = result.value("uuid").toString();
            QString version = result.value("version").toString();
            int mcuId = result.value("mcuId").toInt();
            QString mcu = result.value("mcu").toString();
#if 0
            QString series = result.value("series").toString();
            QString family = result.value("family").toString();
            QString vendor = result.value("vendor").toString();
#endif

            Component current;
            Category foundCategory;

            foundCategory.setId(categoryId);
            foundCategory.setName(category);
            foundCategory.setSubCategoryId(subCategoryId);
            foundCategory.setSubCategoryName(subCategory);

            current.setId(componentId);
            current.setType(type);
            current.setLayerId(layerId);
            current.setCategory(foundCategory);
            current.setName(name);
            current.setDescription(description);
            current.setUuid(uuid);
            current.setVersion(version);
            current.addSupportedMcu(mcu);
            current.appendMcuId(mcuId);

            if(foundComponentList.contains(current))
            {
                for(int i = 0; i < foundComponentList.count(); i++)
                {
                    Component& c = foundComponentList[i];

                    // Обновляем список mcu
                    if(c == current)
                    {
                        if(!c.supportedMcuList().contains(mcu, Qt::CaseInsensitive))
                        {
                            c.addSupportedMcu(mcu);
                            c.appendMcuId(mcuId);
                        }
                        break;
                    }
                }
            }
            else
                foundComponentList.append(current);
        }
    }

    if(!foundComponentList.isEmpty())
    {
        foreach (Component c, foundComponentList)
        {
            foreach (QString mcuName, component.supportedMcuList())
            {
                if(c.supportedMcuList().contains(mcuName, Qt::CaseInsensitive))
                {
                    foundComponent = c;
                }
            }
        }
    }

    return foundComponent;
}

//------------------------------------------------------------------------------
// Поиск категории в базе данных
//------------------------------------------------------------------------------
Category ComponentsInfo::findCategory(const Category category, QString *errorString)
{
    bool status = true;
    Category foundCategory;
    QString sql;

    sql = QString("SELECT "
                    "category.id, "
                    "category.name "
                  "FROM category "
                  "WHERE category.name = '%1' "
                  "LIMIT 1;").
                  arg(category.getName());


    QSqlQuery result = DataBase::instance()->sendQuery(sql, &status);

    if(!status)
    {
        if(errorString)
            *errorString = result.lastError().text();
        return foundCategory;
    }
    else
    {
        while (result.next())
        {
            int categoryId = result.value("id").toInt();
            QString categoryName = result.value("name").toString();

            foundCategory.setId(categoryId);
            foundCategory.setName(categoryName);
        }
    }

    return foundCategory;
}

//------------------------------------------------------------------------------
// Поиск подкатегории в базе данных
//------------------------------------------------------------------------------
Category ComponentsInfo::findSubCategory(const Category category,
                                         QString *errorString)
{
    Category foundSubCategory;
    bool status = true;

    QString sql;

    sql = QString("SELECT "
                    "subcategory.id, "
                    "subcategory.name, "
                    "subcategory.categoryId "
                  "FROM subcategory "
                  "WHERE subcategory.name = '%1' AND subcategory.categoryId = '%2' "
                  "LIMIT 1;").
                  arg(category.getSubCategoryName()).
                  arg(category.getId());


    QSqlQuery result = DataBase::instance()->sendQuery(sql, &status);

    if(!status)
    {
        if(errorString)
            *errorString = result.lastError().text();
        return foundSubCategory;
    }
    else
    {
        while (result.next())
        {
            int id = result.value("id").toInt();
            QString name = result.value("name").toString();

            foundSubCategory = category;
            foundSubCategory.setSubCategoryId(id);
            foundSubCategory.setSubCategoryName(name);
        }
    }

    return foundSubCategory;
}

//------------------------------------------------------------------------------
// Загрузить данные о компонентах из базы данных
//------------------------------------------------------------------------------
QMap<int, Component> ComponentsInfo::requestComponentMap()
{
    QMap<int, Component> componentsMap;
    QString sql = QString("SELECT component.*, "
                          "status.shouldupdate, "
                          "status.hasdownloaded, "
                          "status.hasdeleted, "
                          "status.auditstatus "
                          "FROM component, status "
                          "WHERE component.Component_Status_id = status.id;");

    QSqlQuery result = DataBase::instance()->sendQuery(sql);

    while(result.next())
    {
        Component newComponent;

        newComponent.setId(result.value(0).toInt());
        newComponent.setAuthorId(result.value(1).toInt());
        newComponent.setLayerId(result.value(2).toInt());
        newComponent.setComponentStatusId(result.value(3).toInt());
        newComponent.setShareDocumentId(result.value(4).toInt());
        newComponent.setType(result.value(5).toInt());
        newComponent.setName(result.value(6).toString());
        newComponent.setDescription(result.value(7).toString());
        newComponent.setAdvertisingWord(result.value(8).toString());
        newComponent.setAdvertisingURL(result.value(9).toString());
        newComponent.setUuid(result.value(10).toString());
        newComponent.setTimeuuid(result.value(11).toString());
        newComponent.setRepoUser(result.value(12).toString());
        newComponent.setRepoPass(result.value(13).toString());
        newComponent.setMicro(result.value(14).toString());
        newComponent.setCox(result.value(15).toInt());
        newComponent.setVersion(result.value(16).toString());
        newComponent.setPublishStatus(result.value(17).toString());
        newComponent.setHits(result.value(18).toInt());
        newComponent.setCreationDate(result.value(19).toString());
        newComponent.setUpdateDate(result.value(20).toString());
        newComponent.setTags(result.value(21).toString());

        //Чтение статуса компонента
        Component::ComponentStatus status;

        status.statusId = result.value(3).toInt();
        status.shouldUpdate = result.value(22).toInt();
        status.hasDownloaded = result.value(23).toInt();
        status.hasDeleted = result.value(24).toInt();
        status.auditStatus = result.value(25).toInt();
        newComponent.setStatus(status);

        componentsMap.insert(newComponent.getId(), newComponent);
    }

    //Установка связи между компонентами
    result = DataBase::instance()->sendQuery("SELECT * FROM component_depends_component");

    while(result.next())
    {
        int parentComponentId = result.value(0).toInt();
        int childComponentId = result.value(1).toInt();
        Component* parentComponent = nullptr, *childComponent = nullptr;

        if(componentsMap.contains(parentComponentId) &&
           componentsMap.contains(childComponentId))
        {
            parentComponent = &componentsMap[parentComponentId];
            childComponent = &componentsMap[childComponentId];
            parentComponent->addChild(childComponent);
        }
    }

    //Загрузка списка процессоров для каждого компонента
    result = DataBase::instance()->sendQuery("SELECT * FROM component_supports_mcu");

    while(result.next())
    {
        int componentId = result.value(0).toInt();
        int mcuId = result.value(1).toInt();

        Component * component = &componentsMap[componentId];
        component->appendMcuId(mcuId);
    }

    //Загрузка списка семейств процессоров для каждого компонента
    result = DataBase::instance()->sendQuery("SELECT * FROM component_supports_mcufamily");

    while(result.next())
    {
        int componentId = result.value(0).toInt();
        int familyId = result.value(1).toInt();

        Component * component = &componentsMap[componentId];
        component->appendMcuFamily(familyId);
    }

    //Загрузка списка серий процессоров для каждого компонента
    result = DataBase::instance()->sendQuery("SELECT * FROM component_supports_mcuseries");

    while(result.next())
    {
        int componentId = result.value(0).toInt();
        int serieId = result.value(1).toInt();

        Component * component = &componentsMap[componentId];
        component->appendMcuSerie(serieId);
    }

    //Загрузка списка производителей процессоров для каждого компонента
    result = DataBase::instance()->sendQuery("SELECT * FROM component_supports_mcumanufacturer");

    while(result.next())
    {
        int componentId = result.value(0).toInt();
        int manufacturerId = result.value(1).toInt();

        Component * component = &componentsMap[componentId];
        component->appendMcuManufacturer(manufacturerId);
    }

    return componentsMap;
}

//------------------------------------------------------------------------------
// Загрузить данные о примерах из базы данных
//------------------------------------------------------------------------------
QMap<int, Example> ComponentsInfo::requestExampleMap()
{
    QMap<int, Example> exampleMap;
    QString sql = QString("SELECT example.*, "
                          "status.shouldupdate, "
                          "status.hasdownloaded, "
                          "status.hasdeleted, "
                          "status.auditstatus "
                          "FROM example, status "
                          "WHERE example.statusId = status.id;");

    QSqlQuery result = DataBase::instance()->sendQuery(sql);

    while(result.next())
    {
        Example newExample;

        newExample.setId(result.value("id").toInt());
        newExample.setUserId(result.value("userId").toInt());
        newExample.setStatusId(result.value("statusId").toInt());
        newExample.setName(result.value("name").toString());
        newExample.setDescription(result.value("description").toString());
        newExample.setType(result.value("type").toInt());
        newExample.setUuid(result.value("uuid").toString());
        newExample.setTimeuuid(result.value("timeuuid").toString());
        newExample.setRepoUser(result.value("repo_user").toString());
        newExample.setRepoPassword(result.value("repo_password").toString());
        newExample.setCreateDate(result.value("create_date").toString());
        newExample.setUpdateDate(result.value("update_date").toString());
        newExample.setHits(result.value("hits").toInt());

        //Чтение статуса компонента
        Component::ComponentStatus status;

        status.statusId = result.value("statusId").toInt();
        status.shouldUpdate = result.value("shouldupdate").toInt();
        status.hasDownloaded = result.value("hasdownloaded").toInt();
        status.hasDeleted = result.value("hasdeleted").toInt();
        status.auditStatus = result.value("auditstatus").toInt();
        newExample.setStatus(status);

        exampleMap.insert(newExample.getId(), newExample);
    }

    //Установка связи между компонентами и примерами
    result = DataBase::instance()->sendQuery("SELECT * FROM example_depends_component");

    while(result.next())
    {
        int exampleId = result.value(0).toInt();
        int componentId = result.value(1).toInt();

        // В таблице могут быть ID не существующих примеров
        if(exampleMap.contains(exampleId))
        {
            exampleMap[exampleId].addParentComponent(componentId);
        }
    }

    return exampleMap;
}

//------------------------------------------------------------------------------
// Загрузить данные о компонентах из базы данных
//------------------------------------------------------------------------------
void ComponentsInfo::loadDataFromDb()
{
    this->componentsMap = requestComponentMap();
    this->categoriesMap = requestCategoryMap();
    this->subcategoriesMap = requestSubcategoryMap();

    //Установка связи между категориями и подкатегориями
    QMap<int, Category>::iterator subIterator = this->subcategoriesMap.begin();

    while(subIterator != this->subcategoriesMap.end())
    {
        int currKey = subIterator.key();
        Category * currCategory = &this->subcategoriesMap[currKey];
        int parentId = currCategory->getParentId();

        if(parentId >= 0 && this->categoriesMap.contains(parentId))
        {
            this->categoriesMap[parentId].appendChild(currCategory);
        }

        ++subIterator;
    }
}
