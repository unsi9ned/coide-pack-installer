#include <QDateTime>
#include "logger.h"

Logger* Logger::m_instance = nullptr;
QMutex Logger::m_mutex;

//------------------------------------------------------------------------------
// Создание единственного экземпляра
//------------------------------------------------------------------------------
Logger *Logger::instance()
{
    QMutexLocker locker(&m_mutex);

    if (!m_instance)
    {
        m_instance = new Logger();
    }
    return m_instance;
}

//------------------------------------------------------------------------------
// Конструктор
//------------------------------------------------------------------------------
Logger::Logger(QObject *parent) :
    QObject(parent),
    m_fileStream(&m_logFile)
{

}

//------------------------------------------------------------------------------
// Освобождаем ресурсы
//------------------------------------------------------------------------------
Logger::~Logger()
{
    if(m_logFile.isOpen())
        m_logFile.close();

    if(m_instance)
        delete m_instance;
}

//------------------------------------------------------------------------------
// Добавление сообщения в лог
//------------------------------------------------------------------------------
void Logger::log(LogLevel level,
                 const QString& message,
                 const QString& source,
                 const char* file,
                 int line,
                 const char* function)
{
    if (level < m_minLevel) return;

   QMutexLocker locker(&m_logMutex);

   LogMessage logMsg;
   logMsg.level = level;
   logMsg.source = source;
   logMsg.message = message;
   logMsg.file = file ? QString(file) : QString();
   logMsg.line = line;
   logMsg.function = function ? QString(function) : QString();
   logMsg.timestamp = QDateTime::currentDateTime();

   if (m_consoleOutput)
   {
       fprintf(stderr, "%s\n", logMsg.toString().toLocal8Bit().constData());
       fflush(stderr);
   }

   if (m_fileLoggingEnabled && m_logFile.isOpen())
   {
       m_fileStream << logMsg.toString() << "\n";
       m_fileStream.flush();
   }

   QMetaObject::invokeMethod(this, "emitMessageLogged",
                             Qt::QueuedConnection,
                             Q_ARG(LogMessage, logMsg));
}

//------------------------------------------------------------------------------
// Разрешить логирование в файл
//------------------------------------------------------------------------------
void Logger::enableFileLogging(const QString& filePath)
{
    QMutexLocker locker(&m_logMutex);

    if (m_logFile.isOpen())
    {
        m_logFile.close();
    }

    m_logFile.setFileName(filePath);

    if (m_logFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text))
    {
        m_fileLoggingEnabled = true;
        m_fileStream << "=== Log started at "
                     << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")
                     << " ===\n";
        m_fileStream.flush();
    }
}

//------------------------------------------------------------------------------
// Отключить логирование в файл
//------------------------------------------------------------------------------
void Logger::disableFileLogging()
{
    QMutexLocker locker(&m_logMutex);

    if (m_logFile.isOpen())
    {
        m_fileStream << "=== Log ended at "
                     << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")
                     << " ===\n";
        m_logFile.close();
    }

    m_fileLoggingEnabled = false;
}

//------------------------------------------------------------------------------
// Вспомогательный слот для испускания сигнала messageLogged
//------------------------------------------------------------------------------
void Logger::emitMessageLogged(const LogMessage& logMsg)
{
    emit messageLogged(logMsg);
}
