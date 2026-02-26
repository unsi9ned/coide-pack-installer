#ifndef COMPONENTSINFO_H
#define COMPONENTSINFO_H

#include <QObject>
#include <QList>
#include <QMap>
#include <QVector>

#include "component.h"
#include "database.h"
#include "category.h"
#include "example.h"

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

    QMap<int, Example> requestExampleMap();

    bool fixComponentManufacturerTable(QString * errorString = nullptr);
    bool fixComponentManufacturerTable(QString& errorString);
    bool removeComponentsRelation(int parentId, int childId, QString * errorString = nullptr);
    bool removeComponentsRelation(QMap<int, int> pairs, QString * errorString = nullptr);
    bool removeExampleRelation(int exampleId, int componentId, QString * errorString = nullptr);
    bool removeExampleRelation(QMap<int, int> pairs, QString * errorString = nullptr);
    bool removeComponent(int componentId, QString * errorString = nullptr);
    bool removeComponent(const Component& component, QString * errorString = nullptr);
    bool removeExample(int exampleId, QString * errorString = nullptr);
    bool removeExample(const Example& example, QString * errorString = nullptr);
    bool removeComponents(QVector<int> componentIds, QString * errorString = nullptr);
    bool removeComponentStatus(int statusId, QString * errorString = nullptr);
    bool removeComponentStatuses(QVector<int> statusIdList, QString * errorString = nullptr);
    bool removeComponentPhantomRelations(QString *errorString = nullptr);
    bool removeExamplePhantomRelations(QString *errorString = nullptr);
    bool updateComponentStatus(int statusId,
                               const Component::ComponentStatus& upd,
                               QString * errorString = nullptr);
    bool setComponentStatusOK(int statusId, QString * errorString = nullptr);
    bool removeStatusPhantomRelations(QString *errorString = nullptr);

    bool createComponent(Component& component, QString *errorString = nullptr);

private:

    bool deleteManufacturerList(int componentId, QString& errorString);
    bool addManufacturer(int componentId, int vendorId, QString& errorString);
    bool updateManufacturerList(const Component& component, QString& errorString);
    Component findComponent(const Component component);

signals:

protected slots:

    void loadDataFromDb();
};

#endif // COMPONENTSINFO_H
