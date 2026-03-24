#ifndef LOGGABLE_H
#define LOGGABLE_H

#include <QString>
#include "services/logger.h"

class Loggable
{
public:
    virtual ~Loggable() = default;

protected:
    virtual QString logSource() const = 0;

    void logDebug(const QString& msg) const {
        Logger::instance()->debug(msg, logSource());
    }

    void logInfo(const QString& msg) const {
        Logger::instance()->info(msg, logSource());
    }

    void logWarning(const QString& msg) const {
        Logger::instance()->warning(msg, logSource());
    }

    void logError(const QString& msg) const {
        Logger::instance()->error(msg, logSource());
    }

    void logCritical(const QString& msg) const {
        Logger::instance()->critical(msg, logSource());
    }
};

#endif // LOGGABLE_H
