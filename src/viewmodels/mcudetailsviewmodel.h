#ifndef MCUDETAILSVIEWMODEL_H
#define MCUDETAILSVIEWMODEL_H

#include <QObject>

class McuDetailsViewModel : public QObject
{
    Q_OBJECT
public:
    explicit McuDetailsViewModel(QObject *parent = 0);

signals:

public slots:
};

#endif // MCUDETAILSVIEWMODEL_H