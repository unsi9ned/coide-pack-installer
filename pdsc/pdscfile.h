#ifndef PDSCFILE_H
#define PDSCFILE_H

#include <QString>
#include <QMap>
#include "pdscelement.h"
#include "pdscfilecategory.h"
#include "pdsccondition.h"

class PdscFile : public PdscElement
{
private:
    //
    // Не обязательные параметры
    //
    PdscCondition _condition;
    QString _language;
    QString _scope;
    QString _attr;
    QString _select;
    QString _path;
    QString _copy;
    QString _version;
    QString _src;
    bool    _public;
    QString _projectpath;

    //
    // Обязательные параметры
    //
    FileCategory _category;
    QString      _name;

public:
    PdscFile();
    PdscCondition condition() const;
    QString language() const;
    QString scope() const;
    QString attr() const;
    QString select() const;
    QString path() const;
    QString copy() const;
    QString version() const;
    QString src() const;
    bool isPublic() const;
    QString projectpath() const;
    FileCategory category() const;
    QString name() const;

    void setCondition(const PdscCondition &condition);
    void setLanguage(const QString &language);
    void setScope(const QString &scope);
    void setAttr(const QString &attr);
    void setSelect(const QString &select);
    void setPath(const QString &path);
    void setCopy(const QString &copy);
    void setVersion(const QString &version);
    void setSrc(const QString &src);
    void setPublic(bool _public);
    void setProjectpath(const QString &projectpath);
    void setCategory(const FileCategory &category);
    void setName(const QString &name);
};

#endif // PDSCFILE_H
