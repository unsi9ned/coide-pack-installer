#ifndef PACKDESCRIPTION_H
#define PACKDESCRIPTION_H

#include <QString>
#include <QMap>
#include "manufacturer.h"

class PackDescription
{
private:
    QString _vendor;
    QString _name;
    QString _url;
    QString _description;

    QMap<QString, Manufacturer> _vendorMap;

public:
    PackDescription();

    QString vendor();
    QString name();
    QString url();
    QString description();

    void setVendor(QString vendor);
    void setName(QString name);
    void setUrl(QString url);
    void setDescription(QString description);

    Manufacturer& vendor(const QString& vendorName);
    QMap<QString, Manufacturer>& vendors();

    void printInfo();

private:
    Manufacturer& createNewVendor(const QString& vendorName);
};

#endif // PACKDESCRIPTION_H
