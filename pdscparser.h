#ifndef PDSCPARSER_H
#define PDSCPARSER_H

#include <QString>
#include <QDomDocument>
#include "packdescription.h"

class PdscParser
{
private:
    QString _lastErrorStr;

public:
    PdscParser();

    bool parse(PackDescription& pack);

private:
    void parseDomDocument(QDomDocument * doc, PackDescription& pack);
    void parseDevFamilies(const QDomNode& node, PackDescription& pack);
    Mcu& parseDevice(const QDomNode& deviceNode,
                     const QString& vendor,
                     const QString& processor,
                     const QString& series,
                     PackDescription& pack);
    DeviceFeature parseFeature(const QDomElement &featureElem);
    ProgAlgorithm parseAlgorithm(const QDomElement &algorithmElement);
};

#endif // PDSCPARSER_H
