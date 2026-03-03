#include <QApplication>
#include <QDebug>
#include <QFile>
#include <QDomText>
#include <QDomNode>
#include <QDomNodeList>
#include <QList>
#include <QRegularExpression>
#include "pdscparser.h"

//------------------------------------------------------------------------------
// Конструктор
//------------------------------------------------------------------------------
PdscParser::PdscParser()
{

}

//------------------------------------------------------------------------------
// Загрузка XML-документа в память и разбор
//------------------------------------------------------------------------------
bool PdscParser::parse(PackDescription &pack)
{
    QFile pdscFile(pack.pathToPdsc());

    if(!pdscFile.exists())
    {
        _lastErrorStr = QObject::tr("PDSC-файл не найден");
    }
    else if(!pdscFile.open(QFile::ReadOnly))
    {
        _lastErrorStr = pdscFile.errorString();
    }
    else
    {
        QDomDocument domDocument;
        QString errMsg;
        int errLine;
        int errColum;

        if(domDocument.setContent(&pdscFile, &errMsg, &errLine, &errColum))
        {
            parseDomDocument(&domDocument, pack);
            _lastErrorStr = "";
        }
        else
        {
            _lastErrorStr = QObject::tr("Ошибка парсинга PDSC-файла "
                                       "в строке %1 столбец %2: \"%3\"").
                                        arg(errLine).
                                        arg(errColum).
                                        arg(errMsg);
        }
        pdscFile.close();
    }

    return false;
}

//------------------------------------------------------------------------------
// Парсинг XML-документа
//------------------------------------------------------------------------------
void PdscParser::parseDomDocument(QDomDocument *doc, PackDescription &pack)
{
    if(doc->isNull())
    {
        return;
    }

    // Корневой элемент документа (<package>)
    QDomElement root = doc->documentElement();
    QDomNodeList rootChildren = doc->childNodes();
    QDomNodeList children;

    for(int j = 0; j < rootChildren.length(); j++)
    {
        QDomNode node = rootChildren.at(j);
        QString nodeName = node.nodeName();

        if(nodeName == "package")
        {
            children = node.childNodes();
            break;
        }
    }

    for(int j = 0; j < children.length(); j++)
    {
        QDomNode node = children.at(j);
        QString nodeName = node.nodeName();
        QDomText textNode = QDomText();
        QDomNode firstChild;
        QString text = "";

        if(node.hasChildNodes())
        {
            firstChild = node.childNodes().at(0);
            if(firstChild.isText())
            {
                textNode = firstChild.toText();
                text = textNode.data();
            }
        }

        if(nodeName == "vendor")
        {
            pack.setVendor(text);
        }
        else if(nodeName == "name")
        {
            pack.setName(text);
        }
        else if(nodeName == "description")
        {
            pack.setDescription(text);
        }
        else if(nodeName == "url")
        {
            pack.setUrl(text);
        }
        else if(nodeName == "releases")
        {
            QDomElement lastRelease = node.firstChildElement("release");

            if(!lastRelease.isNull())
            {
                pack.setRelease(lastRelease.attribute("version"));
            }
        }
    }

    QDomElement conditionsElem = root.firstChildElement("conditions");
    QDomElement componentsElem = root.firstChildElement("components");
    QDomElement devicesElem = root.firstChildElement("devices");
    QList<PdscCondition> conditionList;
    QList<PdscComponent> componentList;

    if(!devicesElem.isNull())
    {
        parseDevFamilies(devicesElem, pack);
    }

    if(!conditionsElem.isNull())
    {
        QDomNodeList conditions = conditionsElem.childNodes();

        for(int i = 0; i < conditions.count(); i++)
        {
            QDomNode conditionNode = conditions.at(i);
            PdscCondition cond = parseCondition(conditionNode);
            conditionList.append(cond);
        }
    }

    if (!componentsElem.isNull())
    {
        QDomNodeList components = componentsElem.childNodes();

        for(int i = 0; i < components.count(); i++)
        {
            QDomNode componentNode = components.at(i);
            PdscComponent component = parseComponent(componentNode, conditionList);
            componentList.append(component);
        }
    }

    loadComponents(componentList, pack);
}

