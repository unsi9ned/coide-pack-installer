#include "family.h"
#include "manufacturer.h"

Family::Family()
{
    this->id = -1;
    this->manufacturerId = -1;
    this->m_parent = nullptr;
}

Family::Family(int id, QString familyName, int manufacturerId)
{
    this->id = id;
    this->_name = familyName;
    this->manufacturerId = manufacturerId;
    this->m_parent = nullptr;
}

Family::Family(Family *f)
{
    if(f == nullptr)
        return;

    this->id = f->getId();
    this->_name = f->getName();
    this->manufacturerId = f->getManufacturerId();
    this->m_parent = nullptr;

    for(auto it = f->seriesMap().begin(); it != seriesMap().end(); ++it)
    {
        this->_seriesMap.insert(it.key(), it.value());
    }
}

int Family::getId() const {return id;}

QString Family::getName() const {return _name;}

QString Family::getCoName() const
{
    return QString(_name).replace('-', ' ');
}

int Family::getManufacturerId() const {return manufacturerId;}

QMap<QString, Series> &Family::seriesMap()
{
    return this->_seriesMap;
}

bool Family::hasSeries(){return !this->_seriesMap.isEmpty();}

void Family::setId(int id)
{
    this->id = id;

    for(auto i = _seriesMap.begin(); i != _seriesMap.end(); ++i)
    {
        Series& series = i.value();
        series.setFamilyId(id);
    }
}

void Family::setManufacturerId(int id){this->manufacturerId = id;}

Family &Family::setName(QString name)
{
    this->_name = name;
    return *this;
}

int Family::getSeriesCount(){return this->_seriesMap.count();}

QStringList Family::getSeriesKeys(){return this->_seriesMap.keys();}

Series Family::getSeriesById(int i)
{
    Series s;
    QMap<QString,Series>::iterator seriesIterator = _seriesMap.begin();

    while(seriesIterator != _seriesMap.end())
    {
        Series currSeries = seriesIterator.value();

        if(currSeries.getId() == i)
        {
            s = currSeries;
            break;
        }

        ++seriesIterator;
    }

    return s;
}

Series &Family::series(QString name)
{
    if(_seriesMap.contains(name))
        return _seriesMap[name];
    else
        return createNewSeries(name);
}

//------------------------------------------------------------------------------
// Гарантированно возвращает ссылку на объект серии, даже если ее нет в списке
//------------------------------------------------------------------------------
const Series&Family::constSeries(const QString& name) const
{
    static Series nullSeries;
    auto it = _seriesMap.find(name);
    return it != _seriesMap.end() ? *it : nullSeries;
}

Series &Family::addSeries(const QString &name)
{
    return this->series(name).setName(name);
}

void Family::setParent(Manufacturer* parent)
{
    m_parent = parent;
}

Manufacturer*Family::getParent()
{
    return m_parent;
}

bool Family::hasParent() const
{
    return m_parent != nullptr;
}

const Manufacturer& Family::constParent() const
{
    static const Manufacturer nullManufacturer;
    return m_parent ? *m_parent :nullManufacturer;
}

const Manufacturer&Family::constVendor() const
{
    return constParent();
}

QString Family::getPath() const
{
    return constVendor().getPath() + "/" + getCoName();
}

//------------------------------------------------------------------------------
// Проверка на валидность объекта
//------------------------------------------------------------------------------
bool Family::isValid(QString *errorString) const
{
    QString e;

    if(errorString)
        return isValid(*errorString);
    else
        return isValid(e);
}

bool Family::isValid(QString &errorString) const
{
    if(this->manufacturerId <= 0)
        errorString = QString("Invalid manufacturer id = %1").arg(this->manufacturerId);
    else if(this->_name.isEmpty())
        errorString = QString("The family name is not defined");
    else
        return true;

    return false;
}

//------------------------------------------------------------------------------
// Проверка на нулевой объект
//------------------------------------------------------------------------------
bool Family::isNull() const
{
    return id == -1 && manufacturerId == -1 && _name.isEmpty() && _seriesMap.isEmpty();
}

//------------------------------------------------------------------------------
// Создание серии МК
//------------------------------------------------------------------------------
Series &Family::createNewSeries(const QString &seriesName)
{
    this->_seriesMap.insert(seriesName, Series());
    Series& series = _seriesMap[seriesName];
    series.setName(seriesName);
    series.setFamilyId(this->getId());
    series.setParent(this);
    return series;
}
