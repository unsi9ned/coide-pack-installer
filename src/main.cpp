#include <QApplication>
#include "views/mainform.h"
#include "services/logger.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Регистрируем тип для передачи между потоками
    qRegisterMetaType<LogMessage>("LogMessage");

    // Настройка логгера
    Logger::instance()->setLogLevel(LogLevel::Debug);
    Logger::instance()->enableConsoleLogging(true);

    // Запись в файл
    Logger::instance()->enableFileLogging(QApplication::applicationDirPath() + "/log.txt");

    LOG_INFO("Main", "Application started");

    MainForm w;
    w.show();

    int result = a.exec();

    LOG_INFO("Main", "Application finished");
    return result;
}
