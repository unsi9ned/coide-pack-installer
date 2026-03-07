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

#include "models/pack/pdscparser.h"
#include "models/pack/packdescription.h"
#include "models/pack/packmanager.h"

#include "viewmodels/mainviewmodel.h"

namespace Ui {
class MainForm;
}

class MainForm : public QMainWindow
{
    Q_OBJECT

private:
    PdscParser parser;
    PackDescription pack;
    PackManager packMgr;
    MainViewModel * m_viewModel;

public:
    explicit MainForm(QWidget *parent = 0);
    ~MainForm();

private:

    QString compilationVersion();

private slots:

    void delayedInit();
    void showErrorMessage(QString e);
    void showInfoMessage(QString i);
    void printLogMessages(QString msg);

    void on_pushButtonDataLoad_clicked();
    void on_pushButtonSave_clicked();
    void on_pushButtonSetIdePath_clicked();
    void on_pushButtonDbOptimize_clicked();
    void showLogContextMenu(const QPoint &pos);

    void changeCoIDEPath();
    void loadDFP();

private:
    Ui::MainForm *ui;
};

#endif // MAINFORM_H
