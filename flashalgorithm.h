#ifndef FLASHALGORITHM_H
#define FLASHALGORITHM_H

#include <QObject>
#include <QString>
#include <QByteArray>

class FlashAlgorithm
{

private:

    int     _id;
    QString _name;

public:
    FlashAlgorithm()
    {
        this->_id = -1;
    }

    FlashAlgorithm(int id, QString name)
    {
        this->_id = id;
        this->_name = name;
    }

    FlashAlgorithm(const FlashAlgorithm &fa)
    {
        this->_id = fa.getId();
        this->_name = fa.getName();
    }

    FlashAlgorithm(FlashAlgorithm *fa)
    {
        this->_id = fa->getId();
        this->_name = fa->getName();
    }

    void operator =(const FlashAlgorithm &fa)
    {
        this->_id = fa.getId();
        this->_name = fa.getName();
    }

    void setId(int id){this->_id = id;}
    void setName(QString name){this->_name = name;}

    int getId() const {return this->_id;}
    QString getName() const {return this->_name;}

    bool isNull()
    {
        return this->_id <= 0 || this->_name.isEmpty();
    }
};

#endif // FLASHALGORITHM_H
