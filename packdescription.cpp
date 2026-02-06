#include <QDebug>
#include "packdescription.h"

PackDescription::PackDescription()
{

}

QString PackDescription::vendor()
{
    return this->_vendor;
}

QString PackDescription::name()
{
    return this->_name;
}

QString PackDescription::url()
{
    return this->_url;
}

QString PackDescription::description()
{
    return this->_description;
}

void PackDescription::setVendor(QString vendor)
{
   this->_vendor = vendor;
}

void PackDescription::setName(QString name)
{
   this->_name = name;
}

void PackDescription::setUrl(QString url)
{
   this->_url = url;
}

void PackDescription::setDescription(QString description)
{
    this->_description = description;
}

void PackDescription::printInfo()
{
    qInfo() << "Name: " << name();
    qInfo() << "Vendor: " << vendor();
    qInfo() << "Description: " << description();
    qInfo() << "Url: " << url();
}