//------------------------------------------------------------------------------
// Поиск описания устройств и разбор
//------------------------------------------------------------------------------
void PdscParser::parseDevFamilies(const QDomNode &node, PackDescription &pack)
{
    struct DevNode
    {
        int vendorId;
        QString vendor;
        QString core;
        QString serie;
        QDomNode* device;

        DevNode()
        {
            this->vendorId = -1;
            this->vendor = "";
        }
    };

    QList<DevNode> devNodes;
    QDomNodeList families = node.childNodes();

    for(int j = 0; j < families.length(); j++)
    {
        QDomNode family = families.at(j);
        QString familyName = family.attributes().namedItem("Dfamily").nodeValue();
        QString vendorInfo = family.attributes().namedItem("Dvendor").nodeValue();
        QString familyProcessor = family.namedItem("processor").attributes().namedItem("Dcore").nodeValue();

        if(familyName.isEmpty() || vendorInfo.isEmpty() || !vendorInfo.contains(":"))
            continue;

        QDomNodeList familyElements = family.childNodes();
        QList<ProgAlgorithm> coreAlgorithms;
        QList<ProgAlgorithm> subFamilyAlgorithms;
        QList<DeviceFeature> coreFeatures;
        QList<DeviceFeature> subFamilyFeatures;
        QStringList coreCompileOptions;
        QStringList subFamilyCompileOptions;
        QStringList coreCompileHeaders;
        QStringList subFamilyCompileHeaders;
        QString coreSVD;
        QString subFamilySVD;

        for(int k = 0; k < familyElements.length(); k++)
        {
            QDomNode node = familyElements.at(k);
            QString nodeName = node.nodeName();

            if(nodeName == "subFamily")
            {
                QString subFamilyName = node.attributes().namedItem("DsubFamily").nodeValue();
                QString subFamilyProcessor;

                QDomNodeList subFamilyElements = node.childNodes();

                for (int n = 0; n < subFamilyElements.length(); n++)
                {
                    QDomNode node = subFamilyElements.at(n);
                    QString nodeName = node.nodeName();

                    if(nodeName == "processor")
                    {
                        subFamilyProcessor = node.attributes().namedItem("Dcore").nodeValue();
                    }
                    else if(nodeName == "device")
                    {
                        QString devProcessor = node.namedItem("processor").attributes().namedItem("Dcore").nodeValue();
                        QString definedSymbol = node.namedItem("compile").attributes().namedItem("define").nodeValue();
                        QString compileHeader = node.namedItem("compile").attributes().namedItem("header").nodeValue();
                        QString coreName;

                        if(devProcessor.isEmpty())
                            if(subFamilyProcessor.isEmpty())
                                coreName = familyProcessor;
                            else
                                coreName = subFamilyProcessor;
                        else
                            coreName = devProcessor;

                        Mcu& newMcu = parseDevice(node, vendorInfo, coreName, subFamilyName, pack);

                        //
                        // Добавляем алгоритмы программирования
                        //
                        if(!newMcu.hasAlgorithms())
                            foreach(ProgAlgorithm a, subFamilyAlgorithms)
                                newMcu.addAlgorithm(a);

                        //
                        // Добавляем алгоритм отладки
                        //
                        newMcu.setCoreDebugAlgorithm(coreName);

                        //
                        // Добавляем файл описание периферии, если он общий для семейства
                        //
                        if(newMcu.svdLocalPath().isEmpty())
                        {
                            if(subFamilySVD.isEmpty())
                                newMcu.setSvd(coreSVD);
                            else
                                newMcu.setSvd(subFamilySVD);
                        }

                        //
                        // Добавляем фичи, свойственные для подсемейства
                        //
                        foreach(DeviceFeature feat, subFamilyFeatures)
                        {
                            newMcu.addFeature(feat);
                        }

                        //
                        // Добавляем defined symbols
                        //
                        foreach(QString def, subFamilyCompileOptions)
                        {
                            newMcu.addDefSymbol(def);
                        }

                        if(!definedSymbol.isEmpty())
                            newMcu.addDefSymbol(definedSymbol);

                        //
                        // Добавляем compile header
                        //
                        foreach(QString hdr, subFamilyCompileHeaders)
                        {
                            newMcu.addCompileHeader("header=" + hdr);
                        }

                        if(!compileHeader.isEmpty())
                            newMcu.addCompileHeader("header=" + compileHeader);
                    }
                    else if(nodeName == "feature")
                    {
                        DeviceFeature devFeature = parseFeature(node.toElement());
                        subFamilyFeatures.append(devFeature);
                    }
                    else if(nodeName == "algorithm")
                    {
                        QString coreName;
                        ProgAlgorithm subFamilyAlgorithm = parseAlgorithm(node.toElement());

                        coreName = subFamilyProcessor.isEmpty() ? familyProcessor : subFamilyProcessor;
                        subFamilyAlgorithms.append(subFamilyAlgorithm);
                    }
                    else if(nodeName == "debug")
                    {
                        subFamilySVD = node.attributes().namedItem("svd").nodeValue();
                    }
                    else if(nodeName == "compile")
                    {
                        QString define = node.attributes().namedItem("define").nodeValue();
                        QString header = node.attributes().namedItem("header").nodeValue();

                        if(!define.isEmpty())
                            subFamilyCompileOptions.append(define);

                        if(!header.isEmpty())
                            subFamilyCompileHeaders.append(header);
                    }
                }
            }
            else if(nodeName == "device")
            {
                QString devProcessor = node.namedItem("processor").attributes().namedItem("Dcore").nodeValue();
                QString definedSymbol = node.namedItem("compile").attributes().namedItem("define").nodeValue();
                QString compileHeader = node.namedItem("compile").attributes().namedItem("header").nodeValue();
                QString coreName;

                coreName = devProcessor.isEmpty() ? familyProcessor : devProcessor;
                Mcu& newMcu = parseDevice(node, vendorInfo, coreName, familyName, pack);

                //
                // Добавляем алгоритмы программирования
                //
                if(!newMcu.hasAlgorithms())
                    foreach(ProgAlgorithm a, coreAlgorithms)
                        newMcu.addAlgorithm(a);

                //
                // Добавляем алгоритм отладки
                //
                newMcu.setCoreDebugAlgorithm(coreName);

                //
                // Добавляем файл описание периферии, если он общий для семейства
                //
                if(newMcu.svdLocalPath().isEmpty())
                    newMcu.setSvd(coreSVD);

                //
                // Добавляем фичи, свойственные для ядра
                //
                foreach(DeviceFeature feat, coreFeatures)
                {
                    newMcu.addFeature(feat);
                }

                //
                // Добавляем defined symbols
                //
                foreach(QString def, coreCompileOptions)
                {
                    newMcu.addDefSymbol(def);
                }

                if(!definedSymbol.isEmpty())
                    newMcu.addDefSymbol(definedSymbol);

                //
                // Добавляем заголовочный файлы для компилятора
                //
                foreach (QString hdr, coreCompileHeaders)
                {
                    newMcu.addCompileHeader("header=" + hdr);
                }

                if(!compileHeader.isEmpty())
                    newMcu.addCompileHeader("header=" + compileHeader);
            }
            else if(nodeName == "feature" && !familyProcessor.isEmpty())
            {
                DeviceFeature devFeature = parseFeature(node.toElement());
                coreFeatures.append(devFeature);
            }
            else if(nodeName == "algorithm")
            {
                ProgAlgorithm familyAlgorithm = parseAlgorithm(node.toElement());
                coreAlgorithms.append(familyAlgorithm);
            }
            else if(nodeName == "debug")
            {
                coreSVD = node.attributes().namedItem("svd").nodeValue();
            }
            else if(nodeName == "compile")
            {
                QString define = node.attributes().namedItem("define").nodeValue();
                QString header = node.attributes().namedItem("header").nodeValue();

                if(!define.isEmpty())
                    coreCompileOptions.append(define);

                if(!header.isEmpty())
                    coreCompileHeaders.append(header);
            }
        }
    }
}

