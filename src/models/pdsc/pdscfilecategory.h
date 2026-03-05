#ifndef PDSCFILECATEGORY_H
#define PDSCFILECATEGORY_H

#include <QMap>
#include <QString>

class FileCategory
{
public:
    enum FileCategoryType {
        Doc,
        Header,
        Include,
        Library,
        Object,
        Source,
        SourceC,        // deprecated
        SourceCpp,      // deprecated
        SourceAsm,      // deprecated
        LinkerScript,
        Utility,
        Image,
        Other,
        PreIncludeGlobal,
        PreIncludeLocal,
        GenSource,
        GenHeader,
        GenParams,
        GenAsset,
        Invalid
    };

private:
    FileCategoryType _category;

public:
    FileCategory()
    {
        _category = FileCategory::Invalid;
    }

    FileCategory(const QString& categoryName)
    {
        _category = fromString(categoryName);
    }

    bool isValid()
    {
        return _category != FileCategory::Invalid;
    }

    FileCategoryType type()
    {
        return _category;
    }

    QString name()
    {
        return toString(_category);
    }

    static QString toString(FileCategoryType value) {
        return toStringMap().value(value, QString());
    }

    static FileCategoryType fromString(const QString& str) {
        return fromStringMap().value(str, Invalid);
    }

    static const QMap<FileCategoryType, QString>& toStringMap() {
        static QMap<FileCategoryType, QString> map = {
            {Doc, "doc"},
            {Header, "header"},
            {Include, "include"},
            {Library, "library"},
            {Object, "object"},
            {Source, "source"},
            {SourceC, "sourceC"},
            {SourceCpp, "sourceCpp"},
            {SourceAsm, "sourceAsm"},
            {LinkerScript, "linkerScript"},
            {Utility, "utility"},
            {Image, "image"},
            {Other, "other"},
            {PreIncludeGlobal, "preIncludeGlobal"},
            {PreIncludeLocal, "preIncludeLocal"},
            {GenSource, "genSource"},
            {GenHeader, "genHeader"},
            {GenParams, "genParams"},
            {GenAsset, "genAsset"}
        };
        return map;
    }

    static const QMap<QString, FileCategoryType>& fromStringMap() {
        static QMap<QString, FileCategoryType> map = {
            {"doc", Doc},
            {"header", Header},
            {"include", Include},
            {"library", Library},
            {"object", Object},
            {"source", Source},
            {"sourceC", SourceC},
            {"sourceCpp", SourceCpp},
            {"sourceAsm", SourceAsm},
            {"linkerScript", LinkerScript},
            {"utility", Utility},
            {"image", Image},
            {"other", Other},
            {"preIncludeGlobal", PreIncludeGlobal},
            {"preIncludeLocal", PreIncludeLocal},
            {"genSource", GenSource},
            {"genHeader", GenHeader},
            {"genParams", GenParams},
            {"genAsset", GenAsset}
        };
        return map;
    }
};

#endif // PDSCFILECATEGORY_H
