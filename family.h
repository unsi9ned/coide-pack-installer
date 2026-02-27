#ifndef FAMILY_H
#define FAMILY_H

#include <QObject>
#include <QString>
#include <QList>

#include "series.h"
#include "featurecontainer.h"
#include "algorithmcontainer.h"

class Family
{
private:

    int id;
    QString _name;
    int manufacturerId;

    QMap<QString, Series> _seriesMap;

public:

    Family();
    Family(int id, QString familyName, int manufacturerId);
    Family(Family *f);


    int getId() const;
    QString getName() const;
    QString getCoName();
    int getManufacturerId() const;

    QMap<QString, Series>& seriesMap();

    bool hasSeries();

    void setId(int id);
    void setManufacturerId(int id);

    Family &setName(QString name);

    //Количество серий процессоров в списке
    int getSeriesCount();

    //Список идентификаторов серий
    QStringList getSeriesKeys();

    //Найти серию по идентификатору
    Series getSeriesById(int i);

    // Вернуть серию по имени
    Series& series(QString name);

    Series& addSeries(const QString& name);

    bool isValid(QString * errorString = nullptr) const;
    bool isNull() const;

private:
    bool isValid(QString& errorString) const;
    Series& createNewSeries(const QString& seriesName);
};

#endif // FAMILY_H
