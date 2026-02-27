#ifndef CATEGORY_H
#define CATEGORY_H

#include <QString>
#include <QList>
#include <QMap>

class Category
{

private:

    int id;
    int parentId;
    QString name;
    QMap<int, Category*> children; //deprecated

    int subCategoryId;
    QString subCategoryName;

public:

    Category();
    Category(int id, QString name, int parentId = -1);

    int getId() const;
    QString getName() const;
    int getParentId() const;
    QMap<int, Category *> getChildren() const;

    void setId(int value);
    void setName(const QString &value);
    void setParentId(int value);
    void setChildren(const QMap<int, Category *> &value);
    void appendChild(Category * child);

    void setSubCategoryName(const QString& name);
    void setSubCategoryId(int id);
    QString getSubCategoryName() const;
    int getSubCategoryId() const;

    bool isNull();
    bool isValid();
    bool hasSubCategory();
};

#endif // CATEGORY_H
