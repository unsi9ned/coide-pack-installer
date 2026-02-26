#include "makelink.h"

MakeLink::MakeLink()
{

}

//------------------------------------------------------------------------------
// Создание ссылки на файл
//------------------------------------------------------------------------------
bool MakeLink::createLink(const QString &linkPath,
                          const QString &targetPath,
                          MakeLink::LinkType type)
{
    QStringList arguments;

    // Добавляем флаг в зависимости от типа
    switch(type)
    {
        case SymbolicDirectory:
            arguments << "/D";
            break;
        case HardLink:
            arguments << "/H";
            break;
        case Junction:
            arguments << "/J";
            break;
        case SymbolicFile:
        default:
            // Без флага - символическая ссылка на файл
            break;
    }

    // Добавляем пути (в формате Windows)
    arguments << QDir::toNativeSeparators(linkPath)
              << QDir::toNativeSeparators(targetPath);

    // Запускаем mklink через cmd
    QProcess process;
    process.start("cmd.exe", QStringList() << "/c" << "mklink" << arguments);
    process.waitForFinished();

    // Проверяем результат
    if (process.exitCode() != 0)
    {
        QString error = process.readAllStandardError();
        QString output = process.readAllStandardOutput();
        qDebug() << "Ошибка создания ссылки:" << error << output;
        return false;
    }

    return true;
}

//------------------------------------------------------------------------------
// Удаление ссылки (просто удаляем файл/папку)
//------------------------------------------------------------------------------
bool MakeLink::removeLink(const QString &linkPath)
{
    QFileInfo info(linkPath);

    // Для папок-ссылок
    if (info.isDir())
    {
        QDir dir(linkPath);
        return dir.rmdir(linkPath);
    }
    // Для файлов-ссылок
    else
    {
        QFile file(linkPath);
        return file.remove();
    }
}

//------------------------------------------------------------------------------
// Проверка, является ли путь ссылкой
//------------------------------------------------------------------------------
bool MakeLink::isLink(const QString &path)
{
#ifdef Q_OS_WIN
        QFileInfo info(path);
        return info.isSymLink();
#else
        return QFileInfo(path).isSymLink();
#endif
}

//------------------------------------------------------------------------------
// Получение цели ссылки
//------------------------------------------------------------------------------
QString MakeLink::getLinkTarget(const QString &linkPath)
{
#ifdef Q_OS_WIN
    QFileInfo info(linkPath);

    if (info.isSymLink())
    {
        return info.symLinkTarget();
    }

    // Для junction точек нужно использовать dir
    QProcess process;
    process.start("cmd.exe", QStringList() << "/c" << "dir" << linkPath);
    process.waitForFinished();
    QString output = process.readAllStandardOutput();

    // Парсим вывод dir для поиска <JUNCTION>
    if (output.contains("<JUNCTION>"))
    {
        QStringList lines = output.split("\n");

        foreach(QString line, lines)
        {
            if (line.contains("<JUNCTION>"))
            {
                // Формат: [дата] [время] <JUNCTION> link [target]
                int pos = line.lastIndexOf(']');

                if (pos != -1)
                {
                    QString target = line.mid(pos + 1).trimmed();
                    return target;
                }
            }
        }
    }
#endif
    return QString();
}
