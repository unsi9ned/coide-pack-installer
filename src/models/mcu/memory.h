#ifndef MEMORY_H
#define MEMORY_H

#include <QString>
#include <QMap>

//------------------------------------------------------------------------------
// Класс связывает между собой разные интерпретации наименования региона
// памяти в спецификации Keil и приводит их к одному общему виду
//------------------------------------------------------------------------------
class KeilMemory
{
public:
    enum KeilMemoryId
    {
        IRAM1,
        IRAM2,
        IRAM3,
        IRAM4,
        IRAM5,
        IRAM6,
        IRAM7,
        IRAM8,
        IROM1,
        IROM2,
        IROM3,
        IROM4,
        IROM5,
        IROM6,
        IROM7,
        IROM8,
    };

private:
    static QMap<KeilMemoryId, QString> keilMemoryNames;
    QString _name;

public:
    KeilMemory();
    KeilMemory(KeilMemoryId id);
    KeilMemory(const QString& name);
    KeilMemory(const QString& id, const QString& name);

    QString name() const;
    KeilMemory& setId(KeilMemoryId id);
    KeilMemory& setId(const QString& idStr);
    KeilMemory& setName(const QString& name);
    static QString idToName(KeilMemoryId id);

private:
    KeilMemoryId idToInt(const QString& idStr);
};



//------------------------------------------------------------------------------
// Класс-контейнер, который хранит в себе всю информацию об отдельном
// регионе памяти микроконтроллера
//------------------------------------------------------------------------------
class Memory
{
private:

    /* id specifies the enumerated ID of memory (deprecated in 1.4.4) */
    /* name specifies an identifier being use to refer to this memory range description (added in 1.4.4) */
    KeilMemory   _memoryId;

    uint32_t     _startAddr;
    uint32_t     _size;

    /* access: specifies the access permissions of the memory (added in 1.4.4). Example: access="rwx" */
    QString      _access;

    /* if true, this memory is used for the startup code */
    bool         _startup;

    /* if true, this memory is initialized with 0 */
    bool         _init;

    /* specifies whether the memory is used as default by linker */
    bool         _default;

public:
    Memory();

    QString name() const;
    uint32_t startAddr() const;
    QString startAddrHex() const;
    uint32_t size() const;
    QString sizeHex() const;
    QString access() const;
    bool isStartup() const;
    bool isInit() const;
    bool isDefault() const;

    Memory& setName(const QString& name);
    void setId(const QString& id);
    void setStartAddr(const uint32_t &startAddr);
    void setSize(const uint32_t &size);
    void setAccess(const QString &access);
    void setStartup(bool isStartup);
    void setInit(bool isInit);
    void setDefault(bool state);

    bool isCodeMemory();
    bool isDataMemory();

private:

};

#endif // MEMORY_H
