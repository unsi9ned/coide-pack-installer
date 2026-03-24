#include "category.h"

QMap<Category::PeripheralCaletory, QString> Category::periphCategoryMap =
{
    {Category::CATEGORY_COMMON, "Common"},
    {Category::CATEGORY_BOOT, "Boot"},
    {Category::CATEGORY_RTOS, "rtos"},
    {Category::CATEGORY_PERIPHERAL, "peripheral"},
};

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

Category::Category(Category::PeripheralCaletory type)
{
    switch(type)
    {
        case Category::CATEGORY_COMMON:
            *this = categoryCommon();
        break;

        case Category::CATEGORY_BOOT:
            *this = categoryBoot();
        break;

        case Category::CATEGORY_RTOS:
            *this = categoryRtos();
        break;

        case Category::CATEGORY_PERIPHERAL:
            *this = categoryPeripheral();
        break;

        default:
            *this = categoryCommon();
        break;
    }
}

Category::Category(int id, QString name, int parentId)
{
    this->id = id;
    this->name = name;
    this->parentId = parentId;
    this->subCategoryId = -1;
    this->subCategoryName = "";
}

Category Category::categoryCommon()
{
    return Category(Category::CATEGORY_COMMON,
                    Category::periphCategoryMap.value(Category::CATEGORY_COMMON));
}

Category Category::categoryBoot()
{
    return Category(Category::CATEGORY_BOOT,
                    Category::periphCategoryMap.value(Category::CATEGORY_BOOT));
}

Category Category::categoryRtos()
{
    return Category(Category::CATEGORY_RTOS,
                    Category::periphCategoryMap.value(Category::CATEGORY_RTOS));
}

Category Category::categoryPeripheral()
{
    return Category(Category::CATEGORY_PERIPHERAL,
                    Category::periphCategoryMap.value(Category::CATEGORY_PERIPHERAL));
}

Category::PeripheralCaletory Category::categoryType(const QString& name)
{
    if(name.toLower() == "common")
        return Category::CATEGORY_COMMON;
    else if(name.toLower() == "boot")
        return Category::CATEGORY_BOOT;
    else if(name.toLower() == "rtos")
        return Category::CATEGORY_RTOS;
    else if(name.toLower() == "peripheral")
        return Category::CATEGORY_PERIPHERAL;
    else
        return Category::CATEGORY_COMMON;
}


