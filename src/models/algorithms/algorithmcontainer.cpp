#include "algorithmcontainer.h"

AlgorithmContainer::AlgorithmContainer()
{

}

QList<ProgAlgorithm> &AlgorithmContainer::algorithms()
{
    return _algorithmList;
}

void AlgorithmContainer::addAlgorithm(const QString &name)
{
    ProgAlgorithm algo(name);
    this->_algorithmList.append(algo);
}

void AlgorithmContainer::addAlgorithm(const ProgAlgorithm &algorithm)
{
    this->_algorithmList.append(algorithm);
}

ProgAlgorithm *AlgorithmContainer::getFlashAlgorithm(qint32 flashStartAddr)
{
    for(auto i = 0; i < _algorithmList.length(); i++)
    {
        ProgAlgorithm* a = &_algorithmList[i];

        if(a->isDefault())
        {
            if(flashStartAddr >= 0 && a->start() == static_cast<uint32_t>(flashStartAddr))
            {
                return a;
            }
            else if(flashStartAddr < 0 && a->name().toLower().contains("flash"))
            {
                return a;
            }
        }
    }

    return nullptr;
}

bool AlgorithmContainer::hasAlgorithms()
{
    return !_algorithmList.isEmpty();
}
