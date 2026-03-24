#ifndef LOGGER_H
#define LOGGER_H

#include <QObject>
#include <QApplication>
#include <QFile>
#include <QMutex>
#include <QTextStream>
#include "common/loglevels.h"

class Logger : public QObject
{
    Q_OBJECT
private:
    explicit Logger(QObject *parent = nullptr);
    ~Logger();

    static Logger* m_instance;
    static QMutex m_mutex;

    QFile m_logFile;
    QTextStream m_fileStream;
    LogLevel m_minLevel = LogLevel::Info;
    bool m_consoleOutput = true;
    bool m_fileLoggingEnabled = false;
    QMutex m_logMutex;

    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

public:


    static Logger* instance();

    void log(LogLevel level, const QString& message, const QString& source,
             const char* file = nullptr, int line = 0, const char* function = nullptr);

    // Удобные обертки
    void debug(const QString& message, const QString& source,
               const char* file = nullptr, int line = 0, const char* function = nullptr)
    {
        log(LogLevel::Debug, message, source, file, line, function);
    }

    void info(const QString& message, const QString& source,
              const char* file = nullptr, int line = 0, const char* function = nullptr)
    {
        log(LogLevel::Info, message, source, file, line, function);
    }

    void warning(const QString& message, const QString& source,
                 const char* file = nullptr, int line = 0, const char* function = nullptr)
    {
        log(LogLevel::Warning, message, source, file, line, function);
    }

    void error(const QString& message, const QString& source,
               const char* file = nullptr, int line = 0, const char* function = nullptr)
    {
        log(LogLevel::Error, message, source, file, line, function);
    }

    void critical(const QString& message, const QString& source,
                  const char* file = nullptr, int line = 0, const char* function = nullptr)
    {
        log(LogLevel::Critical, message, source, file, line, function);
    }

    // Настройки
    void setLogLevel(LogLevel level) { m_minLevel = level; }
    void enableFileLogging(const QString& filePath);
    void disableFileLogging();
    void enableConsoleLogging(bool enable) { m_consoleOutput = enable; }

signals:
    void messageLogged(const LogMessage& message);

private slots:
    void emitMessageLogged(const LogMessage& logMsg);
};

// Удобные макросы
#define LOG_DEBUG(src, msg) Logger::instance()->debug(msg, src, __FILE__, __LINE__, __FUNCTION__)
#define LOG_INFO(src, msg)  Logger::instance()->info(msg, src, __FILE__, __LINE__, __FUNCTION__)
#define LOG_WARN(src, msg)  Logger::instance()->warning(msg, src, __FILE__, __LINE__, __FUNCTION__)
#define LOG_ERROR(src, msg) Logger::instance()->error(msg, src, __FILE__, __LINE__, __FUNCTION__)
#define LOG_CRIT(src, msg)  Logger::instance()->critical(msg, src, __FILE__, __LINE__, __FUNCTION__)

#endif // LOGGER_H
