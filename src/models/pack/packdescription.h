#ifndef PACKDESCRIPTION_H
#define PACKDESCRIPTION_H

#include <QString>
#include <QMap>
#include "models/mcu/manufacturer.h"
#include "models/pdsc/pdsccomponent.h"

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
    QMap<QString, QStringList> _coComponentFilesMap;

    // Карта компонентов CMSIS Core для быстрого доступа по версии
    QMap<QString, Component*>  _cmsisComponentMap;

    // Сохранение промежуточных данных о компонентах
    QList<PdscComponent> _pdscComponentList;

public:
    PackDescription();

    QString packVendor() const;
    QString name() const;
    QString url();
    QString description() const;
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

    const Manufacturer& constVendor(const QString& vendorName) const;

    void printInfo();
    bool isValid() const;
    bool hasDevices() const;
    bool hasComponents() const;

    QMap<QString, Component>& coComponentMap();
    QMap<QString, QStringList>& componentFilesMap();
    QMap<QString, Component*>& cmsisComponents();
    QList<PdscComponent>& pdscComponentList();

    void clear();

private:
    void appendVendors(const QMap<QString, Manufacturer>& vendorMap);
    Manufacturer& createNewVendor(const QString& vendorName);
};

#endif // PACKDESCRIPTION_H
