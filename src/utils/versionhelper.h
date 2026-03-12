#ifndef VERSIONHELPER_H
#define VERSIONHELPER_H

#include <QString>
#include <QStringList>

class VersionHelper
{
public:

    static QString compilationVersion()
    {
        QString compilation_date = QString(__DATE__);
        QStringList months;
        QString date_part;
        QString time_part = QString(__TIME__);
        int month = 0;

        months << "Jan" << "Feb" << "Mar" << "Apr" << "May" << "Jun" <<
                  "Jul" << "Aug" << "Sep" << "Oct" << "Nov" << "Dec";

        for(int m = 0; m < months.count(); m++)
        {
            if(months.at(m) == compilation_date.mid(0, 3))
            {
                month = m + 1;
                break;
            }
        }

        if(month == 0)
        {
            date_part = "19000101";
        }
        else
        {
            date_part = compilation_date.mid(7, 4);
            date_part += QString("%1").arg(month, 2, 10, QChar('0'));
            date_part += compilation_date.mid(4, 2).replace(' ', '0');
        }

        time_part = time_part.mid(0, 2) + time_part.mid(3, 2);

        return date_part + "." + time_part;
    }

    static QString buildDateTime() {
        return QString(__DATE__) + " " + QString(__TIME__);
    }

    static int buildYear() {
        return QString(__DATE__).mid(7, 4).toInt();
    }
};

#endif // VERSIONHELPER_H
