#ifndef ZIPARCHIVE_H
#define ZIPARCHIVE_H

#include <QObject>
#include <QProcess>
#include <QFileInfo>
#include <QList>
#include <QDateTime>

class ZipArchive : public QObject
{
    Q_OBJECT

public:
    struct ArchiveEntry
    {
        // Основная информация
        QString name;              // Имя внутри архива
        QString fullPath;          // Полный путь внутри архива
        qint64 size = 0;           // Оригинальный размер
        qint64 compressedSize = 0; // Сжатый размер
        QDateTime modified;        // Дата модификации
        bool isDir = false;        // Это папка?

        // Дополнительная информация
        QString extension;         // Расширение файла
        QString comment;           // Комментарий к файлу
        QString crc32;             // Контрольная сумма
        quint32 permissions = 0;   // Права доступа (Unix)
        QString owner;             // Владелец
        QString group;             // Группа

        // Методы
        double compressionRatio() const
        {
            if (size == 0) return 0.0;
            return 100.0 - (compressedSize * 100.0 / size);
        }

        QString ratioToString() const
        {
            return QString::number(compressionRatio(), 'f', 1) + "%";
        }
    };

    explicit ZipArchive(QObject *parent = 0);

    QList<ArchiveEntry> listContents(const QString& pathToArchive, QString inheritPath = QString());
    bool extractFile(const QString& pathToArchive,
                     const QString& destDir,
                     const QString& pathToFile);

private:
    ArchiveEntry from7zOutput(const QMap<QString, QStringList> &fileInfo);
    QDateTime parseDateTime(const QString& dateTimeStr);
    QStringList execute(QStringList args);

signals:

public slots:
};

#endif // ZIPARCHIVE_H
