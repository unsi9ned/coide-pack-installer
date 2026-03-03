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

#include "pdscparser.h"
#include "packdescription.h"
#include "packmanager.h"

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

public:
    explicit MainForm(QWidget *parent = 0);
    ~MainForm();

private:

    QString compilationVersion();

private slots:

    void showError(QString e);
    void showInfo(QString i);

    void showDFPInfo(QModelIndex index);
    void showFamilyList(QModelIndex index);
    void showSeriesList(QModelIndex index);
    void showMcuList(QModelIndex index);
    void showFeatures(QModelIndex index);

    void showFamilyList(int index);
    void showSeriesList(int index);
    void showMcuList(int index);

    void showFilteredComponents(int itemIndex = 1);

    void refreshData();
    void refreshAlgorithms();
    void refreshNewDebAlgorithm();
    void refreshNewFlashAlgorithm();
    void selectNewTab(int t);
    void visitWebsite();

    void on_pushButtonDataLoad_clicked();
    void on_pushButtonSave_clicked();
    void on_pushButtonSetIdePath_clicked();
    void on_pushButtonDbOptimize_clicked();

    void changeCoIDEPath();
    void loadDFP();

private:
    void loadDFP(const QString& path);
    int extractIdFromItemText(QString text);
    QString extractNameFromItemText(QString text);
    void selectListItemByText(QListWidget *listWidget, const QString& text);
    Ui::MainForm *ui;
};

#endif // MAINFORM_H
