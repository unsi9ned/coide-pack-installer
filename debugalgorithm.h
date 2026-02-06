#ifndef DEDUGALGORITHM_H
#define DEDUGALGORITHM_H

#include <QObject>
#include <QString>
#include <QByteArray>

class DebugAlgorithm
{

private:

    int id;
    QString name;

public:
    DebugAlgorithm(int id = -1)
    {
        this->id = id;
    }

    DebugAlgorithm(int id, QString name)
    {
        this->id = id;
        this->name = name;
    }

    DebugAlgorithm(const DebugAlgorithm &da)
    {
        this->id = da.getId();
        this->name = da.getName();
    }

    DebugAlgorithm(DebugAlgorithm *da)
    {
        this->id = da->getId();
        this->name = da->getName();
    }

    void operator =(const DebugAlgorithm &da)
    {
        this->id = da.getId();
        this->name = da.getName();
    }



    void setId(int id){this->id = id;}
    void setName(QString name){this->name = name;}

    int getId() const {return this->id;}
    QString getName() const {return this->name;}
};

#endif // DEDUGALGORITHM_H