//------------------------------------------------------------------------------
// Парсинг блока device
//------------------------------------------------------------------------------
Mcu &PdscParser::parseDevice(const QDomNode &deviceNode,
                             const QString &vendorInfo,
                             const QString &processor,
                             const QString &series,
                             PackDescription &pack)
{
    QStringList vendorParts = vendorInfo.split(":") << "unknown" << "-1";
    QString vendorName = vendorParts.at(0);
    int vendorId = vendorParts.at(1).toInt();
    QString devName = deviceNode.attributes().namedItem("Dname").nodeValue();

    pack.vendor(vendorName).setId(vendorId);
    Mcu& newMcu = pack.vendor(vendorName).family(processor).series(series).addMcu(devName);

    //
    // Формирует ссылки на документацию и сайт
    //
    newMcu.setDatasheetURL(QString("https://yandex.ru/search/?text=%1+datasheet").arg(devName.replace(' ', "+")));
    newMcu.setWebPageURL(QString("https://yandex.ru/search/?text=%1").arg(vendorName.replace(' ', "+")));

    //
    // Парсинг описания
    //
    if(!deviceNode.namedItem("description").isNull() &&
        deviceNode.namedItem("description").hasChildNodes())
    {
        newMcu.setDescription(deviceNode.namedItem("description").firstChild().nodeValue());
    }

    //
    // Парсинг блока debug
    //
    if(!deviceNode.namedItem("debug").isNull())
    {
        newMcu.setSvd(deviceNode.namedItem("debug").attributes().namedItem("svd").nodeValue());
    }

    //
    // Парсинг блоков memory
    //
    if(!deviceNode.firstChildElement("memory").isNull())
    {
        QDomNodeList memories = deviceNode.toElement().elementsByTagName("memory");

        for (int i = 0; i < memories.count(); i++)
        {
               QDomElement memoryElem = memories.at(i).toElement();

               QString id = memoryElem.attribute("id");
               QString name = memoryElem.attribute("name");

               QString startStr = memoryElem.attribute("start");
               QString sizeStr = memoryElem.attribute("size");

               quint32 start = startStr.toUInt(nullptr, 0);
               quint32 size = sizeStr.toUInt(nullptr, 0);

               QString startup = memoryElem.attribute("startup").toLower();
               QString deflt = memoryElem.attribute("default").toLower();
               QString init = memoryElem.attribute("init").toLower();

               bool isStartup = (startup == "true" || startup == "1");
               bool isDefault = (deflt == "true" || deflt == "1");
               bool isInit = (init == "true" || init == "1");

               QString access = memoryElem.attribute("access");

               Memory& memoryRegion = newMcu.memory(KeilMemory(id, name).name());

               memoryRegion.setStartAddr(start);
               memoryRegion.setSize(size);
               memoryRegion.setAccess(access);
               memoryRegion.setStartup(isStartup);
               memoryRegion.setDefault(isDefault);
               memoryRegion.setInit(isInit);
        }
    }

    //
    // Парсинг блоков feature
    //
    if(!deviceNode.firstChildElement("feature").isNull())
    {
        QDomNodeList features = deviceNode.toElement().elementsByTagName("feature");

        for (int i = 0; i < features.count(); i++)
        {
            QDomElement featureElem = features.at(i).toElement();
            DeviceFeature devFeature = parseFeature(featureElem);

            if(!devFeature.type().isEmpty())
            {
                newMcu.addFeature(devFeature);
            }
        }
    }

    //
    // Парсинг блоков algorithm
    //
    if(!deviceNode.firstChildElement("algorithm").isNull())
    {
        QDomNodeList algorithms = deviceNode.toElement().elementsByTagName("algorithm");

        for (int i = 0; i < algorithms.count(); i++)
        {
            QDomElement algoElem = algorithms.at(i).toElement();
            ProgAlgorithm flashAlgo = parseAlgorithm(algoElem);

            if(!flashAlgo.name().isEmpty())
            {
               newMcu.addAlgorithm(flashAlgo);
            }
        }
    }

    return newMcu;
}

