#ifndef ALGORITHMCONTAINER_H
#define ALGORITHMCONTAINER_H

#include <QString>
#include <QList>
#include "progalgorithm.h"

class AlgorithmContainer
{
private:
    QList<ProgAlgorithm> _algorithmList;

public:
    AlgorithmContainer();

    QList<ProgAlgorithm>& algorithms();
    void addAlgorithm(const QString& name);
    void addAlgorithm(const ProgAlgorithm& algorithm);
    ProgAlgorithm * getFlashAlgorithm(qint32 flashStartAddr = 0x0L);
    bool hasAlgorithms();
};

#endif // ALGORITHMCONTAINER_H
