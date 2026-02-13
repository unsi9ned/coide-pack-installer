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
    QMap<int, Category*> children;

public:

    Category();
    Category(const Category &c);
    Category(Category * c);
    Category(int id, QString name, int parentId = -1);
    void operator =(const Category &c);

    int getId() const;
    void setId(int value);
    QString getName() const;
    void setName(const QString &value);
    int getParentId() const;
    void setParentId(int value);
    QMap<int, Category *> getChildren() const;
    void setChildren(const QMap<int, Category *> &value);
    void appendChild(Category * child);
};

#endif // CATEGORY_H
