#include "progalgorithm.h"

uint32_t ProgAlgorithm::size() const
{
    return _size;
}

void ProgAlgorithm::setSize(const uint32_t &size)
{
    _size = size;
}

uint32_t ProgAlgorithm::RAMstart() const
{
    return _RAMstart;
}

void ProgAlgorithm::setRAMstart(const uint32_t &RAMstart)
{
    _RAMstart = RAMstart;
}

uint32_t ProgAlgorithm::RAMsize() const
{
    return _RAMsize;
}

void ProgAlgorithm::setRAMsize(const uint32_t &RAMsize)
{
    _RAMsize = RAMsize;
}

bool ProgAlgorithm::isDefault() const
{
    return _isDefault;
}

void ProgAlgorithm::setDefault(bool isDefault)
{
    _isDefault = isDefault;
}

uint32_t ProgAlgorithm::start() const
{
    return _start;
}

void ProgAlgorithm::setStart(const uint32_t &start)
{
    _start = start;
}

ProgAlgorithm::ProgAlgorithm(const QString &name)
{
    this->_coId = -1;
    this->_name = name;
}

void ProgAlgorithm::setCoId(int id)
{
    this->_coId = id;
}

void ProgAlgorithm::setName(QString name)
{
    this->_name = name;
}

int ProgAlgorithm::coId() const
{
    return this->_coId;
}

QString ProgAlgorithm::name() const
{
    return this->_name;
}

bool ProgAlgorithm::isNull()
{
    return this->_coId <= 0 || this->_name.isEmpty();
}
