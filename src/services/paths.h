#ifndef PATHS_H
#define PATHS_H

#include <QObject>
#include <QString>

class Paths : public QObject
{
    Q_OBJECT

private:
    static constexpr const char* CO_BIN_DIR = "bin";
    static constexpr const char* CO_DATABASE = "bin/ICoodat.dll";
    static constexpr const char* CO_DEBUG_ALGORITHM_DIR = "config/debugger";
    static constexpr const char* CO_FLASH_ALGORITHM_DIR = "flash";
    static constexpr const char* CO_DATA_DIR = "data";
    static constexpr const char* CO_PACK_DIR = "pack";
    static constexpr const char* CO_CMSIS_PACK_DIR = "CMSIS";
    static constexpr const char* CO_TEMP_DIR = "temp";
    static constexpr const char* CO_REPO_DIR = "repo";
    static constexpr const char* CO_COMPONENTS_DIR = "Components";
    static constexpr const char* CO_DRIVERS_DIR = "Drivers";
    static constexpr const char* CO_EXAMPLES_DIR = "Examples";
    static constexpr const char* APP_SETTINGS = "config.ini";
    static constexpr const char* APP_LOG = "log.txt";
    static constexpr const char* APP_UTILS_DIR = "utils";
    static constexpr const char* SEVEN_ZIP = "7z.exe";
    static constexpr const char* APP_CMSIS_DIR = "CMSIS";
    static constexpr const char* CMSIS_CORE_ZIP = "CMSIS_Core_%VERSION%.zip";

    static const QMap<QString, QString> _coDebugAlgorithmsMap;
    static const QMap<QString, QString> _cmsisCoreMap;

    explicit Paths(QObject *parent = nullptr);
    ~Paths();

    static Paths* m_instance;

public:
    Paths(const Paths&) = delete;
    Paths& operator=(const Paths&) = delete;

    static Paths* instance();

    QString appDirPath();
    QString appSettingsFile();
    QString appLogFile();
    QString sevenZipExe();
    QString cmsisCore(QString version = "0.0.0");
    QString coIdeDir();
    QString coIdeDatabaseFile();
    QString coIdeDebugAlgorithmDir();
    QString coIdeFlashAlgorithmDir();
    QString coIdeDebugAlgorithmFileName(const QString& coreName);
    QString coIdeDebugAlgorithmFile(const QString& coreName);
    QString coIdeDataDir();
    QString coIdePacketsDir();
    QString coIdePackDir(const QString& vendor, const QString& release);
    QString coIdeCmsisDir(QString version = QString());
    QString coIdeTempDir();
    QString coIdeRepositoryDir();
    QString coIdeComponentsDir();
    QString coIdeComponentDir(int id, const QString& name);
    QString coIdeDriversDir();
    QString coIdeDriverDir(int id, const QString& name);
    QString coIdeExamplesDir();
    QString coIdeExampleDir(int id, const QString& name);
    QMap<QString, QString> cmsisCores();

    void setCoIdeDir(const QString& dir);

signals:

public slots:
};

#endif // PATHS_H
