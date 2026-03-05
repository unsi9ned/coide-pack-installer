#ifndef LOGGER_H
#define LOGGER_H

#include <QObject>
#include <QApplication>
#include <QFile>

#define LOG_FILE_PATH    QApplication::applicationDirPath() + "/log.txt"

class Logger : public QObject
{
    Q_OBJECT
private:
    explicit Logger(QObject *parent = nullptr);
    ~Logger();

    static Logger* m_instance;

    QFile logFile;

public:
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    static Logger* instance();

signals:

public slots:
    void addEvent(const QString& event);
};

#endif // LOGGER_H