//------------------------------------------------------------------------------
// Парсинг блока feature
//------------------------------------------------------------------------------
DeviceFeature PdscParser::parseFeature(const QDomElement &featureElem)
{
    DeviceFeature feature;

    QString type = featureElem.attribute("type");
    QString name = featureElem.attribute("name");
    QString n = featureElem.attribute("n");
    QString m = featureElem.attribute("m");
    QString Pname = featureElem.attribute("Pname");
    QString count = featureElem.attribute("count");

    feature.setType(type);
    feature.setName(name);

    if(m.toLower().contains("0x"))
        feature.setM(m.toInt(nullptr, 16));
    else
        feature.setM(m.toDouble(nullptr));

    if(n.toLower().contains("0x"))
        feature.setN(n.toInt(nullptr, 16));
    else
        feature.setN(n.toDouble(nullptr));

    feature.setCount(count.toInt(nullptr, 10));
    feature.setPname(Pname);

    return feature;
}

//------------------------------------------------------------------------------
// Парсинг блока algorithm
//------------------------------------------------------------------------------
ProgAlgorithm PdscParser::parseAlgorithm(const QDomElement &algorithmElement)
{
    ProgAlgorithm algo;

    QString name = algorithmElement.attribute("name");
    QString start = algorithmElement.attribute("start");
    QString size = algorithmElement.attribute("size");
    QString ramStart = algorithmElement.attribute("RAMstart");
    QString ramSize = algorithmElement.attribute("RAMsize");
    QString isDefault = algorithmElement.attribute("default");

    algo.setName(name);

    if(start.contains("0x", Qt::CaseInsensitive))
        algo.setStart(start.toUInt(nullptr, 16));
    else
        algo.setStart(start.toUInt(nullptr, 10));

    if(size.contains("0x", Qt::CaseInsensitive))
        algo.setSize(size.toUInt(nullptr, 16));
    else
        algo.setSize(size.toUInt(nullptr, 10));

    if(ramStart.contains("0x", Qt::CaseInsensitive))
        algo.setRAMstart(ramStart.toUInt(nullptr, 16));
    else
        algo.setRAMstart(ramStart.toUInt(nullptr, 10));

    if(ramSize.contains("0x", Qt::CaseInsensitive))
        algo.setRAMsize(ramSize.toUInt(nullptr, 16));
    else
        algo.setRAMsize(ramSize.toUInt(nullptr, 10));

    if(isDefault == "true" || isDefault == "1")
        algo.setDefault(true);
    else
        algo.setDefault(false);

    return algo;
}

