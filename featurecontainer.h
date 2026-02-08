#ifndef FEATURECONTAINER_H
#define FEATURECONTAINER_H

#include <QString>
#include <QStringList>
#include <QMap>
#include "devicefeature.h"

class FeatureContainer
{
private:
    QMap<QString, DeviceFeature>  _featureMap;

public:
    FeatureContainer();

    QMap<QString, DeviceFeature>& features();
    DeviceFeature& feature(const QString& type);
    DeviceFeature& addFeature(const QString& type);
    DeviceFeature& addFeature(const DeviceFeature& feature);
    QStringList featuresSummary();

private:
    DeviceFeature& createFeature(const QString& type);
};

#endif // FEATURECONTAINER_H
