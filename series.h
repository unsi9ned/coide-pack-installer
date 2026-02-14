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

    Series();

    Series(int id, QString serieName, int familyId);

    Series(Series * s);

    int getId() const;
    QString getName() const;
    int getFamilyId() const;

    void setId(int id);

    Series& setName(QString name);

    void setFamilyId(int id);

    QMap<QString, Mcu>& mcuMap();

    bool hasDevices();

    //Узнать количество процессоров в списке
    int getMcuCount();

    //Получить список идентификаторов процессоров данной серии
    QStringList getMcuKeys();

    //Найти процессор по идентификатору
    Mcu getMcuById(int i);

    //Найти процессор по названию
    Mcu& mcu(QString name);

    //Добавить процессор в список
    Mcu& addMcu(const QString& name);

    Mcu& addMcu(const Mcu& m);

    bool isValid(QString * errorString = nullptr) const;
    bool isNull() const;

private:
    bool isValid(QString& errorString) const;
    Mcu& createNewMcu(const QString& mcuUniqueName);
};

#endif // SERIE_H