//------------------------------------------------------------------------------
// Парсинг блока condition
//------------------------------------------------------------------------------
PdscCondition PdscParser::parseCondition(const QDomNode &conditionNode)
{
    PdscCondition condition;
    QString condId = conditionNode.attributes().namedItem("id").nodeValue();
    QString condDescription = conditionNode.firstChildElement("description").text();

    condition.setId(condId);
    condition.setDescription(condDescription);

    if(!conditionNode.firstChildElement("require").isNull())
    {
        QDomNodeList requires = conditionNode.toElement().elementsByTagName("require");

        for (int i = 0; i < requires.count(); i++)
        {
            QDomNode require = requires.at(i);
            QString conditionId = require.attributes().namedItem("condition").nodeValue();

            // Вложенный condition
            if(!conditionId.isEmpty())
            {
                QDomNode conditionsNode = conditionNode.parentNode();
                QDomNodeList conditions = conditionsNode.childNodes();

                for(int j = 0; j < conditions.count(); j++)
                {
                    QDomNode currNode = conditions.at(j);
                    QString id = currNode.attributes().namedItem("id").nodeValue();

                    if(id == conditionId)
                    {
                        PdscCondition nestedCond = parseCondition(currNode);
                        condition.addCondition(nestedCond);
                        break;
                    }
                }
            }
            else
            {
                PdscRequirement requirement = parseRequirement(require);
                condition.addRequirement(requirement);
            }
        }
    }

    if(!conditionNode.firstChildElement("accept").isNull())
    {
        QDomNodeList requires = conditionNode.toElement().elementsByTagName("accept");

        for (int i = 0; i < requires.count(); i++)
        {
            QDomNode require = requires.at(i);
            PdscRequirement requirement = parseRequirement(require);
            condition.addRequirement(requirement);
        }
    }

    if(!conditionNode.firstChildElement("deny").isNull())
    {
        QDomNodeList requires = conditionNode.toElement().elementsByTagName("deny");

        for (int i = 0; i < requires.count(); i++)
        {
            QDomNode require = requires.at(i);
            PdscRequirement requirement = parseRequirement(require);
            condition.addRequirement(requirement);
        }
    }

    return condition;
}

//------------------------------------------------------------------------------
// Парсинг блоков require, accept, deny
//------------------------------------------------------------------------------
PdscRequirement PdscParser::parseRequirement(const QDomNode &requireNode)
{
    PdscRequirement requirement;
    QString nodeName = requireNode.nodeName();

    if(nodeName == "require")
        requirement.setType(PdscRequirement::Require);
    else if(nodeName == "accept")
        requirement.setType(PdscRequirement::Accept);
    else if(nodeName == "deny")
        requirement.setType(PdscRequirement::Deny);

    // Device requirements
    requirement.setDname(requireNode.attributes().namedItem("Dname").nodeValue());
    requirement.setDfamily(requireNode.attributes().namedItem("Dfamily").nodeValue());
    requirement.setDsubFamily(requireNode.attributes().namedItem("DsubFamily").nodeValue());  // Заглавная F!
    requirement.setDcore(requireNode.attributes().namedItem("Dcore").nodeValue());
    requirement.setDfpu(requireNode.attributes().namedItem("Dfpu").nodeValue());
    requirement.setDmpu(requireNode.attributes().namedItem("Dmpu").nodeValue());
    requirement.setDdsp(requireNode.attributes().namedItem("Ddsp").nodeValue());
    requirement.setDvendor(requireNode.attributes().namedItem("Dvendor").nodeValue());
    requirement.setDvariant(requireNode.attributes().namedItem("Dvariant").nodeValue());

    // Component requirements
    requirement.setCclass(requireNode.attributes().namedItem("Cclass").nodeValue());
    requirement.setCgroup(requireNode.attributes().namedItem("Cgroup").nodeValue());
    requirement.setCsub(requireNode.attributes().namedItem("Csub").nodeValue());
    requirement.setCvariant(requireNode.attributes().namedItem("Cvariant").nodeValue());
    requirement.setCversion(requireNode.attributes().namedItem("Cversion").nodeValue());

    // Compiler requirements
    requirement.setTcompiler(requireNode.attributes().namedItem("Tcompiler").nodeValue());
    requirement.setTversion(requireNode.attributes().namedItem("Tversion").nodeValue());

    // Pack requirements
    requirement.setPvendor(requireNode.attributes().namedItem("Pvendor").nodeValue());
    requirement.setPname(requireNode.attributes().namedItem("Pname").nodeValue());
    requirement.setPversion(requireNode.attributes().namedItem("Pversion").nodeValue());

    return requirement;
}

