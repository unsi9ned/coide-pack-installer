#ifndef FAMILY_H
#define FAMILY_H

#include <QObject>
#include <QString>
#include <QList>

#include "serie.h"

class Family
{
private:

    int id;
    QString familyName;
    int manufacturerId;

    QMap<int, Serie> seriesMap;

public:

    //Конструкторы
    //--------------------------------------------------------------------------
    Family(int id = -1)
    {
        this->id = id;
    }

    Family(int id, QString familyName, int manufacturerId)
    {
        this->id = id;
        this->familyName = familyName;
        this->manufacturerId = manufacturerId;
    }

    Family(const Family &f)
    {
        this->id = f.getId();
        this->familyName = f.getFamilyName();
        this->manufacturerId = f.getManufacturerId();
        this->seriesMap = f.getSeriesMap();
    }

    Family(Family *f)
    {
        this->id = f->getId();
        this->familyName = f->getFamilyName();
        this->manufacturerId = f->getManufacturerId();
        this->seriesMap = f->getSeriesMap();
    }

    //Операторы
    //--------------------------------------------------------------------------
    void operator =(const Family &f)
    {
        this->id = f.getId();
        this->familyName = f.getFamilyName();
        this->manufacturerId = f.getManufacturerId();
        this->seriesMap = f.getSeriesMap();
    }


    //Геттеры, сеттеры
    //--------------------------------------------------------------------------
    int getId() const {return id;}
    QString getFamilyName() const {return familyName;}
    int getManufacturerId() const {return manufacturerId;}
    QMap<int, Serie> getSeriesMap() const {return this->seriesMap;}

    void setId(int id){this->id = id;}
    void setfamilyName(QString name){this->familyName = name;}
    void setmanufacturerId(int id){this->manufacturerId = id;}



    //Количество серий процессоров в списке
    int getSeriesCount(){return this->seriesMap.count();}

    //Список идентификаторов серий
    QList<int> getSeriesKeys(){return this->seriesMap.keys();}

    //Найти серию по идентификатору
    Serie getSerie(int i)
    {
        return (seriesMap.contains(i)) ? seriesMap.value(i) : Serie();
    }

    //Найти серию по имени
    Serie getSerie(QString name)
    {
        Serie s;
        QMap<int,Serie>::iterator serieIterator = seriesMap.begin();

        while(serieIterator != seriesMap.end())
        {
            Serie currSerie = serieIterator.value();

            if(currSerie.getSerieName() == name)
            {
                s = currSerie;
                break;
            }

            ++serieIterator;
        }

        return s;
    }

    void addSerie(const Serie &s){this->seriesMap.insert(s.getId(), s);}

};

#endif // FAMILY_H
