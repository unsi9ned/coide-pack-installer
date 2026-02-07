#ifndef DEDUGALGORITHM_H
#define DEDUGALGORITHM_H

#include <QObject>
#include <QString>
#include <QByteArray>

class DebugAlgorithm
{

private:

    int     _id;
    QString _name;

public:
    DebugAlgorithm()
    {
        this->_id = -1;
    }

    DebugAlgorithm(int id, QString name)
    {
        this->_id = id;
        this->_name = name;
    }

    DebugAlgorithm(const DebugAlgorithm &da)
    {
        this->_id = da.getId();
        this->_name = da.getName();
    }

    DebugAlgorithm(DebugAlgorithm *da)
    {
        this->_id = da->getId();
        this->_name = da->getName();
    }

    void operator =(const DebugAlgorithm &da)
    {
        this->_id = da.getId();
        this->_name = da.getName();
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

#endif // DEDUGALGORITHM_H
