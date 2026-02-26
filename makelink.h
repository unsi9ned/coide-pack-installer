#ifndef MAKELINK_H
#define MAKELINK_H

#include <QProcess>
#include <QDebug>
#include <QDir>

class MakeLink
{
public:
    enum LinkType
    {
        SymbolicFile,      // Без флага (по умолчанию) - символическая ссылка на файл
        SymbolicDirectory, // /D - символическая ссылка на папку
        HardLink,          // /H - жесткая ссылка
        Junction           // /J - junction point (только для папок)
    };

public:
    MakeLink();

    static bool createLink(const QString& linkPath,
                           const QString& targetPath,
                           LinkType type = SymbolicFile);

    static bool removeLink(const QString& linkPath);
    static bool isLink(const QString& path);
    static QString getLinkTarget(const QString& linkPath);
};

#endif // MAKELINK_H
