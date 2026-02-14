#include "featurecontainer.h"

FeatureContainer::FeatureContainer()
{

}

//------------------------------------------------------------------------------
// Возвращает карту свойств
//------------------------------------------------------------------------------
QList<DeviceFeature> &FeatureContainer::features()
{
    return this->_featureList;
}

//------------------------------------------------------------------------------
// Создает новое свойство или вносит изменения в существующее
//------------------------------------------------------------------------------
void FeatureContainer::addFeature(const QString &type)
{
    DeviceFeature feat(type);
    this->_featureList.append(feat);
}

//------------------------------------------------------------------------------
// Создает новое свойство или вносит изменения в существующее путем копирования
//------------------------------------------------------------------------------
void FeatureContainer::addFeature(const DeviceFeature &feature)
{
    DeviceFeature feat(feature.type());
    feat.setCount(feature.count());
    feat.setM(feature.m());
    feat.setN(feature.n());
    feat.setName(feature.name());
    feat.setPname(feature.pname());
    this->_featureList.append(feat);
}

//------------------------------------------------------------------------------
// Возвращает все характеристики в форматированном виде
//------------------------------------------------------------------------------
QStringList FeatureContainer::featuresSummary()
{
    QStringList summary;

    foreach(auto feat, _featureList)
    {
        summary.append(feat.getFormattedDescription());
    }

    return summary;
}

//------------------------------------------------------------------------------
// Возвращает все характеристики в форматированном виде для базы данных CoIDE
//------------------------------------------------------------------------------
QString FeatureContainer::coFeaturesSummary() const
{
    QString summary = "[";

    foreach(auto feat, _featureList)
    {
        summary += QString("\"%1\",").arg(feat.getFormattedDescription());
    }

    if (summary.endsWith(','))
    {
        summary.chop(1);
    }

    summary += "]";
    return summary;
}


