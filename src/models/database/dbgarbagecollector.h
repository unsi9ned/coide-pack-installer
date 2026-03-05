#ifndef DBGARBAGECOLLECTOR_H
#define DBGARBAGECOLLECTOR_H

#include <QObject>

class DBGarbageCollector : public QObject
{
    Q_OBJECT

private:
    QString _errorString;

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

signals:
    void eventOccured(QString e);
    void errorOccured(QString e);

private slots:
    void printEvents(QString e);

public slots:
};

#endif // DBGARBAGECOLLECTOR_H
