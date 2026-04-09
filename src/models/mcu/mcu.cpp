#include "mcu.h"
#include "series.h"
#include "common/constants.h"

Mcu::Mcu()
{
    this->id = -1;
    this->seriesId = -1;
    this->userId = CoUser::USER_COOCOX;
    this->timeuuid = generateTimeUUID();
    this->hits = 0;
    this->m_parent = nullptr;
}

//------------------------------------------------------------------------------
// Формирует описание блоков памяти для базы данных CoIDE
//------------------------------------------------------------------------------
QString Mcu::coMemInfo()
{
    QString memInfo;
    const Memory * codeMem = getCodeMemory();
    const Memory * dataMem = getDataMemory();

    if(codeMem && dataMem)
    {


        memInfo = QString(
                         "[\"{\\\"type\\\":\\\"FLASH\\\","
                         "\\\"id\\\":0,\\\"start\\\":\\\"%1\\\","
                         "\\\"size\\\":\\\"%2\\\"}\","
                         "\"{\\\"type\\\":\\\"RAM\\\","
                         "\\\"id\\\":0,"
                         "\\\"start\\\":\\\"%3\\\","
                         "\\\"size\\\":\\\"%4\\\"}\"]"
                        ).
                arg(codeMem->startAddrHex()).
                arg(codeMem->sizeHex()).
                arg(dataMem->startAddrHex()).
                arg(dataMem->sizeHex());
    }

    return memInfo;
}

int Mcu::getDebugAlgorithmId() const
{
    return this->debugAlgorithm.coId();
}

void Mcu::setDebugAlgorithmId(int id)
{
    this->debugAlgorithm.setCoId(id);
}

Mcu &Mcu::setName(QString s)
{
    this->name = s;
    return *this;
}

Mcu &Mcu::setCoreDebugAlgorithm(const QString &coreName)
{
    debugAlgorithm.setProcessor(coreName);
    return *this;
}

void Mcu::setParent(Series* parent)
{
   m_parent = parent;
}

Series*Mcu::getParent()
{
    return m_parent;
}

bool Mcu::hasParent() const
{
    return m_parent != nullptr;
}

const Series&Mcu::constParent() const
{
    static Series nullSeries;
    return m_parent ? *m_parent : nullSeries;
}

const Series&Mcu::constSeries() const
{
    return constParent();
}

QString Mcu::getPath() const
{
    return constSeries().getPath() + "/" + getCoName();
}

QMap<QString, Memory> &Mcu::memoryRegions()
{
    return this->_memoryMap;
}

Memory &Mcu::memory(const QString &name)
{
    if(_memoryMap.contains(name))
        return _memoryMap[name];
    else
        return createMemoryRegion(name);
}

Memory &Mcu::addMemoryRegion(const QString name)
{
    return memory(name);
}

const Memory *Mcu::getCodeMemory() const
{
    for(auto it = _memoryMap.begin(); it != _memoryMap.end(); ++it)
    {
        if(it.value().isCodeMemory())
            return &it.value();
    }

    return nullptr;
}

const Memory* Mcu::getDataMemory() const
{
    for(auto it = _memoryMap.begin(); it != _memoryMap.end(); ++it)
    {
        if(it.value().isDataMemory())
            return &it.value();
    }

    return nullptr;
}

//------------------------------------------------------------------------------
// Поиск дефолтного алгоритма программирования
//------------------------------------------------------------------------------
ProgAlgorithm*Mcu::getDefaultFlashAlgorithm()
{
    ProgAlgorithm * flashAlgorithm = nullptr;

    auto codeMem = getCodeMemory();
    int32_t flashStartAddr = codeMem ? codeMem->startAddr() : -1;
    flashAlgorithm = getFlashAlgorithm(flashStartAddr);

    return flashAlgorithm;
}

QString Mcu::generateTimeUUID()
{
    QString str = QUuid::createUuid().toString();

    // Удаляем фигурные скобки в начале и конце
    if (str.startsWith('{') && str.endsWith('}'))
    {
        str = str.mid(1, str.length() - 2);
    }

    return str;
}

QString Mcu::defSym2coMicro()
{
    QString micro;

    foreach(QString d, _definedSymbols)
    {
        micro += d;
        micro += ",";
    }

    if(micro.endsWith(','))
        micro.chop(1);

    return micro;
}

QString Mcu::coDescription()
{
    const Memory * codeMem = getCodeMemory();
    const Memory * dataMem = getDataMemory();

    if(description.isEmpty())
        return QString("%1 has %2 Kbytes of FLASH and %3 Kbytes of RAM").
                       arg(getName()).
                       arg(codeMem ? codeMem->size()/1024 : 0).
                       arg(dataMem ? dataMem->size()/1024 : 0);
    else
        return description;
}

bool Mcu::isValid(QString *errorString) const
{
    QString e;

    if(errorString)
        return isValid(*errorString);
    else
        return isValid(e);
}

bool Mcu::isNull()
{
    return id == -1 ||
           seriesId == -1 ||
           debugAlgorithm.isNull() ||
           name.isEmpty();
}

bool Mcu::isValid(QString &errorString) const
{
    if(seriesId <= 0)
        errorString = QString("Invalid series ID = %1").arg(seriesId);
    else if(name.isEmpty())
        errorString = QString("Mcu name is not defined");
    else if(_memoryMap.isEmpty())
        errorString = "Memory regions are not defined for the device";
    else if(getDataMemory() == nullptr)
        errorString = "The RAM region is not specified";
    else if(getCodeMemory() == nullptr)
        errorString = "The FLASH region is not specified";
    else if(debugAlgorithm.isNull())
        errorString = "The debugging algorithm is not defined";
    else return true;

    return false;
}

Memory &Mcu::createMemoryRegion(const QString regionName)
{
    _memoryMap.insert(regionName, Memory());
    return _memoryMap[regionName].setName(regionName);
}
