#ifndef DEDUGALGORITHM_H
#define DEDUGALGORITHM_H

#include <QObject>
#include <QString>
#include <QByteArray>
#include <QMap>
#include <QDateTime>
#include "common/constants.h"
#include "models/mcu/devicehierarchynode.h"

class DebugAlgorithm : public DeviceHierarchyNode
{

private:

    int       _coId;
    QString   _name;
    QString   _description;
    QDateTime _createDate;
    QDateTime _updateDate;
    QString   _timeUUID;
    int       _documentId;

public:
    DebugAlgorithm(const QString& name = QString());

    void setCoId(int id);
    void setName(QString name);
    void setProcessor(const QString& core);
    void setDescription(const QString& description);
    void setCreateDate(const QString& dt);
    void setUpdateDate(const QString& dt);
    void setTimeUUID(const QString& uuid);
    void setDocumentId(int id);

    int coId() const;
    qint32 getCoMaxId() const {return ID_DEBUG_ALGO_LAST; }
    QString name() const;
    QString description() const;
    QString creationDate(QString dtFormat = QString("yyyy-MM-dd HH:mm:ss")) const;
    QString updateDate(QString dtFormat = QString("yyyy-MM-dd HH:mm:ss")) const;
    QString timeUUID() const;
    int documentId() const;

    bool isNull() const;

    QString generateTimeUUID();
    QString getPath() const;
};

#endif // DEDUGALGORITHM_H
