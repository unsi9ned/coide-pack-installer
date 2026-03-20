#include "manufacturer.h"

const QMap<QString, int> Manufacturer::_coVendorMap = {
    {"ARM", 1},
    {"Atmel", 2},
    {"Energy Micro", 3},
    {"Freescale", 4},
    {"Holtek", 5},
    {"TI", 6},
    {"NXP", 7},
    {"Nuvoton", 8},
    {"ST", 9},
    {"Toshiba", 10},
    {"Spansion", 12},
    {"Active-Semi", 11},
};

const QMap<QString, int> Manufacturer::_keilVendorMap = {
    // Основные вендоры
    {"ABOV Semiconductor", 126},
    {"Actel", 56},
    {"Active-Semi", 140},
    {"Altera", 85},
    {"Altium", 65},
    {"Ambiq Micro", 120},
    {"Analog Devices", 1},
    {"ARM", 82},
    {"ARM CMSIS", 109},
    {"Atmel", 3},
    {"CSR", 118},
    {"Cypress", 19},
    {"Dialog Semiconductor", 113},
    {"Dolphin", 57},
    {"Domosys", 26},
    {"Ember", 98},
    {"Energy Micro", 97},
    {"EnOcean", 91},
    {"Evatronix", 64},
    {"Freescale", 78},
    {"Generic", 5},
    {"GigaDevice", 123},
    {"Holtek", 106},
    {"Hynix Semiconductor", 6},
    {"Hyundai", 35},
    {"Infineon", 7},
    {"Kionix", 127},
    {"Lapis Semiconductor", 10},
    {"Luminary Micro", 76},
    {"Maxim", 23},
    {"MegaChips", 128},
    {"Mentor Graphics Co.", 24},
    {"Micronas", 30},
    {"Microsemi", 112},
    {"Milandr", 99},
    {"NetSilicon", 67},
    {"Nordic Semiconductor", 54},
    {"Nuvoton", 18},
    {"NXP", 11},
    {"OKI SEMICONDUCTOR CO.,LTD.", 108},
    {"Realtek Semiconductor", 124},
    {"Redpine Signals", 125},
    {"Renesas", 117},
    {"ROHM", 103},
    {"Samsung", 47},
    {"Silicon Labs", 21},
    {"SONiX", 110},
    {"Spansion", 100},
    {"STMicroelectronics", 13},
    {"Sunrise Micro Devices", 121},
    {"TI", 16},
    {"Texas Instruments", 16},
    {"Toshiba", 92},
    {"Triad Semiconductor", 104},
    {"WIZnet", 122},

    // Deprecated - оставляем для обратной совместимости
    {"Freescale Semiconductor", 78},
    {"NXP (founded by Philips)", 11},
};

Manufacturer::Manufacturer()
{
    this->id = -1;
}

Manufacturer::Manufacturer(int id, QString name)
{
    this->id = id;
    this->name = name;

    if(this->name.isEmpty())
    {
        for(auto it = _coVendorMap.begin(); it != _coVendorMap.end(); ++it)
        {
            if(it.value() == id)
            {
                this->name = it.key();
                this->name = toKeilName();
                break;
            }
        }
    }
}

Manufacturer::Manufacturer(Manufacturer *m)
{
    if(m == nullptr)
        return;

    this->id = m->getId();
    this->name = m->getName();

    for (auto it = m->familyMap.begin(); it != m->familyMap.end(); ++it)
    {
        this->familyMap.insert(it.key(), it.value());
    }
}

//------------------------------------------------------------------------------
// Вернуть все семейства
//------------------------------------------------------------------------------
QMap<QString, Family> &Manufacturer::families()
{
    return this->familyMap;
}

//------------------------------------------------------------------------------
// Проверить имеет ли производитель загруженные семейства
//------------------------------------------------------------------------------
bool Manufacturer::hasFamilies() {return !this->familyMap.isEmpty();}

//------------------------------------------------------------------------------
// Вернуть ID производителя
//------------------------------------------------------------------------------
int Manufacturer::getId() const {return id;}

//------------------------------------------------------------------------------
// Вернуть имя производителя
//------------------------------------------------------------------------------
QString Manufacturer::getName() const {return name;}

//------------------------------------------------------------------------------
// Преобразовать имя вендора CoIDE в валидное имя Keil
//------------------------------------------------------------------------------
QString Manufacturer::toKeilName() const
{
    QString keilName;

    if (this->name == "ST") {
        keilName = "STMicroelectronics";
    } else if (this->name == "TI") {
        keilName = "Texas Instruments";
    } else if (this->name == "ARM") {
        keilName = "ARM";
    } else if (this->name == "Atmel") {
        keilName = "Atmel";
    } else if (this->name == "Energy Micro") {
        keilName = "Energy Micro";
    } else if (this->name == "Freescale") {
        keilName = "Freescale";
    } else if (this->name == "Holtek") {
        keilName = "Holtek";
    } else if (this->name == "NXP") {
        keilName = "NXP";
    } else if (this->name == "Nuvoton") {
        keilName = "Nuvoton";
    } else if (this->name == "Toshiba") {
        keilName = "Toshiba";
    } else if (this->name == "Spansion") {
        keilName = "Spansion";
    } else if (this->name == "Active-Semi") {
        keilName = "Active-Semi";
    } else {
        keilName = this->name;  // оставляем как есть
    }

    return keilName;
}

