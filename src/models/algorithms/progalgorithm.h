#ifndef FLASHALGORITHM_H
#define FLASHALGORITHM_H

#include <QObject>
#include <QString>
#include <QByteArray>

class ProgAlgorithm
{

private:

    int      _coId;

    QString  _name;
    uint32_t _start;
    uint32_t _size;
    uint32_t _RAMstart;
    uint32_t _RAMsize;
    bool     _isDefault;

public:
    ProgAlgorithm(const QString& name = QString());

    void setCoId(int id);
    void setName(QString name);
    void setStart(const uint32_t &start);
    void setSize(const uint32_t &size);
    void setRAMstart(const uint32_t &RAMstart);
    void setRAMsize(const uint32_t &RAMsize);
    void setDefault(bool isDefault);

    int coId() const;
    QString name() const;
    uint32_t start() const;
    uint32_t size() const;
    uint32_t RAMstart() const;
    uint32_t RAMsize() const;
    bool isDefault() const;
    bool isNull();

};

#endif // FLASHALGORITHM_H
