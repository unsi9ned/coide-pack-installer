#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include "jdscparser.h"

//------------------------------------------------------------------------------
// Конструктор
//------------------------------------------------------------------------------
JdscParser::JdscParser()
{

}

//------------------------------------------------------------------------------
// Разбор файла описания
//------------------------------------------------------------------------------
bool JdscParser::parse(PackDescription& pack)
{
    bool status = false;
    QFile jsonFile(pack.pathToPdsc());

    if(!jsonFile.exists())
    {
        m_lastError = QObject::tr("The Package Description File isn't found");
    }
    else if(!jsonFile.open(QFile::ReadOnly))
    {
        m_lastError = jsonFile.errorString();
    }
    else
    {
        QJsonParseError parseError;
        QJsonDocument doc = QJsonDocument::fromJson(jsonFile.readAll(), &parseError);

        if(doc.isNull())
        {
            m_lastError = parseError.errorString();
            status = false;
        }
        else
        {
            QJsonObject root = doc.object();
            QJsonArray conditions = root["conditions"].toArray();
            QJsonArray components = root["components"].toArray();

            pack.setVendor(root["vendor"].toString());
            pack.setName(root["name"].toString());
            pack.setUrl(root["url"].toString());
            pack.setDescription(root["description"].toString());
            pack.setRelease(root["version"].toString());

            auto jdscConditions = loadConditions(conditions);
            auto jdscComponents = loadComponents(components, jdscConditions);
            pack.jdscComponentList() = linkNestedComponents(jdscComponents);

            jsonFile.close();
            status = true;
        }
    }

    return status;
}

//------------------------------------------------------------------------------
// Перезагрузка компонентов
//------------------------------------------------------------------------------
void JdscParser::reloadComponents(PackDescription& pack)
{
    QMap<QString, Component> coComponentMap;
    QMap<QString, QList<PackDescriptionParser::ParentComponentInfo> > parentComponentInfoMap;

    loadComponents(coComponentMap, parentComponentInfoMap, pack.jdscComponentList(), pack);
    linkComponents(coComponentMap, parentComponentInfoMap, pack);
}

//------------------------------------------------------------------------------
// Загрузка условий
//------------------------------------------------------------------------------
QList<JdscCondition> JdscParser::loadConditions(const QJsonArray& conditions)
{
    QList<JdscCondition> jdscConditions;

    for (const auto& cond : conditions)
    {
        QJsonObject obj = cond.toObject();
        QString id = obj["id"].toString();
        QString description = obj["description"].toString();
        QStringList devices;

        for (const auto& dev : obj["devices"].toArray())
        {
            devices.append(dev.toString());
        }

        JdscCondition jdscCondition;
        jdscCondition.setId(id);
        jdscCondition.setDescription(description);
        jdscCondition.addSupportedDevices(devices);
        jdscConditions.append(jdscCondition);
    }

    return jdscConditions;
}

//------------------------------------------------------------------------------
// Загрузка списка компонентов
//------------------------------------------------------------------------------
QList<JdscComponent> JdscParser::loadComponents(const QJsonArray& components,
                                                const QList<JdscCondition>& conditions)
{
    QList<JdscComponent> jdscComponents;

    for (const auto& comp : components)
    {
        JdscComponent newComponent;
        QJsonObject obj = comp.toObject();

        QString name = obj["name"].toString();
        QString description = obj["description"].toString();
        QString cclass = obj["class"].toString();
        QString group = obj["group"].toString();
        QString version = obj["version"].toString();
        bool external = obj["external"].toBool();
        QStringList files;

        for (const auto& file : obj["files"].toArray())
        {
            files.append(file.toString());
        }

        // Загрузка зависимостей
        for (const auto& p : obj["requires"].toArray())
        {
            QJsonObject parent = p.toObject();
            newComponent.addParent(JdscComponent(parent["name"].toString(),
                                                 parent["version"].toString()));
        }

        // Загрузка defines
        for (const auto& define : obj["defines"].toArray())
        {
            newComponent.addDefine(define.toString());
        }

        // Поддержка устройств
        for (const auto& condition : conditions)
        {
            newComponent.addCondition(condition);
        }

        newComponent.setName(name);
        newComponent.setDescription(description);
        newComponent.setClass(cclass);
        newComponent.setGroup(group);
        newComponent.setVersion(version);
        newComponent.setExternal(external);
        newComponent.addFiles(files);

        jdscComponents.append(newComponent);
    }

    return jdscComponents;
}

//------------------------------------------------------------------------------
// Связь компонентов между собой
//------------------------------------------------------------------------------
QList<JdscComponent> JdscParser::linkNestedComponents(const QList<JdscComponent>& components)
{
    QList<JdscComponent> linkedComponents = components;

    for(auto& current : linkedComponents)
    {
        auto parentMap = current.parents();

        for(auto parent : parentMap)
        {
            for(const auto& component : components)
            {
                if(parent.name() == component.name() &&
                   parent.version() == component.version())
                {
                    current.addParent(component);
                }
            }
        }
    }

    return linkedComponents;
}

