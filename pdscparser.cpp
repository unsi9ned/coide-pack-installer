#include <QApplication>
#include <QDebug>
#include <QFile>
#include <QDomText>
#include <QDomNode>
#include <QDomNodeList>
#include <QList>
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
bool PdscParser::parse(const QString &pathToPdsc, PackDescription &pack)
{
    QFile pdscFile(pathToPdsc);

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
        else if(nodeName == "devices")
        {
            parseDevFamilies(node, pack);
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
                        QString coreName;

                        if(devProcessor.isEmpty())
                            if(subFamilyProcessor.isEmpty())
                                coreName = familyProcessor;
                            else
                                coreName = subFamilyProcessor;
                        else
                            coreName = devProcessor;

                        Mcu& newMcu = parseDevice(node, vendorInfo, coreName, subFamilyName, pack);

                        if(!newMcu.hasAlgorithms())
                            foreach(ProgAlgorithm a, subFamilyAlgorithms)
                                newMcu.addAlgorithm(a);
                    }
                    else if(nodeName == "feature")
                    {
                        QString coreName;
                        DeviceFeature devFeature = parseFeature(node.toElement());

                        coreName = subFamilyProcessor.isEmpty() ? familyProcessor : subFamilyProcessor;
                        pack.vendorByDvendor(vendorInfo).family(coreName).series(subFamilyName).addFeature(devFeature);
                    }
                    else if(nodeName == "algorithm")
                    {
                        QString coreName;
                        ProgAlgorithm subFamilyAlgorithm = parseAlgorithm(node.toElement());

                        coreName = subFamilyProcessor.isEmpty() ? familyProcessor : subFamilyProcessor;
                        subFamilyAlgorithms.append(subFamilyAlgorithm);
                    }
                }
            }
            else if(nodeName == "device")
            {
                QString devProcessor = node.namedItem("processor").attributes().namedItem("Dcore").nodeValue();
                QString coreName;

                coreName = devProcessor.isEmpty() ? familyProcessor : devProcessor;
                Mcu& newMcu = parseDevice(node, vendorInfo, coreName, familyName, pack);

                if(!newMcu.hasAlgorithms())
                    foreach(ProgAlgorithm a, coreAlgorithms)
                        newMcu.addAlgorithm(a);
            }
            else if(nodeName == "feature" && !familyProcessor.isEmpty())
            {
                DeviceFeature devFeature = parseFeature(node.toElement());
                pack.vendorByDvendor(vendorInfo).family(familyProcessor).addFeature(devFeature);
            }
            else if(nodeName == "algorithm")
            {
                ProgAlgorithm familyAlgorithm = parseAlgorithm(node.toElement());
                coreAlgorithms.append(familyAlgorithm);
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
    // Парсинг описания
    //
    if(!deviceNode.namedItem("description").isNull() &&
        deviceNode.namedItem("description").hasChildNodes())
    {
        newMcu.setDescription(deviceNode.namedItem("description").firstChild().nodeValue());
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
