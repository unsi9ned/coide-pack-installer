#ifndef COMPONENTSINFO_H
#define COMPONENTSINFO_H

#include <QObject>
#include <QList>
#include <QMap>
#include <QVector>
#include <QPair>

#include "models/components/component.h"
#include "models/components/category.h"
#include "models/components/example.h"
#include "database.h"
#include "common/loggable.h"

class ComponentsInfo : public QObject, public Loggable
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

    QString m_lastError;

protected:
    QString logSource() const override { return "ComponentsInfo"; }
    QString lastError() const { return m_lastError; }

    void logError(const QString& e)
    {
        m_lastError = e;
        Loggable::logError(m_lastError);
    }

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
    bool updateComponent(Component& component, QString *errorString = nullptr);
    bool createComponentPdscAttrTable(QString * errorString = nullptr);

private:

    bool deleteManufacturerList(int componentId, QString& errorString);
    bool addManufacturer(int componentId, int vendorId, QString& errorString);
    bool updateManufacturerList(const Component& component, QString& errorString);
    Component findComponent(const Component component, QString* errorString = nullptr);
    Category findCategory(const Category category, QString* errorString = nullptr);
    Category findSubCategory(const Category category, QString* errorString = nullptr);

    bool createComponentStatus(Component::ComponentStatus& status, QString *errorString = nullptr);
    bool createCategory(Category& category, QString *errorString = nullptr);
    bool createSubCategory(Category& category, QString *errorString = nullptr);

    bool hasComponentMcuLink(int componentId, const QString& mcuName, int * mcuId = nullptr, bool * status = nullptr, QString * errorString = nullptr);
    bool createComponentMcuLink(int componentId, const QString& mcuName, QString * errorString = nullptr);

    bool hasComponentCategoryLink(int componentId, const QString& categoryName, int * categoryId = nullptr, bool * status = nullptr, QString * errorString = nullptr);
    bool createComponentCategoryLink(int componentId, const QString& categoryName, QString * errorString = nullptr);

    bool hasComponentSubCategoryLink(int componentId, const QString& categoryName, const QString& subCategoryName, bool * status = nullptr, QString * errorString = nullptr);
    bool createComponentSubCategoryLink(int componentId, const QString& categoryName, const QString& subCategoryName, QString * errorString = nullptr);

    bool hasComponentsLink(int parentId, int childId, bool * status = nullptr, QString * errorString = nullptr);
    bool createComponentsLink(int parentId, int childId, QString * errorString = nullptr);

    bool addComponentPdscAttributes(const Component& component, QString * errorString = nullptr);

signals:

protected slots:

    void loadDataFromDb();
};

#endif // COMPONENTSINFO_H
