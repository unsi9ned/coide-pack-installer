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
    QMap<int, Family> familiesMap;

public:

    //Конструкторы
    //--------------------------------------------------------------------------
    Manufacturer(int id = -1)
    {
        this->id = id;
    }
    Manufacturer(int id, QString name)
    {
        this->id = id;
        this->name = name;
    }

    Manufacturer(const Manufacturer &m)
    {
        this->id = m.getId();
        this->name = m.getName();
        this->familiesMap = m.getFamiliesMap();
    }

    Manufacturer(Manufacturer *m)
    {
        this->id = m->getId();
        this->name = m->getName();
        this->familiesMap = m->getFamiliesMap();
    }

    //Конструкторы
    //--------------------------------------------------------------------------
    void operator =(const Manufacturer &m)
    {
        this->id = m.getId();
        this->name = m.getName();
        this->familiesMap = m.getFamiliesMap();
    }


    //Геттеры/сеттеры
    //--------------------------------------------------------------------------
    int getId() const {return id;}
    QString getName() const {return name;}
    QMap<int, Family> getFamiliesMap() const
    {
        return this->familiesMap;
    }

    void setId(int id){this->id = id;}
    void setName(QString name){this->name = name;}



    //Узнать количество семейств в списке
    int getFamiliesCount(){return this->familiesMap.count();}

    //Вернуть идентификаторы
    QList<int> getFamiliesKeys(){return this->familiesMap.keys();}

    //Найти семейство по идентификатору
    Family getFamily(int i)
    {
        return (familiesMap.contains(i)) ? familiesMap.value(i) : Family();
    }

    //Добавить семейство
    void addFamily(Family f){this->familiesMap.insert(f.getId(), f);}

};

#endif // MANUFACTURER_H
