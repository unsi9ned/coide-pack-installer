#ifndef DBCONF_H
#define DBCONF_H

#include <QApplication>

#define DB_FILE         "/ICoodat.dll"
#define BIN_DIR         "/bin"
#define CONFIG_DIR      "/config"
#define DEBUGGER_DIR    CONFIG_DIR + "/debugger"
#define FLASH_DIR       "/flash"
#define DB_PATH     QApplication::applicationDirPath() + BIN_DIR + DB_FILE
#define IDE_PATH    QApplication::applicationDirPath()
#define CONFIG_FILE QApplication::applicationDirPath() + "/config.dat"

#endif // DBCONF_H
