#include "manufacturer.h"

Manufacturer::Manufacturer()
{
    this->id = -1;
}

Manufacturer::Manufacturer(int id, QString name)
{
    this->id = id;
    this->name = name;
}

Manufacturer::Manufacturer(Manufacturer *m)
{
    if(m == nullptr)
        return;

    this->id = m->getId();
    this->name = m->getName();

    for (auto it = m->familyMap.begin(); it != m->familyMap.end(); ++it)
    {
        this->familyMap.insert(it.key(), it.value());
    }
}

//------------------------------------------------------------------------------
// Вернуть все семейства
//------------------------------------------------------------------------------
QMap<QString, Family> &Manufacturer::families()
{
    return this->familyMap;
}

//------------------------------------------------------------------------------
// Проверить имеет ли производитель загруженные семейства
//------------------------------------------------------------------------------
bool Manufacturer::hasFamilies() {return !this->familyMap.isEmpty();}

//------------------------------------------------------------------------------
// Вернуть ID производителя
//------------------------------------------------------------------------------
int Manufacturer::getId() const {return id;}

//------------------------------------------------------------------------------
// Вернуть имя производителя
//------------------------------------------------------------------------------
QString Manufacturer::getName() const {return name;}

//------------------------------------------------------------------------------
// Задать ID производителя
//------------------------------------------------------------------------------
void Manufacturer::setId(int id){this->id = id;}

//------------------------------------------------------------------------------
// Задать название производителя
//------------------------------------------------------------------------------
Manufacturer &Manufacturer::setName(QString name)
{
    this->name = name;
    return *this;
}

//------------------------------------------------------------------------------
// Количество загруженных семейств
//------------------------------------------------------------------------------
int Manufacturer::getFamiliesCount(){return this->familyMap.count();}

//------------------------------------------------------------------------------
// Список загруженных семейств
//------------------------------------------------------------------------------
QStringList Manufacturer::getFamiliesKeys(){return this->familyMap.keys();}

//------------------------------------------------------------------------------
// Найти семейство по идентификатору
//------------------------------------------------------------------------------
Family Manufacturer::getFamilyById(int i)
{
    Family fam;
    QMap<QString,Family>::iterator famIterator = familyMap.begin();

    while(famIterator != familyMap.end())
    {
        Family currFamily = famIterator.value();

        if(currFamily.getId() == i)
        {
            fam = currFamily;
            break;
        }

        ++famIterator;
    }

    return fam;
}

//------------------------------------------------------------------------------
// Найти или создать новое семейство по имени
//------------------------------------------------------------------------------
Family &Manufacturer::family(const QString &name)
{
    if(this->familyMap.contains(name))
        return this->familyMap[name];
    else
        return this->createNewFamily(name);
}

//------------------------------------------------------------------------------
// Найти или создать новое семейство по имени
//------------------------------------------------------------------------------
Family &Manufacturer::addFamily(const QString &name)
{
    return this->family(name).setName(name);
}

//------------------------------------------------------------------------------
// Возврат списка svd-файлов
//------------------------------------------------------------------------------
QList<Manufacturer::SvdInfo> &Manufacturer::svdList()
{
    return this->_svdList;
}

//------------------------------------------------------------------------------
// Возврат конкретного SVD
//------------------------------------------------------------------------------
Manufacturer::SvdInfo *Manufacturer::svd(const QString &path)
{
    for(int i = 0; i < _svdList.count(); i++)
    {
        Manufacturer::SvdInfo s = _svdList.at(i);

        if(s.pathInArchive == path)
            return &_svdList[i];
    }

    return nullptr;
}

//------------------------------------------------------------------------------
// Проверка валидности объекта Manufacturer
//------------------------------------------------------------------------------
bool Manufacturer::isValid()
{
    return this->id > 0 && !this->name.isEmpty();
}

//------------------------------------------------------------------------------
// Создание нового семейства
//------------------------------------------------------------------------------
Family &Manufacturer::createNewFamily(const QString &name)
{
    this->familyMap.insert(name, Family());
    return this->familyMap[name].setName(name);
}
