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
    void setCondition(const PdscCondition &condition);
    QString language() const;
    void setLanguage(const QString &language);
    QString scope() const;
    void setScope(const QString &scope);
    QString attr() const;
    void setAttr(const QString &attr);
    QString select() const;
    void setSelect(const QString &select);
    QString path() const;
    void setPath(const QString &path);
    QString copy() const;
    void setCopy(const QString &copy);
    QString version() const;
    void setVersion(const QString &version);
    QString src() const;
    void setSrc(const QString &src);
    bool isPublic() const;
    void setPublic(bool _public);
    QString projectpath() const;
    void setProjectpath(const QString &projectpath);
    FileCategory category() const;
    void setCategory(const FileCategory &category);
    QString name() const;
    void setName(const QString &name);
};

#endif // PDSCFILE_H
