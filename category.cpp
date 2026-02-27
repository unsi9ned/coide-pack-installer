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

void Category::setSubCategoryName(const QString &name)
{
    subCategoryName = name;
}

void Category::setSubCategoryId(int id)
{
    subCategoryId = id;
}

QString Category::getSubCategoryName() const
{
    return subCategoryName;
}

int Category::getSubCategoryId() const
{
    return subCategoryId;
}

bool Category::isNull()
{
    return this->name.isEmpty();
}

bool Category::isValid()
{
    return this->id != -1 && !this->name.isEmpty();
}

bool Category::hasSubCategory()
{
    return (!subCategoryName.isEmpty());
}

Category::Category()
{
    this->id = -1;
    this->parentId = -1;
    this->name = "";
    this->subCategoryId = -1;
    this->subCategoryName = "";
}

Category::Category(int id, QString name, int parentId)
{
    this->id = id;
    this->name = name;
    this->parentId = parentId;
    this->subCategoryId = -1;
    this->subCategoryName = "";
}


