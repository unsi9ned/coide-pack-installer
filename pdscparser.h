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

    bool parse(const QString& pathToPdsc, PackDescription& pack);

private:
    void parseDomDocument(QDomDocument * doc, PackDescription& pack);
    void parseDevices(const QDomNode& node, PackDescription& pack);
};

#endif // PDSCPARSER_H
