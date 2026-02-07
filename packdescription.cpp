#include <QDebug>
#include <iostream>
#include <iomanip>
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

Manufacturer &PackDescription::vendor(const QString &vendorName)
{
    return _vendorMap.contains(vendorName) ?
           _vendorMap[vendorName] :
                createNewVendor(vendorName);
}

QMap<QString, Manufacturer> &PackDescription::vendors()
{
    return this->_vendorMap;
}

void PackDescription::printInfo()
{
//    qInfo() << "Name: " << name();
//    qInfo() << "Vendor: " << vendor();
//    qInfo() << "Description: " << description();
//    qInfo() << "Url: " << url();

    // Устанавливаем выравнивание влево для всех
    std::cout << std::left;

    std::cout << std::setw(13) << "Name:"        << name().toStdString() << std::endl;
    std::cout << std::setw(13) << "Vendor:"      << vendor().toStdString() << std::endl;
    std::cout << std::setw(13) << "Description:" << description().toStdString() << std::endl;
    std::cout << std::setw(13) << "Url:"         << url().toStdString() << std::endl;

    std::cout << std::endl;

    // Шапка таблицы
    std::cout << std::left
              << std::setw(20) << "Device Name"
              << std::setw(25) << "Manufacturer"
              << std::setw(12) << "Core"
              << std::setw(20) << "Series"
              << std::setw(20) << "Memory"
              << std::setw(10) << "Start"
              << std::setw(10) << "Size"
              << std::endl;
    std::cout << std::string(120, '-') << std::endl;

    foreach (Manufacturer vendor, vendors())
    {
        foreach(Family family, vendor.families())
        {
            foreach(Series series, family.seriesMap())
            {
                foreach(Mcu chip, series.mcuMap())
                {
                    std::cout << std::left
                              << std::setw(20) << chip.getName().toStdString()
                              << std::setw(25) << vendor.getName().toStdString()
                              << std::setw(12) << family.getName().toStdString()
                              << std::setw(20) << series.getName().toStdString()
                              << std::endl;

                    foreach(Memory region, chip.memoryRegions())
                    {
                        std::cout << std::left
                                  << std::setw(77) << ""
                                  << std::setw(20) << region.name().toStdString()
                                  << std::setw(10) << QString::number(region.startAddr(), 16).toStdString()
                                  << std::setw(10) << QString::number(region.size(), 16).toStdString()
                                  << std::endl;
                    }
                }
            }
        }
    }
}

Manufacturer &PackDescription::createNewVendor(const QString &vendorName)
{
    _vendorMap.insert(vendorName, Manufacturer());
    return _vendorMap[vendorName].setName(vendorName);
}
