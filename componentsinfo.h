#ifndef COMPONENTSINFO_H
#define COMPONENTSINFO_H

#include <QObject>
#include <QList>
#include <QMap>
#include <QVector>

#include "component.h"
#include "database.h"
#include "category.h"

class ComponentsInfo : public QObject
{
    Q_OBJECT

protected:

    QMap<int, Component> componentsMap;
    QMap<int, Category> categoriesMap;
    QMap<int, Category> subcategoriesMap;

    ComponentsInfo() = default;

    // Запрещаем копирование
    ComponentsInfo(const ComponentsInfo&) = delete;
    ComponentsInfo& operator=(const ComponentsInfo&) = delete;

public:

    virtual ~ComponentsInfo() = default;

    QMap<int, Component> * components();
    QMap<int, Category> * categories();
    QMap<int, Category> * subcategories();

    QMap<int, Component> requestComponentMap();
    QMap<int, Category> requestCategoryMap();
    QMap<int, Category> requestSubcategoryMap();

    bool fixComponentManufacturerTable(QString * errorString = nullptr);
    bool fixComponentManufacturerTable(QString& errorString);

private:

    bool deleteManufacturerList(int componentId, QString& errorString);
    bool addManufacturer(int componentId, int vendorId, QString& errorString);
    bool updateManufacturerList(const Component& component, QString& errorString);

signals:

protected slots:

    void loadDataFromDb();
};

#endif // COMPONENTSINFO_H
