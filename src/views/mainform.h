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

#include "views/aboutdialog.h"
#include "viewmodels/mcubrowserviewmodel.h"

namespace Ui {
class MainForm;
}

class MainForm : public QMainWindow
{
    Q_OBJECT

private:
    QAction * actionLoadDfp;
    QAction * actionReloadDfp;
    QAction * actionDbOptimize;
    QAction * actionInstall;
    QAction * actionSettings;

    McuBrowserViewModel * m_mcuBrowserViewModel;

    AboutDialog* aboutProgDialog;

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

    void showMcuDetails();
    void clearForm();
    void lockUI(bool enabled);
    void onDeviceTreeItemClicked(QTreeWidgetItem *item, int column);

private:
    void updateTreeFromModel();
    QTreeWidgetItem* createTreeItem(const DeviceNode& node, QTreeWidgetItem* parent = nullptr);

    void expandDeviceTree();
    QTreeWidgetItem* findVendorItem(const QString& vendor);
    QTreeWidgetItem* findChildItem(QTreeWidgetItem * parent, const QString& text);

    void updateComponentsTree();
    QTreeWidgetItem* createComponentTreeItem(const ComponentNode& node, QTreeWidgetItem* parent = nullptr);

    Ui::MainForm *ui;
};

#endif // MAINFORM_H
