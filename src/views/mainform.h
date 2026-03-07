#ifndef MAINFORM_H
#define MAINFORM_H

#include <QMainWindow>
#include <QDebug>
#include <QtSql/QSqlDatabase>
#include <QSqlQuery>
#include <QModelIndex>
#include <QStringList>
#include <QMessageBox>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFileDialog>
#include <QDesktopServices>
#include <QString>
#include <QListWidget>
#include <QPoint>

#include "viewmodels/mainviewmodel.h"

namespace Ui {
class MainForm;
}

class MainForm : public QMainWindow
{
    Q_OBJECT

private:
    MainViewModel * m_viewModel;

public:
    explicit MainForm(QWidget *parent = 0);
    ~MainForm();

private:

private slots:

    void delayedInit();
    void showErrorMessage(QString e);
    void showInfoMessage(QString i);
    void printLogMessages(QString msg);
    void changeCoIDEPath();
    void loadDFP();

private:
    Ui::MainForm *ui;
};

#endif // MAINFORM_H
