#include <stdlib.h>
#include <QApplication>
#include <QCoreApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QStringList>

#include "views/mainform.h"
#include "services/logger.h"
#include "common/version.h"
#include "services/paths.h"
#include "services/settings.h"

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

    // Настройка логгера
    Logger::instance()->setLogLevel(LogLevel::Debug);
    Logger::instance()->enableConsoleLogging(true);

    // Путь для файла лога
    Logger::instance()->enableFileLogging(QApplication::applicationDirPath() + "/log.txt");

    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addVersionOption();

    QCommandLineOption ideOption(
        QStringList() << "d" << "ide",
        "Path to CoIDE directory",
        "ide_path"
    );
    parser.addOption(ideOption);

    QCommandLineOption packOption(
        QStringList() << "p" << "pack",
        "Path to Device Family Package (*.pack, *.jpack)",
        "pack_path"
    );
    parser.addOption(packOption);

    QCommandLineOption listDevOption(
        QStringList() << "l" << "devices",
        "Uploading a DFP and displaying a list of devices"
    );
    parser.addOption(listDevOption);

    QCommandLineOption listCompOption(
        QStringList() << "c" << "components",
        "Uploading a DFP and displaying a list of components"
    );
    parser.addOption(listCompOption);

    QCommandLineOption installOption(
        QStringList() << "i" << "install",
        "Install Device Family Package in CoIDE"
    );
    parser.addOption(installOption);

    QCommandLineOption optimizeDbOption(
        QStringList() << "optimize-db",
        "Optimize database (clean unused tables and obsolete data)"
    );
    parser.addOption(optimizeDbOption);

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
    QString pathToArchive;
    bool showDeviceList = parser.isSet(listDevOption);
    bool showComponentList = parser.isSet(listCompOption);
    bool optimizeDb = parser.isSet(optimizeDbOption);
    bool install = parser.isSet(installOption);

    // Путь к CoIDE
    if (parser.isSet(ideOption))
    {
        idePath = parser.value(ideOption);

        if(!idePath.isEmpty())
        {
            Paths::instance()->setCoIdeDir(idePath);
            LOG_INFO("Main", QString("The path to the IDE is set: %1").arg(idePath));
        }
    }

    // Путь к пакету DFP
    if (parser.isSet(packOption))
    {
        pathToArchive = parser.value(packOption);

        if(!pathToArchive.isEmpty())
        {
            Settings::instance()->saveLastLoadedPack(pathToArchive);
            LOG_INFO("Main", QString("The path to the DFP is set: %1").arg(pathToArchive));
        }
    }

    //--------------------------------------------------------------------------
    // Вывод дерева устройств
    //--------------------------------------------------------------------------
    if (showDeviceList)
    {
        LOG_INFO("Main", "Printing the device tree");


        McuBrowserViewModel viewModel;

        if (!viewModel.loadPack())
        {
            LOG_ERROR("Main", "Failed to load pack: " + pathToArchive);
            return 1;
        }

        viewModel.printDeviceTree();

        LOG_INFO("Main", "Device tree printing completed");
    }

    //--------------------------------------------------------------------------
    // Вывод дерева компонентов
    //--------------------------------------------------------------------------
    if (showComponentList)
    {
        LOG_INFO("Main", "Printing the components tree");


        McuBrowserViewModel viewModel;

        if (!viewModel.loadPack())
        {
            LOG_ERROR("Main", "Failed to load pack: " + pathToArchive);
            return 1;
        }

        viewModel.printComponentTree();

        LOG_INFO("Main", "Component tree printing completed");
    }

    if(showDeviceList || showComponentList) return 0;

    //--------------------------------------------------------------------------
    // Запуск процесс оптимизации
    //--------------------------------------------------------------------------
    if(optimizeDb)
    {
        LOG_INFO("Main", "Starting database optimization");

        McuBrowserViewModel viewModel;

        // Сигнал завершения
        QEventLoop loop;
        QObject::connect(&viewModel, &McuBrowserViewModel::dbOptimizeFinished,
                        &loop, &QEventLoop::quit);

        viewModel.optimizeDatabase();
        loop.exec();

        LOG_INFO("Main", "Database optimization completed");
    }

    //--------------------------------------------------------------------------
    // Запуск процесса установки пакета
    //--------------------------------------------------------------------------
    if(install)
    {
        LOG_INFO("Main", "Starting pack installation");

        McuBrowserViewModel viewModel;

        if (!viewModel.loadPack())
        {
            LOG_ERROR("Main", "Failed to load pack: " + pathToArchive);
            return 1;
        }

        LOG_INFO("Main", "Pack loaded successfully");

        // Сигнал завершения
        QEventLoop loop;
        bool success = false;
        QString resultMessage;

        // Подключаем сигналы
        QObject::connect(&viewModel, &McuBrowserViewModel::installStarted,[]()
        {
            LOG_INFO("Main", "Installation started");
        });

        // Подключаемся к сигналу packInstalled
        QObject::connect(&viewModel, &McuBrowserViewModel::packInstalled,
                        [&success, &resultMessage, &loop](bool ok, const QString& message)
        {
            success = ok;
            resultMessage = message;
            loop.quit();
        });

        viewModel.installCurrentPack();

        loop.exec();

        if (success)
        {
            LOG_INFO("Main", "Pack installation completed successfully");
            return 0;
        }
        else
        {
            LOG_ERROR("Main", "Pack installation failed: " + resultMessage);
            return 1;
        }
    }

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

