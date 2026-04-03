#ifndef DBGARBAGECOLLECTOR_H
#define DBGARBAGECOLLECTOR_H

#include <QObject>
#include "common/loggable.h"

class DBGarbageCollector : public QObject, public Loggable
{
    Q_OBJECT

private:
    QString _errorString;

protected:
    QString logSource() const override { return "DBGarbageCollector"; }

public:
    explicit DBGarbageCollector();

    bool deleteObsoleteData();
    bool deleteUnnecessaryTables();

    QString errorString();

private:

    bool cleanUsers();
    bool cleanComponents();
    bool cleanExamples();
    bool removeDirectory(const QString &dirPath);
    void logError(const QString& e);

signals:

private slots:

public slots:
};

#endif // DBGARBAGECOLLECTOR_H
