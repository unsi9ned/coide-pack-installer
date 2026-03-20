#ifndef MANUFACTURER_H
#define MANUFACTURER_H

#include <QObject>
#include <QString>
#include <QList>

#include "family.h"
#include "devicehierarchynode.h"

class Manufacturer : public DeviceHierarchyNode
{
public:

    struct SvdInfo
    {
        QString pathInArchive;
        QString destDirectory;
        QString relativePath;
        QStringList mcuList;

        SvdInfo(const QString& path, const QString& dir = QString())
        {
            pathInArchive = path;
            destDirectory = dir;
        }

        bool operator==(const SvdInfo &other) const
        {
            return pathInArchive == other.pathInArchive;
        }

        void addMcuName(const QString& mcuName)
        {
            mcuList.append(mcuName);
        }
    };

private:

    int id;
    QString name;
    QMap<QString, Family> familyMap;
    QList<SvdInfo> _svdList;

    static const QMap<QString, int> _coVendorMap;
    static const QMap<QString, int> _keilVendorMap;

public:

    Manufacturer();
    Manufacturer(int id, QString name = QString());
    Manufacturer(Manufacturer* m);

    QMap<QString, Family>& families();

    bool hasFamilies();

    void setId(int id);
    int getId() const;
    qint32 getUniqueId() const;

    Manufacturer& setName(QString name);
    QString getName() const;
    QString toKeilName() const;
    int toKeilId() const;

    static QString co2keilName(const QString& coName);
    static int co2keilId(int coId);

    //Узнать количество семейств в списке
    int getFamiliesCount();

    //Вернуть идентификаторы
    QStringList getFamiliesKeys();

    //Найти семейство по идентификатору
    Family getFamilyById(int i);

    Family& family(const QString& name);
    const Family& constFamily(const QString& name) const;

    //Добавить семейство
    Family& addFamily(const QString& name);

    //Возврат списка svd-файлов
    QList<SvdInfo>& svdList();

    //Возврат конкретного SVD
    SvdInfo * svd(const QString& path);

    bool isValid(QString * errorString = nullptr) const;
    bool isNull();
    QString getPath() const;

private:

    bool isValid(QString& errorString) const;
    Family& createNewFamily(const QString& name);
};

#endif // MANUFACTURER_H
