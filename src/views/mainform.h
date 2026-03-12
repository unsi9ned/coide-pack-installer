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
#include <QTreeWidgetItem>

#include "viewmodels/mainviewmodel.h"

namespace Ui {
class MainForm;
}

class MainForm : public QMainWindow
{
    Q_OBJECT

    enum DeviceTreeItemType
    {
        TypeVendor,
        TypeFamily,
        TypeSeries,
        TypeMcu
    };

private:
    QAction * actionLoadDfp;
    QAction * actionReloadDfp;
    QAction * actionDbOptimize;
    QAction * actionInstall;
    QAction * actionSettings;

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
    void loadDFP(bool hideFileDialog = false);


    void showFeatures(QTreeWidgetItem * item);
    void clearForm();
    void updateDeviceTree();
    void onDeviceTreeItemClicked(QTreeWidgetItem *item, int column);
    void updateComponentsTree();

private:
    void showFeatures(const QString& vendor, const QString& core, const QString& series, const QString& mcu);
    void selectMcu(const QString& vendor,
                   const QString& family,
                   const QString& series,
                   const QString& mcu);
    void updateComponentsTree(QTreeWidgetItem *parentItem, Component * component);
    QTreeWidgetItem* findVendorItem(const QString& vendor);
    QTreeWidgetItem* findChildItem(QTreeWidgetItem * parent, const QString& text);


    Ui::MainForm *ui;
};

#endif // MAINFORM_H