//------------------------------------------------------------------------------
// Загрузка компонентов
//------------------------------------------------------------------------------
void JdscParser::loadComponents(QMap<QString, Component>& coComponentMap,
                                QMap<QString, QList<PackDescriptionParser::ParentComponentInfo> >& parentComponentInfoMap,
                                const QList<JdscComponent>& componentList,
                                PackDescription& pack)
{
    if(pack.vendors().isEmpty())
        return;

    foreach (JdscComponent jComponent, componentList)
    {
        if(jComponent.name().startsWith("CMSIS_Core", Qt::CaseInsensitive))
            continue;

        QMap<QString, Manufacturer>& vendors = pack.vendors();

        for(auto v = vendors.begin(); v != vendors.end(); ++v)
        {
            Manufacturer& vendor = v.value();
            QMap<QString, Family>& families = vendor.families();

            for(auto f = families.begin(); f != families.end(); ++f)
            {
                Family& family = f.value();
                QMap<QString, Series>& seriesMap = family.seriesMap();

                for(auto s = seriesMap.begin(); s != seriesMap.end(); ++s)
                {
                    Series& series = s.value();
                    QMap<QString, Mcu>& devices = series.mcuMap();

                    for(auto d = devices.begin(); d != devices.end(); ++d)
                    {
                        Mcu& device = d.value();

                        //
                        // Компонент предназначен для данного устройства
                        //
                        if(jComponent.supportDevice(device.getName()))
                        {
                            Component coComponent;
                            Category coCategory(Category::categoryType(jComponent.group()));

                            coCategory.setSubCategoryName(jComponent.cclass());

                            coComponent.setLayerId(Component::LAYER_MCU);
                            coComponent.setType(Component::COMPONENT);
                            coComponent.setCategory(coCategory);

                            if(!jComponent.description().isEmpty())
                                coComponent.setDescription(jComponent.description());
                            else
                                coComponent.setDescription(jComponent.name());

                            coComponent.files().clear();
                            coComponent.files().append(addMetaDataToFiles(jComponent.files()));

                            //
                            // Заполняем поля, которые помогут идентифицировать
                            // компонент как уникальный объект
                            //
                            coComponent.setPdscVendor(pack.packVendor());

                            coComponent.setPdscClass(jComponent.cclass());
                            coComponent.setPdscGroup(jComponent.group());
                            coComponent.setPdscSub(jComponent.name());
                            //coComponent.setPdscVariant(jComponent.attributes().getCvariant());

                            if(!jComponent.version().isEmpty())
                                coComponent.setPdscVersion(jComponent.version());
                            else
                                coComponent.setPdscVersion(pack.release());
                            coComponent.setVersion(coComponent.getPdscVersion());

                            // Формируем имя компонента
                            coComponent.setName(QString("%1_%2").
                                                arg(jComponent.name()).
                                                arg(coComponent.getVersion()));

                            //
                            // Добавляем текущий компонент в карту
                            //
                            Component * coideComponent = nullptr;

                            if(coComponentMap.values().contains(coComponent))
                            {
                                for(auto it = coComponentMap.begin(); it != coComponentMap.end(); ++it)
                                {
                                    Component& existingComponent = it.value();

                                    if(existingComponent == coComponent)
                                    {
                                        coideComponent = &existingComponent;
                                        existingComponent.addSupportedMcu(device.getName());
                                        break;
                                    }
                                }
                            }
                            else
                            {
                                coComponent.addSupportedMcu(device.getName());
                                coComponentMap.insert(coComponent.getUuid(), coComponent);
                                coideComponent = &coComponentMap[coComponent.getUuid()];
                            }

                            //
                            // Фиксируем связь с другими компонентами
                            //
                            for(auto parent : jComponent.parents())
                            {
                                // Компонент не может быть связан сам с собой
                                if(jComponent.name() == parent.name() &&
                                   jComponent.version() == parent.version())
                                    continue;

                                ParentComponentInfo newParentInfo;
                                newParentInfo.Jname = parent.name();
                                newParentInfo.Jversion = parent.version();

                                updateParentComponentMap(parentComponentInfoMap,
                                                         coideComponent->getUuid(),
                                                         newParentInfo);
                            }
                        }
                    }
                }
            }
        }
    }
}

//------------------------------------------------------------------------------
// Ищет компонент в карте по косвенным признакам
//------------------------------------------------------------------------------
QList<Component*> JdscParser::findParentsComponent(
        const QMap<QString, Component>& coComponentMap,
        const PackDescriptionParser::ParentComponentInfo& parent)
{
    QList<Component*> foundComponents;

    for(auto it = coComponentMap.begin(); it != coComponentMap.end(); ++it)
    {
        const Component& c = it.value();

        if(parent.Jname.startsWith("CMSIS_Core", Qt::CaseInsensitive))
        {
            if(c.getName().toUpper() == QString("%1_%2").arg(parent.Jname).arg(parent.Jversion).toUpper())
            {
                foundComponents.append(&const_cast<Component&>(c));
            }
        }
        else
        {
            if(c.getName().startsWith(QString("%1_").arg(parent.Jname)))
            {
                foundComponents.append(&const_cast<Component&>(c));
            }
        }
    }

    return foundComponents;
}

//------------------------------------------------------------------------------
// Добавление служебной информации к пути файла
//------------------------------------------------------------------------------
QStringList JdscParser::addMetaDataToFiles(const QStringList& files)
{
    QStringList modifiedList;

    foreach (const QString& file, files)
    {
        modifiedList.append(QString("%1=%2").arg("source").arg(file));
    }

    return modifiedList;
}



