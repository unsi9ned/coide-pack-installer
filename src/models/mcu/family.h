#ifndef FAMILY_H
#define FAMILY_H

#include <QObject>
#include <QString>
#include <QList>

#include "series.h"
#include "featurecontainer.h"
#include "models/algorithms/algorithmcontainer.h"
#include "devicehierarchynode.h"

class Manufacturer;

class Family : public DeviceHierarchyNode
{
private:

    int id;
    QString _name;
    int manufacturerId;

    QMap<QString, Series> _seriesMap;
    Manufacturer * m_parent;

public:

    Family();
    Family(int id, QString familyName, int manufacturerId);
    Family(Family *f);


    int getId() const;
    qint32 getCoMaxId() const {return ID_FAMILY_LAST; }
    QString getName() const;
    QString getCoName() const;
    int getManufacturerId() const;

    QMap<QString, Series>& seriesMap();
    const QMap<QString, Series>& seriesMapConst() const;

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
    const Series& constSeries(const QString& name) const;

    Series& addSeries(const QString& name);

    void setParent(Manufacturer* parent);
    Manufacturer * getParent();
    bool hasParent() const;
    const Manufacturer& constParent() const;
    const Manufacturer& constVendor() const;

    QString getPath() const;

    bool isValid(QString * errorString = nullptr) const;
    bool isNull() const;

private:
    bool isValid(QString& errorString) const;
    Series& createNewSeries(const QString& seriesName);
};

#endif // FAMILY_H
