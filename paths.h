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
    static constexpr const char* CO_TEMP_DIR = "temp";
    static constexpr const char* CO_REPO_DIR = "repo";
    static constexpr const char* CO_COMPONENTS_DIR = "Components";
    static constexpr const char* CO_DRIVERS_DIR = "Drivers";
    static constexpr const char* APP_SETTINGS = "config.dat";
    static constexpr const char* APP_UTILS_DIR = "utils";
    static constexpr const char* SEVEN_ZIP = "7z.exe";

    static const QMap<QString, QString> _coDebugAlgorithmsMap;

    explicit Paths(QObject *parent = nullptr);
    ~Paths();

    static Paths* m_instance;

    QString _coIdeDirectory;

public:
    Paths(const Paths&) = delete;
    Paths& operator=(const Paths&) = delete;

    static Paths* instance();

    QString appSettingsFile();
    QString sevenZipExe();
    QString coIdeDir();
    QString coIdeDatabaseFile();
    QString coIdeDebugAlgorithmDir();
    QString coIdeFlashAlgorithmDir();
    QString coIdeDebugAlgorithmFileName(const QString& coreName);
    QString coIdeDebugAlgorithmFile(const QString& coreName);
    QString coIdeDataDir();
    QString coIdePacketsDir();
    QString coIdePackDir(const QString& vendor, const QString& release);
    QString coIdeTempDir();
    QString coIdeRepositoryDir();
    QString coIdeComponentsDir();
    QString coIdeComponentDir(int id, const QString& name);
    QString coIdeDriversDir();
    QString coIdeDriverDir(int id, const QString& name);

    void setCoIdeDir(const QString& dir);

signals:

public slots:
};

#endif // PATHS_H
