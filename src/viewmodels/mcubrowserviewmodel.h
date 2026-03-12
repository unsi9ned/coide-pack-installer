#ifndef MCUBROWSERVIEWMODEL_H
#define MCUBROWSERVIEWMODEL_H

#include <QObject>

class McuBrowserViewModel : public QObject
{
    Q_OBJECT
public:
    explicit McuBrowserViewModel(QObject *parent = 0);

signals:

public slots:
};

#endif // MCUBROWSERVIEWMODEL_H