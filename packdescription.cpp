#include <QDebug>
#include <iostream>
#include <iomanip>
#include "packdescription.h"

PackDescription::PackDescription()
{

}

QString PackDescription::packVendor() const
{
    return this->_vendor;
}

QString PackDescription::name() const
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

QString PackDescription::release() const
{
    return _release;
}

QString PackDescription::pathToArchive() const
{
    return _pathToArchive;
}

QString PackDescription::installDir() const
{
    return _installDir;
}

QString PackDescription::pathToPdsc() const
{
    return _pathToPdsc;
}

QString PackDescription::pathToSVD(const Mcu &device)
{
    return installDir() + "/" + device.svdLocalPath().replace('\\', '/');
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

void PackDescription::setRelease(QString release)
{
    this->_release = release;
}

void PackDescription::setInstallDir(const QString &path)
{
    this->_installDir = path;
}

void PackDescription::setPathToPdsc(const QString &path)
{
    this->_pathToPdsc = path;
}

void PackDescription::setPathToArchive(const QString& path)
{
    this->_pathToArchive = path;
}

Manufacturer &PackDescription::vendor(const QString &vendorName)
{
    return _vendorMap.contains(vendorName) ?
           _vendorMap[vendorName] :
                createNewVendor(vendorName);
}

Manufacturer &PackDescription::vendorByDvendor(const QString &Dvendor)
{
    QStringList vendorParts = Dvendor.split(":") << "unknown" << "-1";
    QString vendorName = vendorParts.at(0);
    int vendorId = vendorParts.at(1).toInt();

    Manufacturer& m = vendor(vendorName);

    if(m.getId() == -1)
    {
        m.setId(vendorId);
    }

    return m;
}

QMap<QString, Manufacturer> &PackDescription::vendors()
{
    return this->_vendorMap;
}

void PackDescription::printInfo()
{
    // Устанавливаем выравнивание влево для всех
    std::cout << std::left;

    std::cout << std::setw(13) << "Name:"        << name().toStdString() << std::endl;
    std::cout << std::setw(13) << "Vendor:"      << packVendor().toStdString() << std::endl;
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

//------------------------------------------------------------------------------
// Проверка пакета на валидность
//------------------------------------------------------------------------------
bool PackDescription::isValid() const
{
    return true;
}

QMap<QString, Component> &PackDescription::components()
{
    return _componentMap;
}

QMap<QString, QStringList> &PackDescription::coComponentMap()
{
    return _coComponentMap;
}

//------------------------------------------------------------------------------
// Очистка содержимого пакета, всех полей и списков
//------------------------------------------------------------------------------
void PackDescription::clear()
{
    _vendor.clear();
    _name.clear();
    _url.clear();
    _description.clear();
    _release.clear();
    _pathToArchive.clear();
    _installDir.clear();
    _pathToPdsc.clear();
    _vendorMap.clear();
    _componentMap.clear();
    _coComponentMap.clear();
}

Manufacturer &PackDescription::createNewVendor(const QString &vendorName)
{
    _vendorMap.insert(vendorName, Manufacturer());
    return _vendorMap[vendorName].setName(vendorName);
}
