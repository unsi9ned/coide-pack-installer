#ifndef QCLICKABLELABEL_H
#define QCLICKABLELABEL_H

#include <QWidget>
#include <QLabel>
#include <QObject>
#include <QDesktopServices>
#include <QUrl>

class QClickableLabel : public QLabel
{
    Q_OBJECT

public:
    QClickableLabel(QWidget * widget = Q_NULLPTR) : QLabel(widget)
    {
        connect(this, &QClickableLabel::clicked, this, &QClickableLabel::visitWebsite);
    }

protected:

    void mousePressEvent(QMouseEvent *event)
    {
        QLabel::mousePressEvent(event);
        emit clicked();
    }

signals:

    void clicked();

private slots:

    void visitWebsite()
    {
        QDesktopServices::openUrl(QUrl(this->text()));
    }
};

#endif // QCLICKABLELABEL_H
