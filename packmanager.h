#ifndef PACKMANAGER_H
#define PACKMANAGER_H

#include <QObject>
#include "packdescription.h"

class PackManager : public QObject
{
    Q_OBJECT
private:
    PackDescription _pdsc;
public:
    explicit PackManager(QObject *parent = 0);

    void readPackDescription(const QString& pathToPack, PackDescription& pack);
    void packInstall(const PackDescription& pack);

private:
    QString findPDSC(const QString& pathToPack);
    bool extractPDSC(const QString& pathToPack, const QString& pathToPdsc);
    void svdFileRegister(const QString& vendorName, const QString& svdFileName);

signals:
    void errorOccured(QString e);
public slots:
};

#endif // PACKMANAGER_H
