#ifndef FEATURECONTAINER_H
#define FEATURECONTAINER_H

#include <QString>
#include <QStringList>
#include <QMap>
#include <QList>
#include "devicefeature.h"

class FeatureContainer
{
private:
    QList<DeviceFeature> _featureList;

public:
    FeatureContainer();

    QList<DeviceFeature>& features();
    void addFeature(const QString& type);
    void addFeature(const DeviceFeature& feature);
    QStringList featuresSummary();
    QString coFeaturesSummary() const;

private:

};

#endif // FEATURECONTAINER_H