//------------------------------------------------------------------------------
// Парсинг блока component
//------------------------------------------------------------------------------
PdscComponent PdscParser::parseComponent(const QDomNode &componentNode,
                                         const QList<PdscCondition> &conditionList)
{
    PdscComponent component;
    QDomNamedNodeMap attr = componentNode.attributes();
    QString conditionId = attr.namedItem("condition").nodeValue();

    // Устанавливаем все атрибуты компонента
    component.attributes().setCvendor(attr.namedItem("Cvendor").nodeValue());
    component.attributes().setCbundle(attr.namedItem("Cbundle").nodeValue());
    component.attributes().setCclass(attr.namedItem("Cclass").nodeValue());
    component.attributes().setCgroup(attr.namedItem("Cgroup").nodeValue());
    component.attributes().setCsub(attr.namedItem("Csub").nodeValue());
    component.attributes().setCvariant(attr.namedItem("Cvariant").nodeValue());
    component.attributes().setCversion(attr.namedItem("Cversion").nodeValue());
    component.attributes().setCapiversion(attr.namedItem("Capiversion").nodeValue());
    component.attributes().setInstances(attr.namedItem("instances").nodeValue());

    component.setDescription(componentNode.firstChildElement("description").text());

    // У компонента есть условия
    if(!conditionId.isEmpty())
    {
        foreach(PdscCondition c, conditionList)
        {
            if(c.id() == conditionId)
            {
                component.setCondition(c);
                break;
            }
        }
    }

    // Компонент содержит файлы
    if(!componentNode.firstChildElement("files").isNull())
    {
        QDomNodeList fileList = componentNode.firstChildElement("files").childNodes();

        for(int i = 0; i < fileList.count(); i++)
        {
            QDomNode fileNode = fileList.at(i);

            if(fileNode.nodeName() == "file")
            {
                PdscFile file = parseFile(fileNode, conditionList);
                component.files().append(file);
            }
        }
    }

    return component;
}

//------------------------------------------------------------------------------
// Парсинг блока file
//------------------------------------------------------------------------------
PdscFile PdscParser::parseFile(const QDomNode &fileNode,
                               const QList<PdscCondition> &conditionList)
{
    PdscFile file;
    QDomNamedNodeMap attr = fileNode.attributes();
    QString conditionId = attr.namedItem("condition").nodeValue();

    if(!conditionId.isEmpty())
    {
        foreach(PdscCondition c, conditionList)
        {
            if(c.id() == conditionId)
            {
                file.setCondition(c);
                break;
            }
        }
    }

    file.setLanguage(attr.namedItem("language").nodeValue());
    file.setScope(attr.namedItem("scope").nodeValue());
    file.setAttr(attr.namedItem("attr").nodeValue());
    file.setSelect(attr.namedItem("select").nodeValue());
    file.setPath(attr.namedItem("path").nodeValue());
    file.setCopy(attr.namedItem("copy").nodeValue());
    file.setVersion(attr.namedItem("version").nodeValue());
    file.setSrc(attr.namedItem("src").nodeValue());
    file.setPublic(attr.namedItem("public").nodeValue() == "1");
    file.setProjectpath(attr.namedItem("projectpath").nodeValue());
    file.setName(attr.namedItem("name").nodeValue());
    file.setCategory(FileCategory(attr.namedItem("category").nodeValue()));

    return file;
}

