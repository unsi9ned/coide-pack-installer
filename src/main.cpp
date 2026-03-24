#include <stdlib.h>
#include <QApplication>
#include <QCoreApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QStringList>

#include "views/mainform.h"
#include "services/logger.h"
#include "common/version.h"

//------------------------------------------------------------------------------
// Запуск приложения в режиме GUI
//------------------------------------------------------------------------------
int runGui(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Инициализация логгера
    qRegisterMetaType<LogMessage>("LogMessage");

    // Настройка логгера
    Logger::instance()->setLogLevel(LogLevel::Debug);
    Logger::instance()->enableConsoleLogging(true);

    // Путь для файла лога
    Logger::instance()->enableFileLogging(QApplication::applicationDirPath() + "/log.txt");

    LOG_INFO("Main", "Application started");

    MainForm w;
    w.show();

    int result = a.exec();

    LOG_INFO("Main", "Application finished");
    return result;
}

//------------------------------------------------------------------------------
// Запуск приложения в режиме CLI
//------------------------------------------------------------------------------
int runConsole(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    a.setApplicationVersion(APP_VERSION);

    // Инициализация логгера
    qRegisterMetaType<LogMessage>("LogMessage");

    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addVersionOption();

    QCommandLineOption ideOption(
        QStringList() << "ide",
        "Path to CoIDE directory",
        "ide_path"
    );
    parser.addOption(ideOption);

    parser.parse(a.arguments());

    if (parser.isSet("help"))
    {
        parser.showHelp();
        return 0;
    }

    if (parser.isSet("version"))
    {
        parser.showVersion();
        return 0;
    }

    QString idePath;

    // Проверяем наличие параметра
    if (parser.isSet(ideOption))
    {
        idePath = parser.value(ideOption);
        printf("Path to CoIDE: %s\n", idePath.toLatin1().constData());
    }

    //LOG_INFO("Main", "IDE path: " + idePath);

    return 0;
}

//------------------------------------------------------------------------------
// Точка входа
//------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
    if(argc > 1)
        return runConsole(argc, argv);
    else
        return runGui(argc, argv);
}

