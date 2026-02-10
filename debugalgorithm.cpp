#include "debugalgorithm.h"

const QMap<QString, QString> DebugAlgorithm::coDebugAlgorithmsMap = {
    {"Cortex-M0",  "cortex-m0.xml"},
    {"Cortex-M0+", "cortex-m0plus.xml"},
    {"Cortex-M3",  "cortex-m3.xml"},
    {"Cortex-M4",  "cortex-m4.xml"},
};

DebugAlgorithm::DebugAlgorithm(const QString &name)
{
    this->_coId = -1;
    this->_name = name;
}

void DebugAlgorithm::setCoId(int id){this->_coId = id;}

void DebugAlgorithm::setName(QString name){this->_name = name;}

void DebugAlgorithm::setProcessor(const QString &core)
{
    for(auto it = DebugAlgorithm::coDebugAlgorithmsMap.begin();
             it != DebugAlgorithm::coDebugAlgorithmsMap.end();
           ++it)
    {
        if(it.key() == core)
        {
            _name = it.value();
            break;
        }
    }
}

int DebugAlgorithm::coId() const {return this->_coId;}

QString DebugAlgorithm::name() const {return this->_name;}

bool DebugAlgorithm::isNull()
{
    return this->_coId <= 0 || this->_name.isEmpty();
}
