#include "series.h"
#include "family.h"
#include "manufacturer.h"

Series::Series()
{
    this->id = -1;
    this->familyId = -1;
    this->m_parent = nullptr;
}

Series::Series(int id, QString serieName, int familyId)
{
    this->id = id;
    this->_name = serieName;
    this->familyId = familyId;
    this->m_parent = nullptr;
}

Series::Series(Series *s)
{
    if(s == nullptr)
        return;

    this->id = s->getId();
    this->_name = s->getName();
    this->familyId = s->getFamilyId();
    this->m_parent = nullptr;

    for(auto it = s->mcuMap().begin(); it != s->mcuMap().end(); ++it)
    {
        this->_mcuMap.insert(it.key(), it.value());
    }
}

int Series::getId() const {return id;}

QString Series::getName() const {return _name;}

int Series::getFamilyId() const {return familyId;}

//------------------------------------------------------------------------------
// Установка идентификатора серии и обновлении его во всех дочерних элементах
//------------------------------------------------------------------------------
void Series::setId(int id)
{
    this->id = id;

    for(auto i = _mcuMap.begin(); i != _mcuMap.end(); ++i)
    {
        Mcu& m = i.value();
        m.setSeriesId(id);
    }
}

Series &Series::setName(QString name)
{
    this->_name = name;
    return *this;
}

void Series::setFamilyId(int id){this->familyId = id;}

QMap<QString, Mcu> &Series::mcuMap()
{
    return _mcuMap;
}

const QMap<QString, Mcu>&Series::mcuMapConst() const
{
    return _mcuMap;
}

bool Series::hasDevices(){return !this->_mcuMap.isEmpty();}

int Series::getMcuCount(){return this->_mcuMap.count();}

QStringList Series::getMcuKeys(){return this->_mcuMap.keys();}

Mcu Series::getMcuById(int i)
{
    Mcu m;
    QMap<QString,Mcu>::iterator mcuIterator = _mcuMap.begin();

    while(mcuIterator != _mcuMap.end())
    {
        Mcu currentMcu = mcuIterator.value();

        if(currentMcu.getId() == i)
        {
            m = currentMcu;
            break;
        }

        ++mcuIterator;
    }

    return m;
}

Mcu &Series::mcu(QString name)
{
    if(_mcuMap.contains(name))
        return _mcuMap[name];
    else
        return createNewMcu(name);
}

//------------------------------------------------------------------------------
// Гарантированно возвращает ссылку на объект mcu, даже если такого нет в списке
//------------------------------------------------------------------------------
const Mcu&Series::constMcu(const QString& name) const
{
    static Mcu nullMcu;
    auto it = _mcuMap.find(name);
    return it != _mcuMap.end() ? *it : nullMcu;
}

Mcu &Series::addMcu(const QString &name)
{
    return this->mcu(name).setName(name);
}

Mcu &Series::addMcu(const Mcu &m)
{
    Mcu& newMcu = mcu(m.getName());
    newMcu = m;
    newMcu.setSeriesId(this->id);
    newMcu.setParent(this);
    return newMcu;
}

void Series::setParent(Family* parent)
{
    m_parent = parent;
}

Family*Series::getParent()
{
    return m_parent;
}

bool Series::hasParent() const
{
    return m_parent != nullptr;
}

const Family&Series::constParent() const
{
    static Family nullFamily;
    return m_parent ? * m_parent : nullFamily;
}

const Family&Series::constFamily() const
{
    return constParent();
}

QString Series::getPath() const
{
    return constFamily().getPath() + "/" + getName();
}

//------------------------------------------------------------------------------
// Проверка объекта на NULL
//------------------------------------------------------------------------------
bool Series::isNull() const
{
    return this->id == -1 &&
           this->familyId == -1 &&
           this->_name.isEmpty() &&
           this->_mcuMap.isEmpty();
}

//------------------------------------------------------------------------------
// Проверка объекта на валидность
//------------------------------------------------------------------------------
bool Series::isValid(QString *errorString) const
{
    QString e;

    if(errorString)
        return isValid(*errorString);
    else
        return isValid(e);
}

bool Series::isValid(QString &errorString) const
{
    if(this->familyId == -1)
        errorString = QString("Family id = %1 is not valid").arg(this->familyId);
    else if(this->_name.isEmpty())
        errorString= "Series name is not defined";
    else
        return true;

    return false;
}

//------------------------------------------------------------------------------
// Добавление нового устройства в семейство
//------------------------------------------------------------------------------
Mcu &Series::createNewMcu(const QString &mcuUniqueName)
{
    this->_mcuMap.insert(mcuUniqueName, Mcu());
    Mcu& newMcu = this->_mcuMap[mcuUniqueName];
    newMcu.setName(mcuUniqueName);
    newMcu.setSeriesId(getId());
    newMcu.setParent(this);
    return newMcu;
}
