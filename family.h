#ifndef FAMILY_H
#define FAMILY_H

#include <QObject>
#include <QString>
#include <QList>

#include "series.h"
#include "featurecontainer.h"

class Family : public FeatureContainer
{
private:

    int id;
    QString _name;
    int manufacturerId;

    QMap<QString, Series> _seriesMap;

public:

    //Конструкторы
    //--------------------------------------------------------------------------
    Family()
    {
        this->id = -1;
    }

    Family(int id, QString familyName, int manufacturerId)
    {
        this->id = id;
        this->_name = familyName;
        this->manufacturerId = manufacturerId;
    }

    Family(Family *f)
    {
        if(f == nullptr)
            return;

        this->id = f->getId();
        this->_name = f->getName();
        this->manufacturerId = f->getManufacturerId();

        for(auto it = f->seriesMap().begin(); it != seriesMap().end(); ++it)
        {
            this->_seriesMap.insert(it.key(), it.value());
        }
    }


    int getId() const {return id;}
    QString getName() const {return _name;}
    int getManufacturerId() const {return manufacturerId;}

    QMap<QString, Series>& seriesMap()
    {
        return this->_seriesMap;
    }

    void setId(int id){this->id = id;}
    void setManufacturerId(int id){this->manufacturerId = id;}

    Family& setName(QString name)
    {
        this->_name = name;
        return *this;
    }

    //Количество серий процессоров в списке
    int getSeriesCount(){return this->_seriesMap.count();}

    //Список идентификаторов серий
    QStringList getSeriesKeys(){return this->_seriesMap.keys();}

    //Найти серию по идентификатору
    Series getSeriesById(int i)
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

    // Вернуть серию по имени
    Series& series(QString name)
    {
        if(_seriesMap.contains(name))
            return _seriesMap[name];
        else
            return createNewSeries(name);
    }

    Series& addSeries(const QString& name)
    {
        return this->series(name).setName(name);
    }

    bool isValid()
    {
        return this->id > 0 && !this->_name.isEmpty();
    }

private:
    Series& createNewSeries(const QString& seriesName)
    {
        this->_seriesMap.insert(seriesName, Series());
        return this->_seriesMap[seriesName].setName(seriesName);
    }
};

#endif // FAMILY_H
