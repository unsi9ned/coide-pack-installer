#include "debugalgorithm.h"
#include "paths.h"

DebugAlgorithm::DebugAlgorithm(const QString &name)
{
    this->_coId = -1;
    this->_name = name;
}

void DebugAlgorithm::setCoId(int id)
{
    this->_coId = id;
}

void DebugAlgorithm::setName(QString name)
{
    this->_name = name;
}

void DebugAlgorithm::setProcessor(const QString &core)
{
    QString debugAlgotrithmFile = Paths::instance()->coIdeDebugAlgorithmFileName(core);

    if(!debugAlgotrithmFile.isEmpty())
    {
        _name = debugAlgotrithmFile;
    }
}

int DebugAlgorithm::coId() const
{
    return this->_coId;
}

QString DebugAlgorithm::name() const
{
    return this->_name;
}

bool DebugAlgorithm::isNull()
{
    return this->_coId <= 0 || this->_name.isEmpty();
}
