#include "featurecontainer.h"

FeatureContainer::FeatureContainer()
{

}

//------------------------------------------------------------------------------
// Возвращает карту свойств
//------------------------------------------------------------------------------
QMap<QString, DeviceFeature> &FeatureContainer::features()
{
    return this->_featureMap;
}

//------------------------------------------------------------------------------
// Возвращает ссылку на существующее или созданное свойство
//------------------------------------------------------------------------------
DeviceFeature &FeatureContainer::feature(const QString &type)
{
    if(this->_featureMap.contains(type))
        return this->_featureMap[type];
    else
        return createFeature(type);
}

//------------------------------------------------------------------------------
// Создает новое свойство или вносит изменения в существующее
//------------------------------------------------------------------------------
DeviceFeature &FeatureContainer::addFeature(const QString &type)
{
    return feature(type);
}

//------------------------------------------------------------------------------
// Создает новое свойство или вносит изменения в существующее путем копирования
//------------------------------------------------------------------------------
DeviceFeature &FeatureContainer::addFeature(const DeviceFeature &feature)
{
    DeviceFeature& feat = this->feature(feature.type());
    feat.setCount(feature.count());
    feat.setM(feature.m());
    feat.setN(feature.n());
    feat.setName(feature.name());
    feat.setPname(feature.pname());

    return feat;
}

//------------------------------------------------------------------------------
// Возвращает все характеристики в форматированном виде
//------------------------------------------------------------------------------
QStringList FeatureContainer::featuresSummary()
{
    QStringList summary;

    for(auto it = _featureMap.begin(); it != _featureMap.end(); ++it)
    {
        summary.append(it.value().getFormattedDescription());
    }

    return summary;
}

//------------------------------------------------------------------------------
// Создает новое свойство и добавляет его в карту
//------------------------------------------------------------------------------
DeviceFeature &FeatureContainer::createFeature(const QString &type)
{
    _featureMap.insert(type, DeviceFeature());
    return _featureMap[type].setType(type);
}
