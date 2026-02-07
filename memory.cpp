#include "memory.h"

//------------------------------------------------------------------------------
// Валидные ID в спецификации Keil
//------------------------------------------------------------------------------
QMap<KeilMemory::KeilMemoryId, QString> KeilMemory::keilMemoryNames = {
    {IRAM1, "IRAM1"}, {IRAM2, "IRAM2"}, {IRAM3, "IRAM3"}, {IRAM4, "IRAM4"},
    {IRAM5, "IRAM5"}, {IRAM6, "IRAM6"}, {IRAM7, "IRAM7"}, {IRAM8, "IRAM8"},
    {IROM1, "IROM1"}, {IROM2, "IROM2"}, {IROM3, "IROM3"}, {IROM4, "IROM4"},
    {IROM5, "IROM5"}, {IROM6, "IROM6"}, {IROM7, "IROM7"}, {IROM8, "IROM8"}
};

KeilMemory::KeilMemory()
{
    _name = idToName(KeilMemory::IROM1);
}

KeilMemory::KeilMemory(KeilMemory::KeilMemoryId id)
{
    _name = idToName(id);
}

KeilMemory::KeilMemory(const QString &name)
{
    _name = name;
}

KeilMemory::KeilMemory(const QString &id, const QString &name)
{
    if(name.isEmpty())
        setId(id);
    else
        setName(name);
}

QString KeilMemory::name() const
{
    return _name;
}

KeilMemory &KeilMemory::setId(KeilMemory::KeilMemoryId id)
{
    return setName(idToName(id));
}

KeilMemory &KeilMemory::setId(const QString &idStr)
{
    return setId(idToInt(idStr));
}

KeilMemory &KeilMemory::setName(const QString &name)
{
    if(name.isEmpty())
        setId(KeilMemory::IROM1);
    else
        _name = name;

    return *this;
}

QString KeilMemory::idToName(KeilMemory::KeilMemoryId id)
{
    return KeilMemory::keilMemoryNames.value(id);
}

KeilMemory::KeilMemoryId KeilMemory::idToInt(const QString &idStr)
{
    KeilMemory::KeilMemoryId id = KeilMemory::IROM1;

    for(auto it = KeilMemory::keilMemoryNames.begin();
        it != KeilMemory::keilMemoryNames.end();
        ++it)
    {
        if(it.value() == idStr)
        {
            id = it.key();
            break;
        }
    }

    return id;
}

Memory::Memory()
{

}

Memory &Memory::setName(const QString &name)
{
    _memoryId.setName(name);
    return *this;
}

void Memory::setId(const QString &id)
{
    _memoryId.setId(id);
}

QString Memory::name() const
{
    return _memoryId.name();
}

uint32_t Memory::startAddr() const
{
    return _startAddr;
}

void Memory::setStartAddr(const uint32_t &startAddr)
{
    _startAddr = startAddr;
}

uint32_t Memory::size() const
{
    return _size;
}

void Memory::setSize(const uint32_t &size)
{
    _size = size;
}

QString Memory::access() const
{
    return _access;
}

void Memory::setAccess(const QString &access)
{
    _access = access;
}

bool Memory::isStartup() const
{
    return _startup;
}

void Memory::setStartup(bool startup)
{
    _startup = startup;
}

bool Memory::isInit() const
{
    return _init;
}

void Memory::setInit(bool init)
{
    _init = init;
}

bool Memory::isDefault() const
{
    return _default;
}

void Memory::setDefault(bool state)
{
    _default = state;
}

//------------------------------------------------------------------------------
// Определяет является ли данный регион памятью программ
//------------------------------------------------------------------------------
bool Memory::isCodeMemory()
{
    if(!isDefault() || !isStartup())
        return false;

#if 0
    for(KeilMemory::KeilMemoryId id = KeilMemory::IROM1; id <= KeilMemory::IROM8; id++)
    {
        if(name() == KeilMemory::idToName(id))
            return true;
    }
#endif

    if(name().toLower().contains("irom") ||
       name().toLower().contains("flash") ||
       name().toLower().contains("rom"))
    {
        return true;
    }

    return false;
}

//------------------------------------------------------------------------------
// Определяет является ли данный регион памятью данных
//------------------------------------------------------------------------------
bool Memory::isDataMemory()
{
    if(!isDefault())
        return false;

    if(name().toLower().contains("iram") ||
       name().toLower().contains("ram"))
    {
        return true;
    }

    return false;
}





