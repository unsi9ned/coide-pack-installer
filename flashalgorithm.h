#ifndef FLASHALGORITHM_H
#define FLASHALGORITHM_H

#include <QObject>
#include <QString>
#include <QByteArray>

class FlashAlgorithm
{

private:

    int id;
    QString name;

public:
    FlashAlgorithm(int id = -1){this->id = id;}
    FlashAlgorithm(int id, QString name)
    {
        this->id = id;
        this->name = name;
    }

    FlashAlgorithm(const FlashAlgorithm &fa)
    {
        this->id = fa.getId();
        this->name = fa.getName();
    }

    FlashAlgorithm(FlashAlgorithm *fa)
    {
        this->id = fa->getId();
        this->name = fa->getName();
    }

    void operator =(const FlashAlgorithm &fa)
    {
        this->id = fa.getId();
        this->name = fa.getName();
    }

    void setId(int id){this->id = id;}
    void setName(QString name){this->name = name;}

    int getId() const {return this->id;}
    QString getName() const {return this->name;}
};

#endif // FLASHALGORITHM_H
