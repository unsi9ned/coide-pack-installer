#ifndef PACKMANAGER_H
#define PACKMANAGER_H

#include <QObject>
#include "packdescription.h"

class PackManager : public QObject
{
    Q_OBJECT
private:

public:
    explicit PackManager(QObject *parent = 0);

    void readPackDescription(PackDescription& pack);
    bool packInstall(PackDescription& pack, QString &errorString);

private:
    QString findPackDescriptionFile(const PackDescription& pack, const QString& extension);
    bool extractPackDescriptionFile(PackDescription& pack, QString& errorString);
    bool extractSVD(PackDescription& pack, QString& errorString);
    bool makeSvdDatabase(PackDescription& pack, QString& errorString);
    void loadCoComponents(PackDescription& pack);
    QStringList getFullFileList(PackDescription& pack);
    QStringList getCmsisFileList(PackDescription& pack, const QString version);
    bool extractSources(PackDescription& pack, QString& errorString);
    bool createComponentMirrors(PackDescription& pack, QString& errorString);

signals:
    void errorOccured(QString e);
    void eventOccured(QString e);

public slots:

private slots:
    void debugPrintMessage(QString e);
};

#endif // PACKMANAGER_H
