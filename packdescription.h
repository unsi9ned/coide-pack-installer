#ifndef PACKDESCRIPTION_H
#define PACKDESCRIPTION_H

#include <QString>
#include <QMap>
#include "manufacturer.h"
#include "pdsccomponent.h"

class PackDescription
{
private:
    QString _vendor;
    QString _name;
    QString _url;
    QString _description;
    QString _release;
    QString _pathToArchive;
    QString _installDir;
    QString _pathToPdsc;

    QMap<QString, Manufacturer> _vendorMap;
    QMap<QString, Component> _componentMap;

    // Карта компонентов CoIDE. Ключ - UUID компонента, значение - список файлов
    QMap<QString, QStringList> _coComponentMap;

    // Карта компонентов CMSIS Core для быстрого доступа по версии
    QMap<QString, Component*>  _cmsisComponentMap;

public:
    PackDescription();

    QString packVendor() const;
    QString name() const;
    QString url();
    QString description();
    QString release() const;
    QString pathToArchive() const;
    QString installDir() const;
    QString pathToPdsc() const;
    QString pathToSVD(const Mcu& device);

    void setVendor(QString vendor);
    void setName(QString name);
    void setUrl(QString url);
    void setDescription(QString description);
    void setRelease(QString release);
    void setPathToArchive(const QString& path);
    void setInstallDir(const QString& path);
    void setPathToPdsc(const QString& path);

    Manufacturer& vendor(const QString& vendorName);
    Manufacturer& vendorByDvendor(const QString& Dvendor);
    QMap<QString, Manufacturer>& vendors();

    void printInfo();
    bool isValid() const;

    QMap<QString, Component>& components();
    QMap<QString, QStringList>& coComponentMap();
    QMap<QString, Component*>& cmsisComponents();

    void clear();

private:
    Manufacturer& createNewVendor(const QString& vendorName);
};

#endif // PACKDESCRIPTION_H
