#include <QDateTime>
#include "logger.h"

Logger* Logger::m_instance = nullptr;

//------------------------------------------------------------------------------
// Создание единственного экземпляра
//------------------------------------------------------------------------------
Logger *Logger::instance()
{
    if(m_instance == nullptr)
       m_instance = new Logger();

    return m_instance;
}

//------------------------------------------------------------------------------
// Конструктор
//------------------------------------------------------------------------------
Logger::Logger(QObject *parent) : QObject(parent)
{
    logFile.setFileName(LOG_FILE_PATH);
}

//------------------------------------------------------------------------------
// Освобождаем ресурсы
//------------------------------------------------------------------------------
Logger::~Logger()
{
    if(logFile.isOpen())
        logFile.close();

    if(m_instance)
        delete m_instance;
}

//------------------------------------------------------------------------------
// Добавление события в лог-файл
//------------------------------------------------------------------------------
void Logger::addEvent(const QString &event)
{
    if(logFile.open(QFile::Append))
    {
        QDateTime now = QDateTime::currentDateTime();
        logFile.write(QString("[%1] - ").arg(now.toString("yyyy/MM/dd hh:mm:ss")).toLatin1());
        logFile.write(event.trimmed().toLatin1());
        logFile.write("\r\n");
        logFile.close();
    }
}
