#ifndef COMPONENTSINFO_H
#define COMPONENTSINFO_H

#include <QObject>
#include <QList>
#include <QMap>

#include "component.h"
#include "database.h"
#include "category.h"

class ComponentsInfo : public QObject
{
    Q_OBJECT

private:

    QMap<int, Component> componentsMap;
    QMap<int, Category> categoriesMap;
    QMap<int, Category> subcategoriesMap;

    explicit ComponentsInfo();
    ~ComponentsInfo();

    static ComponentsInfo* _m_instance;

public:

    ComponentsInfo(const ComponentsInfo&) = delete;
    ComponentsInfo& operator=(const ComponentsInfo&) = delete;

    static ComponentsInfo* instance();

    QMap<int, Component> * components();
    QMap<int, Category> * categories();
    QMap<int, Category> * subcategories();

    QMap<int, Component> requestComponentsMap();
    QMap<int, Category> requestCategoryMap();
    QMap<int, Category> requestSubcategoryMap();

    bool fixManufacturerIDs(QMap<int, Component>& components, QString * errorString = nullptr);
    bool fixManufacturerIDs(QMap<int, Component>& components, QString& errorString);
    bool fixManufacturerIDs(QString& errorString);
    bool fixManufacturerIDs(QString * errorString = nullptr);

private:

    bool deleteManufacturerList(int componentId, QString& errorString);
    bool addManufacturer(int componentId, int vendorId, QString& errorString);
    bool updateManufacturerList(const Component& component, QString& errorString);

signals:

public slots:

    ComponentsInfo& loadDataFromDb();
};

#endif // COMPONENTSINFO_H
