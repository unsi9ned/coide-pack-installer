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

    DataBase * db;

    QMap<int, Component> componentsMap;
    QMap<int, Category> categoriesMap;
    QMap<int, Category> subcategoriesMap;

public:
    explicit ComponentsInfo(DataBase * db, QObject *parent = 0);
    ~ComponentsInfo();

    QMap<int, Component> * components();
    QMap<int, Category> * categories();
    QMap<int, Category> * subcategories();

private:

    QMap<int, Component> requestComponentsMap();
    QMap<int, Category> requestCategoryMap();
    QMap<int, Category> requestSubcategoryMap();

signals:

public slots:

    void loadDataFromDb();
};

#endif // COMPONENTSINFO_H