//------------------------------------------------------------------------------
// Формирование компонентов и связь с устройствами
//------------------------------------------------------------------------------
void PdscParser::loadComponents(const QList<PdscComponent> &componentList,
                                PackDescription &pack)
{
    if(pack.vendors().isEmpty())
        return;

    foreach (PdscComponent pComponent, componentList)
    {
        // Проверка, что компонент соответствует пакету и его версии
        if(!checkRequirements(pack, pComponent, componentList))
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

#if 0
                        if(pComponent.attributes().getCclass() == "Device" &&
                           pComponent.attributes().getCgroup() == "Startup" &&
                           pComponent.condition().id() == "nRF5340 Device network core and CMSIS" &&
                           device.getName() != "nRF5340_xxAA")
                        {
                            qInfo() << device.getName() << pComponent.condition().id();
                        }
#endif

                        //
                        // Компонент предназначен для данного устройства
                        //
                        if(checkRequirements(pack, vendor, family, series, device, pComponent, componentList))
                        {
                            Component coComponent;
                            Category coCategory = Category::categoryCommon();

                            // Принудительно задаем точку входа в программу main()
                            if(pComponent.attributes().getCclass().toLower() == "device" &&
                               pComponent.attributes().getCgroup().toLower() == "startup")
                            {
                                coCategory = Category::categoryBoot();
                                coComponent.setMicro("__START=main");
                            }

                            coCategory.setSubCategoryName(pComponent.attributes().getCclass());

                            coComponent.setLayerId(Component::LAYER_MCU);
                            coComponent.setType(Component::COMPONENT);

                            if(pComponent.attributes().getCversion().isEmpty())
                            {
                                coComponent.setVersion(pack.release());
                                coComponent.setName(pComponent.attributes().getCgroup() + "_" +
                                                    pack.release());
                            }
                            else
                            {
                                coComponent.setVersion(pComponent.attributes().getCversion());
                                coComponent.setName(pComponent.attributes().getCgroup() + "_" +
                                                    pComponent.attributes().getCversion());
                            }

                            coComponent.setCategory(coCategory);

                            if(!pComponent.description().isEmpty())
                                coComponent.setDescription(pComponent.description());
                            else if(pComponent.hasCondition())
                                coComponent.setDescription(pComponent.condition().description());

                            coComponent.files().clear();
                            coComponent.files().append(getFilteredFiles(pack,
                                                                        vendor,
                                                                        family,
                                                                        series,
                                                                        device,
                                                                        pComponent,
                                                                        componentList));

                            //
                            // Связываем компоненты с CMSIS
                            //
                            QList<PdscRequirement> requiredComponents = pComponent.condition().requirementsMap()[PdscRequirement::Require].value(PdscRequirement::Component);

                            foreach(PdscRequirement r, requiredComponents)
                            {
                                if(r.Cclass().toUpper() == "CMSIS" && r.Cgroup().toUpper() == "CORE")
                                {
                                    //TODO пока жестко завязываем на одну верси CMSIS
                                    if(pack.cmsisComponents().contains("5.6.0"))
                                    {
                                        Component * cmsisCore = pack.cmsisComponents()["5.6.0"];

                                        if(cmsisCore)
                                        {
                                            coComponent.addDependence(cmsisCore);
                                        }
                                    }
                                    break;
                                }
                            }

#if 1
                            //-------------------------------------------------------------------------------
                            // Если у pdsc кривое описание и нет явного определения CMSIS CORE
                            // Как у Nordic DFP 8.28.0
                            if(pComponent.attributes().getCclass().toLower() == "device" &&
                               pComponent.attributes().getCgroup().toLower() == "startup" &&
                               !coComponent.hasDependence(pack.cmsisComponents().value("5.6.0")->getName()))
                            {
                                Component * cmsisCore = pack.cmsisComponents()["5.6.0"];

                                if(cmsisCore)
                                {
                                    coComponent.addDependence(cmsisCore);
                                }
                            }
                            //-------------------------------------------------------------------------------
#endif

                            if(pack.components().values().contains(coComponent))
                            {
                                for(auto it = pack.components().begin(); it != pack.components().end(); ++it)
                                {
                                    Component& existingComponent = it.value();

                                    if(existingComponent == coComponent)
                                    {
                                        existingComponent.addSupportedMcu(device.getName());
                                        break;
                                    }
                                }
                            }
                            else
                            {
                                coComponent.addSupportedMcu(device.getName());
                                pack.components().insert(coComponent.getUuid(), coComponent);
                            }
                        }

                        //
                        // Превращаем параметр compile header в компонент
                        //
                        if(!device.compileHeaders().isEmpty())
                        {
                            Component coComponent;
                            Category coCategory = Category::categoryCommon();

                            coCategory.setSubCategoryName("Device");

                            coComponent.setLayerId(Component::LAYER_MCU);
                            coComponent.setType(Component::COMPONENT);

                            coComponent.setVersion(pack.release());
                            coComponent.setName("Compile_" + pack.release());

                            coComponent.setCategory(coCategory);
                            coComponent.setDescription("CMSIS-Core compliant device header file");
                            coComponent.files().clear();
                            coComponent.files().append(device.compileHeaders());

                            if(pack.components().values().contains(coComponent))
                            {
                                for(auto it = pack.components().begin(); it != pack.components().end(); ++it)
                                {
                                    Component& existingComponent = it.value();

                                    if(existingComponent == coComponent)
                                    {
                                        existingComponent.addSupportedMcu(device.getName());
                                        break;
                                    }
                                }
                            }
                            else
                            {
                                coComponent.addSupportedMcu(device.getName());
                                pack.components().insert(coComponent.getUuid(), coComponent);
                            }
                        }
                    }
                }
            }
        }
    }
}

