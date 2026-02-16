#include "componentsinfo.h"
#include "database.h"
#include "manufacturer.h"

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
                          "WHERE component.Component_Status_id == status.id;");
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
        newComponent.setCreateDate(result.value(19).toString());
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

        Component *parentComponent = &componentsMap[parentComponentId];
        Component *childComponent = &componentsMap[childComponentId];

        parentComponent->appendChild(childComponent);
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
