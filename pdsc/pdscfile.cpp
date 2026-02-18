#include "pdscfile.h"

PdscCondition PdscFile::condition() const
{
    return _condition;
}

void PdscFile::setCondition(const PdscCondition &condition)
{
    _condition = condition;
}

QString PdscFile::language() const
{
    return _language;
}

void PdscFile::setLanguage(const QString &language)
{
    _language = language;
}

QString PdscFile::scope() const
{
    return _scope;
}

void PdscFile::setScope(const QString &scope)
{
    _scope = scope;
}

QString PdscFile::attr() const
{
    return _attr;
}

void PdscFile::setAttr(const QString &attr)
{
    _attr = attr;
}

QString PdscFile::select() const
{
    return _select;
}

void PdscFile::setSelect(const QString &select)
{
    _select = select;
}

QString PdscFile::path() const
{
    return _path;
}

void PdscFile::setPath(const QString &path)
{
    _path = path;
}

QString PdscFile::copy() const
{
    return _copy;
}

void PdscFile::setCopy(const QString &copy)
{
    _copy = copy;
}

QString PdscFile::version() const
{
    return _version;
}

void PdscFile::setVersion(const QString &version)
{
    _version = version;
}

QString PdscFile::src() const
{
    return _src;
}

void PdscFile::setSrc(const QString &src)
{
    _src = src;
}

bool PdscFile::isPublic() const
{
    return _public;
}

void PdscFile::setPublic(bool p)
{
    _public = p;
}

QString PdscFile::projectpath() const
{
    return _projectpath;
}

void PdscFile::setProjectpath(const QString &projectpath)
{
    _projectpath = projectpath;
}

FileCategory PdscFile::category() const
{
    return _category;
}

void PdscFile::setCategory(const FileCategory &category)
{
    _category = category;
}

QString PdscFile::name() const
{
    return _name;
}

bool PdscFile::hasCondition()
{
    return !_condition.isNull();
}

void PdscFile::setName(const QString &name)
{
    _name = name;
}

PdscFile::PdscFile()
{

}