//------------------------------------------------------------------------------
// Проверить соответствует ли компонент требованиям и принадлежности к MCU
//------------------------------------------------------------------------------
bool PdscParser::checkRequirements(const PackDescription& pack,
                                   const Manufacturer &vendor,
                                   const Family &family,
                                   const Series &series,
                                   const Mcu &device,
                                   const PdscComponent &component,
                                   const QList<PdscComponent> &componentList)
{
//    Q_UNUSED(pack)
//    Q_UNUSED(vendor)
//    Q_UNUSED(family)
//    Q_UNUSED(series)
//    Q_UNUSED(componentList)

    bool status = true;
    auto requirementMap = component.condition().requirementsMap();

    foreach(PdscRequirement r, requirementMap[PdscRequirement::Require].value(PdscRequirement::Device))
    {
        if(!r.isValid())
            continue;

        if(device.getName() != r.Dname() && !device.getName().contains(QRegExp(r.Dname())))
        {
            status = false;
            break;
        }
    }

    foreach(PdscRequirement r, requirementMap[PdscRequirement::Accept].value(PdscRequirement::Device))
    {
        if(!r.isValid())
            continue;

        if(device.getName() == r.Dname() || device.getName().contains(QRegExp(r.Dname())))
        {
            status = true;
            break;
        }
        else
            status = false;
    }

    foreach(PdscRequirement r, requirementMap[PdscRequirement::Deny].value(PdscRequirement::Device))
    {
        if(!r.isValid())
            continue;

        if(device.getName() == r.Dname() || device.getName().contains(QRegExp(r.Dname())))
        {
            status = false;
            break;
        }
    }

    return status;
}

//------------------------------------------------------------------------------
// Проверить соответствует ли компонент общим требованиям к пакету и компилятору
//------------------------------------------------------------------------------
bool PdscParser::checkRequirements(const PackDescription &pack,
                                   const PdscComponent &component,
                                   const QList<PdscComponent> &componentList)
{
    auto requirementMap = component.condition().requirementsMap();

    //
    // Проверка наличия компонентов, от которых зависит данный компонент
    //
    foreach(PdscRequirement r, requirementMap[PdscRequirement::Require].value(PdscRequirement::Component))
    {
        bool found = false;

        if(!r.isValid())
            continue;

        foreach(PdscComponent c, componentList)
        {
            if(r.Cclass() == c.attributes().getCclass() &&
               r.Cgroup() == c.attributes().getCgroup())
            {
                found = true;
                break;
            }
        }

        //TODO пока игнорируем CMSIS
        if(!found && r.Cclass() != "CMSIS" && r.Cgroup() != "CORE")
            return false;
    }

    //
    // Проверка соответствия пакету и его версии (глупо, но всё же :-)))
    //
    foreach(PdscRequirement r, requirementMap[PdscRequirement::Require].value(PdscRequirement::Pack))
    {
        if(!r.isValid())
            continue;

        if(r.Pname() != pack.name() || r.Pvendor() != pack.packVendor() || r.Pversion() != pack.release())
            return false;
    }

    foreach(PdscRequirement r, requirementMap[PdscRequirement::Deny].value(PdscRequirement::Pack))
    {
        if(!r.isValid())
            continue;

        if(r.Pname() == pack.name() && r.Pvendor() == pack.packVendor() && r.Pversion() == pack.release())
            return false;
    }

    //
    // Проверка подходит ли компилятор GCC к данному компоненту
    //
    foreach(PdscRequirement r, requirementMap[PdscRequirement::Deny].value(PdscRequirement::Compiler))
    {
        if(!r.isValid())
            continue;

        if(r.Tcompiler() == "GCC")
            return false;
    }

    foreach(PdscRequirement r, requirementMap[PdscRequirement::Require].value(PdscRequirement::Compiler))
    {
        if(!r.isValid())
            continue;

        if(r.Tcompiler() != "GCC")
            return false;
    }

    return true;
}

//------------------------------------------------------------------------------
// Выводит список файлов компонента, которые соответствуют всем требованиям
//------------------------------------------------------------------------------
QStringList PdscParser::getFilteredFiles(const PackDescription &pack,
                                         const Manufacturer &vendor,
                                         const Family &family,
                                         const Series &series,
                                         const Mcu &device,
                                         PdscComponent &component,
                                         const QList<PdscComponent> &componentList)
{
    QStringList files;
    QStringList allowedCategories =
    {
        "header", "include", "library", "object", "source", "sourceC", "sourceCpp", "sourceAsm", "linkerScript"
    };

    foreach(PdscFile f, component.files())
    {
        if(allowedCategories.contains(f.category().name()))
        {
            //
            // Добавляем сначала файлы без условий
            //
            if(!f.hasCondition() )
            {
                files.append(QString("%1=%2").arg(f.category().name()).arg(f.name()));
            }
            //
            // Проверяем файл на соответствие требованиям
            //
            else
            {
                auto requirementMap = f.condition().requirementsMap();

                // TODO в большинстве случаев это соответствие файла компилятору
                // если на практике окажется, что нужна праверка еще каких-то условий,
                // то доработаю проверку
                foreach (PdscRequirement r, requirementMap[PdscRequirement::Require].value(PdscRequirement::Compiler))
                {
                    if(!r.isValid())
                        continue;

                    if(r.Tcompiler() == "GCC")
                    {
                        files.append(QString("%1=%2").arg(f.category().name()).arg(f.name()));
                        break;
                    }
                }
            }
        }
    }

    return files;
}

