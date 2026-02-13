#include "category.h"

int Category::getId() const
{
    return id;
}

void Category::setId(int value)
{
    id = value;
}

QString Category::getName() const
{
    return name;
}

void Category::setName(const QString &value)
{
    name = value;
}

int Category::getParentId() const
{
    return parentId;
}

void Category::setParentId(int value)
{
    parentId = value;
}

QMap<int, Category *> Category::getChildren() const
{
    return children;
}

void Category::setChildren(const QMap<int, Category *> &value)
{
    children = value;
}

void Category::appendChild(Category *child)
{
    this->children.insert(child->getId(), child);
}

Category::Category()
{
    this->id = -1;
    this->parentId = -1;
    this->name = "";
}

Category::Category(const Category &c)
{
    this->id = c.id;
    this->name = c.name;
    this->parentId = c.parentId;
    this->children = c.getChildren();
}

Category::Category(Category *c)
{
    this->id = c->id;
    this->name = c->name;
    this->parentId = c->parentId;
    this->children = c->getChildren();
}

Category::Category(int id, QString name, int parentId)
{
    this->id = id;
    this->name = name;
    this->parentId = parentId;
}

void Category::operator =(const Category &c)
{
    this->id = c.id;
    this->name = c.name;
    this->parentId = c.parentId;
    this->children = c.getChildren();
}