//------------------------------------------------------------------------------
// Преобразовать ID вендора CoIDE в валидное ID Keil
//------------------------------------------------------------------------------
int Manufacturer::toKeilId() const
{
    int keilId = 0;
    QString keilName = this->toKeilName();

    if(_keilVendorMap.contains(keilName))
        keilId = _keilVendorMap.value(keilName) + 1000;
    else
        keilId = this->id;

    return keilId;
}

//------------------------------------------------------------------------------
// Преобразовать имя вендора CoIDE в валидное имя Keil
//------------------------------------------------------------------------------
QString Manufacturer::co2keilName(const QString &coName)
{
    QString keilName;

    if (coName == "ST")
    {
        keilName = "STMicroelectronics";
    }
    else if (coName == "TI")
    {
        keilName = "Texas Instruments";
    }
    else
    {
        keilName = coName;
    }

    return keilName;
}

//------------------------------------------------------------------------------
// Преобразовать ID вендора CoIDE в валидное ID Keil
//------------------------------------------------------------------------------
int Manufacturer::co2keilId(int coId)
{
    QString coName;

    for(auto it = Manufacturer::_coVendorMap.begin(); it != Manufacturer::_coVendorMap.end(); ++it)
    {
        if(coId == it.value())
        {
            coName = it.key();
            break;
        }
    }

    if(coName.isEmpty())
    {
        return coId;
    }
    else
    {
        QString keilName = co2keilName(coName);

        if(Manufacturer::_keilVendorMap.contains(keilName))
            return Manufacturer::_keilVendorMap.value(keilName) + 1000;
        else
            return coId;
    }

    return coId;
}

//------------------------------------------------------------------------------
// Задать ID производителя
//------------------------------------------------------------------------------
void Manufacturer::setId(int id){this->id = id;}

//------------------------------------------------------------------------------
// Задать название производителя
//------------------------------------------------------------------------------
Manufacturer &Manufacturer::setName(QString name)
{
    this->name = name;
    return *this;
}

//------------------------------------------------------------------------------
// Количество загруженных семейств
//------------------------------------------------------------------------------
int Manufacturer::getFamiliesCount(){return this->familyMap.count();}

//------------------------------------------------------------------------------
// Список загруженных семейств
//------------------------------------------------------------------------------
QStringList Manufacturer::getFamiliesKeys(){return this->familyMap.keys();}

//------------------------------------------------------------------------------
// Найти семейство по идентификатору
//------------------------------------------------------------------------------
Family Manufacturer::getFamilyById(int i)
{
    Family fam;
    QMap<QString,Family>::iterator famIterator = familyMap.begin();

    while(famIterator != familyMap.end())
    {
        Family currFamily = famIterator.value();

        if(currFamily.getId() == i)
        {
            fam = currFamily;
            break;
        }

        ++famIterator;
    }

    return fam;
}

//------------------------------------------------------------------------------
// Найти или создать новое семейство по имени
//------------------------------------------------------------------------------
Family &Manufacturer::family(const QString &name)
{
    if(this->familyMap.contains(name))
        return this->familyMap[name];
    else
        return this->createNewFamily(name);
}

//------------------------------------------------------------------------------
// Гарантированно возвращает ссылку на объект Family, даже если такого нет с списке
//------------------------------------------------------------------------------
const Family& Manufacturer::constFamily(const QString& name) const
{
    static Family nullFamily;
    auto it = familyMap.find(name);
    return it != familyMap.end() ? *it : nullFamily;
}

//------------------------------------------------------------------------------
// Найти или создать новое семейство по имени
//------------------------------------------------------------------------------
Family &Manufacturer::addFamily(const QString &name)
{
    return this->family(name).setName(name);
}

//------------------------------------------------------------------------------
// Возврат списка svd-файлов
//------------------------------------------------------------------------------
QList<Manufacturer::SvdInfo> &Manufacturer::svdList()
{
    return this->_svdList;
}

//------------------------------------------------------------------------------
// Возврат конкретного SVD
//------------------------------------------------------------------------------
Manufacturer::SvdInfo *Manufacturer::svd(const QString &path)
{
    for(int i = 0; i < _svdList.count(); i++)
    {
        Manufacturer::SvdInfo s = _svdList.at(i);

        if(s.pathInArchive == path)
            return &_svdList[i];
    }

    return nullptr;
}

//------------------------------------------------------------------------------
// Проверка валидности объекта Manufacturer
//------------------------------------------------------------------------------
bool Manufacturer::isValid(QString *errorString) const
{
    QString e;

    if(!errorString)
        return isValid(&e);
    else
        return isValid(*errorString);
}

bool Manufacturer::isValid(QString &errorString) const
{
    if(this->id <= 0)
        errorString = QString("Invalid manufacturer id = %1").arg(this->id);
    else if(this->name.isEmpty())
        errorString = QString("Manufacturer's name is not defined");
    else
        return true;

    return false;
}

bool Manufacturer::isNull()
{
    return this->id == -1 && this->name.isEmpty() && this->familyMap.isEmpty();
}

QString Manufacturer::getPath() const
{
    return toKeilName();
}

//------------------------------------------------------------------------------
// Создание нового семейства
//------------------------------------------------------------------------------
Family &Manufacturer::createNewFamily(const QString &name)
{
    this->familyMap.insert(name, Family());
    Family& family = this->familyMap[name];
    family.setName(name);
    family.setManufacturerId(toKeilId());
    family.setParent(this);
    return family;
}
