#ifndef MANUFACTURER_H
#define MANUFACTURER_H

#include <QObject>
#include <QString>
#include <QList>

#include "family.h"

class Manufacturer
{
private:

    int id;
    QString name;
    QMap<QString, Family> familyMap;

public:

    //Конструкторы
    //--------------------------------------------------------------------------
    Manufacturer()
    {
        this->id = -1;
    }
    Manufacturer(int id, QString name)
    {
        this->id = id;
        this->name = name;
    }

    Manufacturer(Manufacturer* m)
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

    QMap<QString, Family>& families()
    {
        return this->familyMap;
    }

    int getId() const {return id;}
    QString getName() const {return name;}

    void setId(int id){this->id = id;}

    Manufacturer& setName(QString name)
    {
        this->name = name;
        return *this;
    }



    //Узнать количество семейств в списке
    int getFamiliesCount(){return this->familyMap.count();}

    //Вернуть идентификаторы
    QStringList getFamiliesKeys(){return this->familyMap.keys();}

    //Найти семейство по идентификатору
    Family getFamilyById(int i)
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

    Family& family(const QString& name)
    {
        if(this->familyMap.contains(name))
            return this->familyMap[name];
        else
            return this->createNewFamily(name);
    }

    //Добавить семейство
    Family& addFamily(const QString& name)
    {
        return this->family(name).setName(name);
    }

    bool isValid()
    {
        return this->id > 0 && !this->name.isEmpty();
    }

private:

    Family& createNewFamily(const QString& name)
    {
        this->familyMap.insert(name, Family());
        return this->familyMap[name].setName(name);
    }
};

#endif // MANUFACTURER_H
