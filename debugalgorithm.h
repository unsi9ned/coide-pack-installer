#ifndef DEDUGALGORITHM_H
#define DEDUGALGORITHM_H

#include <QObject>
#include <QString>
#include <QByteArray>
#include <QMap>

class DebugAlgorithm
{

private:

    int     _coId;
    QString _name;
    const static QMap<QString, QString> coDebugAlgorithmsMap;

public:
    DebugAlgorithm(const QString& name = QString());

    void setCoId(int id);
    void setName(QString name);
    void setProcessor(const QString& core);

    int coId() const;
    QString name() const;

    bool isNull();
};

#endif // DEDUGALGORITHM_H
