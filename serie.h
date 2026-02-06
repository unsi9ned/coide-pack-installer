#ifndef SERIE_H
#define SERIE_H

#include <QObject>
#include <QString>
#include <QList>
#include <QMap>

#include "mcu.h"

class Serie
{

private:

    int id;
    QString serieName;
    int familyId;

    QMap<int, Mcu> processorsMap;

public:

    //Конструкторы
    //--------------------------------------------------------------------------
    Serie(int id = -1)
    {
        this->id = id;
    }

    Serie(int id, QString serieName, int familyId)
    {
        this->id = id;
        this->serieName = serieName;
        this->familyId = familyId;
    }

    Serie(const Serie &s)
    {
        this->id = s.getId();
        this->serieName = s.getSerieName();
        this->familyId = s.getFamilyId();
        this->processorsMap = s.getProcessorsMap();
    }

    Serie(Serie * s)
    {
        this->id = s->getId();
        this->serieName = s->getSerieName();
        this->familyId = s->getFamilyId();
        this->processorsMap = s->getProcessorsMap();
    }

    //Операторы
    //--------------------------------------------------------------------------
    void operator =(const Serie &s)
    {
        this->id = s.getId();
        this->serieName = s.getSerieName();
        this->familyId = s.getFamilyId();
        this->processorsMap = s.getProcessorsMap();
    }

    //Геттеры, сеттеры
    //--------------------------------------------------------------------------
    int getId() const {return id;}
    QString getSerieName() const {return serieName;}
    int getFamilyId() const {return familyId;}

    void setId(int id){this->id = id;}
    void setSeriesName(QString name){this->serieName = name;}
    void setSeriesName(int id){this->familyId = id;}
    QMap<int, Mcu> getProcessorsMap() const {return processorsMap;}



    //Узнать количество процессоров в списке
    int getMcuCount(){return this->processorsMap.count();}

    //Получить список идентификаторов процессоров данной серии
    QList<int> getMcuKeys(){return this->processorsMap.keys();}

    //Найти процессор по идентификатору
    Mcu getMcu(int i)
    {
        return (processorsMap.contains(i)) ? processorsMap.value(i) : Mcu();
    }

    //Найти процессор по названию
    Mcu getMcu(QString name)
    {
        Mcu m;
        QMap<int,Mcu>::iterator mcuIterator = processorsMap.begin();

        while(mcuIterator != processorsMap.end())
        {
            Mcu currentMcu = mcuIterator.value();

            if(currentMcu.getName() == name)
            {
                m = currentMcu;
                break;
            }

            ++mcuIterator;
        }

        return m;
    }

    //Добавить процессор в список
    void addMcu(const Mcu &m){this->processorsMap.insert(m.getId(), m);}

};

#endif // SERIE_H
