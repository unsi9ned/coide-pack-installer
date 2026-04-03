#ifndef LOGLEVELS_H
#define LOGLEVELS_H

#include <QString>
#include <QMetaType>
#include <QDateTime>

enum class LogLevel
{
    Debug = 0,
    Info = 1,
    Warning = 2,
    Error = 3,
    Critical = 4
};

inline QString logLevelToString(LogLevel level)
{
    switch(level)
    {
        case LogLevel::Debug: return "DEBUG";
        case LogLevel::Info: return "INFO";
        case LogLevel::Warning: return "WARN";
        case LogLevel::Error: return "ERROR";
        case LogLevel::Critical: return "CRIT";
        default: return "UNKNOWN";
    }
}

struct LogMessage
{
    LogLevel level;
    QString source;
    QString message;
    QString file;
    int line;
    QString function;
    QDateTime timestamp;

    LogMessage() : level(LogLevel::Info), line(0) {}

    QString toString() const
    {
        return QString("[%1] [%2] %3: %4")
                .arg(timestamp.toString("hh:mm:ss.zzz"))
                .arg(logLevelToString(level), -5)
                .arg(source)
                .arg(message);
    }

    QString toHtml() const
    {
        QString color;

        switch(level)
        {
            case LogLevel::Debug: color = "gray"; break;
            case LogLevel::Info: color = "black"; break;
            case LogLevel::Warning: color = "orange"; break;
            case LogLevel::Error: color = "red"; break;
            case LogLevel::Critical: color = "darkred"; break;
        }
        return QString("<font color='%1'>[%2] [%3] %4: %5</font>")
                .arg(color)
                .arg(timestamp.toString("hh:mm:ss.zzz"))
                .arg(logLevelToString(level), -5)
                .arg(source)
                .arg(message);
    }
};

Q_DECLARE_METATYPE(LogMessage)

#endif // LOGLEVELS_H
