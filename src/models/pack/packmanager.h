#ifndef PACKMANAGER_H
#define PACKMANAGER_H

#include <QObject>
#include "packdescription.h"
#include "common/loggable.h"

class PackManager : public QObject, public Loggable
{
    Q_OBJECT
private:
    QString m_lastError;

protected:
    QString logSource() const override { return "PackManager"; }

public:
    explicit PackManager(QObject *parent = 0);

    void readPackDescription(PackDescription& pack);
    bool packInstall(PackDescription& pack, QString &errorString);
    QString lastError() const {return m_lastError;}

private:
    void logError(const QString& error);
    QString findPackDescriptionFile(const PackDescription& pack, const QString& extension);
    bool extractPackDescriptionFile(PackDescription& pack, QString& errorString);
    bool extractSVD(PackDescription& pack, QString& errorString);
    bool extractFLM(PackDescription& pack, QString& errorString);
    bool makeSvdDatabase(PackDescription& pack, QString& errorString);
    void loadCoComponents(PackDescription& pack);
    QStringList getFullFileList(PackDescription& pack);
    QStringList getCmsisFileList(PackDescription& pack, const QString version);
    bool extractSources(PackDescription& pack, QString& errorString);
    bool createComponentMirrors(PackDescription& pack, QString& errorString);
    void updatePaths(PackDescription& pack);

signals:

public slots:

private slots:

};

#endif // PACKMANAGER_H
