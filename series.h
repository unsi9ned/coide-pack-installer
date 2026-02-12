#ifndef SERIE_H
#define SERIE_H

#include <QObject>
#include <QString>
#include <QList>
#include <QMap>

#include "mcu.h"
#include "featurecontainer.h"
#include "algorithmcontainer.h"

class Series : public FeatureContainer
{

private:

    int id;
    QString _name;
    int familyId;

    QMap<QString, Mcu> _mcuMap;

public:

    Series()
    {
        this->id = -1;
    }

    Series(int id, QString serieName, int familyId)
    {
        this->id = id;
        this->_name = serieName;
        this->familyId = familyId;
    }

    Series(Series * s)
    {
        if(s == nullptr)
            return;

        this->id = s->getId();
        this->_name = s->getName();
        this->familyId = s->getFamilyId();

        for(auto it = s->mcuMap().begin(); it != s->mcuMap().end(); ++it)
        {
            this->_mcuMap.insert(it.key(), it.value());
        }
    }

    int getId() const {return id;}
    QString getName() const {return _name;}
    int getFamilyId() const {return familyId;}

    void setId(int id){this->id = id;}

    Series& setName(QString name)
    {
        this->_name = name;
        return *this;
    }

    void setFamilyId(int id){this->familyId = id;}

    QMap<QString, Mcu>& mcuMap()
    {
        return _mcuMap;
    }

    bool hasDevices(){return !this->_mcuMap.isEmpty();}

    //Узнать количество процессоров в списке
    int getMcuCount(){return this->_mcuMap.count();}

    //Получить список идентификаторов процессоров данной серии
    QStringList getMcuKeys(){return this->_mcuMap.keys();}

    //Найти процессор по идентификатору
    Mcu getMcuById(int i)
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

    //Найти процессор по названию
    Mcu& mcu(QString name)
    {
        if(_mcuMap.contains(name))
            return _mcuMap[name];
        else
            return createNewMcu(name);
    }

    //Добавить процессор в список
    Mcu& addMcu(const QString& name)
    {
        return this->mcu(name).setName(name);
    }

    bool isValid()
    {
        return this->id > 0 && !this->_name.isEmpty();
    }

private:
    Mcu& createNewMcu(const QString& mcuUniqueName)
    {
        Mcu newMcu;
        this->_mcuMap.insert(mcuUniqueName, newMcu);
        return this->_mcuMap[mcuUniqueName].setName(mcuUniqueName);
    }
};

#endif // SERIE_H
