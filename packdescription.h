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

    QMap<int, Manufacturer> _devManufacturers;

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

    void printInfo();
};

#endif // PACKDESCRIPTION_H
