#ifndef QCLICKABLELABEL_H
#define QCLICKABLELABEL_H

#include <QWidget>
#include <QLabel>
#include <QObject>

class QClickableLabel : public QLabel
{
    Q_OBJECT
public:
    QClickableLabel(QWidget * widget = Q_NULLPTR) : QLabel(widget)
    {

    }

protected:
    void mousePressEvent(QMouseEvent *event)
    {
        emit clicked();
    }

signals:

    void clicked();
};

#endif // QCLICKABLELABEL_H
