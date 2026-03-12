#include <QScrollBar>
#include <QTimer>
#include "mainform.h"
#include "ui_mainform.h"
#include "services/paths.h"
#include "utils/ziparchive.h"
#include "models/pack/packmanager.h"
#include "models/database/dbgarbagecollector.h"
#include "services/settings.h"
#include "utils/versionhelper.h"

//------------------------------------------------------------------------------
// Конструктор окна
//------------------------------------------------------------------------------
MainForm::MainForm(QWidget *parent) :
    QMainWindow(parent),
    m_viewModel(new MainViewModel(this)),
    ui(new Ui::MainForm)
{
    ui->setupUi(this);

    // Иконка
    this->setWindowIcon(QIcon(":coide_project.ico"));

    // Панель инструментов
    actionLoadDfp = ui->toolBar->addAction(QIcon(":/img/file_open.png"), "Load DFP");
    actionReloadDfp = ui->toolBar->addAction(QIcon(":/img/file_reload.png"), "Reload DFP");
    actionDbOptimize = ui->toolBar->addAction(QIcon(":/img/cleaning.png"), "DB optimization");
    actionInstall = ui->toolBar->addAction(QIcon(":/img/install.png"), "Package install");
    actionSettings = ui->toolBar->addAction(QIcon(":/img/settings.png"), "Settings");

    // Дерево устройств
    ui->treeWidgetDevices->setColumnCount(1);
    ui->treeWidgetDevices->setHeaderLabel("MCU Browser");
    ui->treeWidgetDevices->setHeaderHidden(true);

    // Дерево компонентов
    ui->treeWidgetComponents->setColumnCount(2);
    ui->treeWidgetComponents->setHeaderLabels(QStringList() << "Component" << "Description");

    // Подключаем выбор элемента
    connect(ui->treeWidgetDevices, &QTreeWidget::itemClicked, this, &MainForm::onDeviceTreeItemClicked);

    // Версия программы
    ui->labelAppVersion->setText(VersionHelper::compilationVersion());
    ui->lineEditIdePath->setText(Paths::instance()->coIdeDir());
    ui->plainTextEdit->setContextMenuPolicy(Qt::CustomContextMenu);

    //--------------------------------------------------------------------------
    // Добавление кнопки очистки в контекстное меню окна логирования
    //--------------------------------------------------------------------------
    connect(ui->plainTextEdit, &QPlainTextEdit::customContextMenuRequested, [this](const QPoint &pos)
    {
        QMenu *menu = ui->plainTextEdit->createStandardContextMenu();

        menu->addSeparator();

        QAction *clearAction = menu->addAction("Clear");
        clearAction->setIcon(style()->standardIcon(QStyle::SP_DialogResetButton));

        connect(clearAction, &QAction::triggered,
                ui->plainTextEdit, &QPlainTextEdit::clear);

        menu->exec(ui->plainTextEdit->mapToGlobal(pos));
        delete menu;
    });

    // Вывод сообщений
#if 0
    connect(&packMgr,
            SIGNAL(errorOccured(QString)),
            SLOT(printLogMessages(QString)));

    connect(&packMgr,
            SIGNAL(eventOccured(QString)),
            SLOT(printLogMessages(QString)));
#endif

    //--------------------------------------------------------------------------
    // Сигналы загрузки
    //--------------------------------------------------------------------------
    connect(m_viewModel, &MainViewModel::loadStarted, [this]()
    {
        ui->statusBar->showMessage("Загрузка...");
        actionLoadDfp->setEnabled(false);
        clearForm();
    });

    connect(m_viewModel, &MainViewModel::loadFinished, [this]() {
        ui->lineEditRelease->setText(m_viewModel->releaseVersion());
        actionLoadDfp->setEnabled(true);
        ui->statusBar->showMessage("Готово", 3000);

        updateDeviceTree();
        updateComponentsTree();

        selectMcu(m_viewModel->currentMcu(),
                  m_viewModel->currentFamily(),
                  m_viewModel->currentSeries(),
                  m_viewModel->currentMcu());
    });

    connect(m_viewModel, &MainViewModel::loadFailed, [this](const QString& error) {
        QMessageBox::critical(this, "Ошибка", error);
        actionLoadDfp->setEnabled(true);
    });

    connect(m_viewModel, &MainViewModel::statusMessage,
            ui->statusBar, &QStatusBar::showMessage);

    //--------------------------------------------------------------------------
    // Сигналы установки
    //--------------------------------------------------------------------------
    connect(m_viewModel, &MainViewModel::installStarted, [this]() {
        ui->statusBar->showMessage("Установка...");
        // можно показать прогресс-бар
    });

    connect(m_viewModel, &MainViewModel::installFinished, [this](bool success, const QString& message) {
        ui->statusBar->showMessage(message, 3000);
        if (success) {
            QMessageBox::information(this, "Успех", message);
        }
    });

    connect(m_viewModel, &MainViewModel::installError, [this](const QString& error) {
        ui->statusBar->showMessage("Ошибка", 5000);
        QMessageBox::critical(this, "Ошибка", error);
    });

    connect(m_viewModel, &MainViewModel::installLogMessage, this, &MainForm::printLogMessages);

    //--------------------------------------------------------------------------
    // Отслеживаем процесс оптимизации
    //--------------------------------------------------------------------------
    connect(m_viewModel, &MainViewModel::dbOptimizeStarted, [this]() {
        actionDbOptimize->setEnabled(false);
        ui->statusBar->showMessage("Оптимизация БД...");
    });

    connect(m_viewModel, &MainViewModel::dbOptimizeFinished, [this]() {
        actionDbOptimize->setEnabled(true);
        ui->statusBar->showMessage("Оптимизация завершена", 3000);
    });

    connect(m_viewModel, &MainViewModel::dbOptimizeError, [this](const QString& error) {
        actionDbOptimize->setEnabled(true);
        ui->statusBar->showMessage("Ошибка: " + error, 5000);
        QMessageBox::warning(this, "Ошибка", error);
    });

    connect(m_viewModel, &MainViewModel::dbLogMessage, this, &MainForm::printLogMessages);

    //--------------------------------------------------------------------------
    // Обновление списков
    //--------------------------------------------------------------------------
#if 0
    connect(m_viewModel, &MainViewModel::vendorsChanged, [this]() {
        ui->listWidgetManufacturer->clear();
        ui->listWidgetManufacturer->addItems(m_viewModel->vendors());
    });

    connect(m_viewModel, &MainViewModel::familiesChanged, [this]() {
        ui->listWidgetFamily->clear();
        ui->listWidgetFamily->addItems(m_viewModel->families());

        if(!m_viewModel->currentVendor().isEmpty())
        {
            auto items = ui->listWidgetManufacturer->findItems(m_viewModel->currentVendor(), Qt::MatchExactly);

            if(!items.isEmpty())
            {
                ui->listWidgetManufacturer->setCurrentItem(items.first());
            }
        }
    });

    connect(m_viewModel, &MainViewModel::seriesChanged, [this]() {
        ui->listWidgetSeries->clear();
        ui->listWidgetSeries->addItems(m_viewModel->series());

        if(!m_viewModel->currentFamily().isEmpty())
        {
            auto items = ui->listWidgetFamily->findItems(m_viewModel->currentFamily(), Qt::MatchExactly);

            if(!items.isEmpty())
            {
                ui->listWidgetFamily->setCurrentItem(items.first());
            }
        }
    });

    connect(m_viewModel, &MainViewModel::mcusChanged, [this]()
    {
        ui->listWidgetMcu->clear();
        ui->listWidgetMcu->addItems(m_viewModel->mcus());

        if(!m_viewModel->currentSeries().isEmpty())
        {
            auto items = ui->listWidgetSeries->findItems(m_viewModel->currentSeries(), Qt::MatchExactly);

            if(!items.isEmpty())
            {
                ui->listWidgetSeries->setCurrentItem(items.first());
            }
        }
    });


    // Обновление детальной информации
    connect(m_viewModel, &MainViewModel::mcuChanged, [this]()
    {
        ui->lineEditFlashStart->setText(m_viewModel->flashStart());
        ui->lineEditFlashSize->setText(m_viewModel->flashSize());
        ui->lineEditRamStart->setText(m_viewModel->ramStart());
        ui->lineEditRamSize->setText(m_viewModel->ramSize());
        ui->plainTextEditFeatures->setPlainText(m_viewModel->features());
        ui->plainTextEditDescription->setPlainText(m_viewModel->description());
        ui->lineEditUrl->setText(m_viewModel->webPageUrl());
        ui->lineEditDatasheetUrl->setText(m_viewModel->datasheetUrl());
        ui->lineEditSVD->setText(m_viewModel->svdLocalPath());

        // Заполнение списка алгоритмов отладки
        ui->comboBoxDebugAlg->clear();

        if(!m_viewModel->debugAlgorithm().isEmpty())
        {
            ui->comboBoxDebugAlg->addItem(m_viewModel->debugAlgorithm());
            ui->comboBoxDebugAlg->setCurrentIndex(0);
        }

        // Заполнение списка алгоритмов программирования
        ui->comboBoxFlashAlg->clear();

        for(int i = 0; i < m_viewModel->flashAlgorithms().count(); i++)
        {
            QString algo = m_viewModel->flashAlgorithms().at(i);

            ui->comboBoxFlashAlg->addItem(algo);

            if(algo == m_viewModel->currentFlashAlgorithm())
            {
                ui->comboBoxFlashAlg->setCurrentIndex(i);
            }
        }

        // Восстанавливаем выбор, если элемент еще существует
        if (!m_viewModel->currentMcu().isEmpty())
        {
            auto items = ui->listWidgetMcu->findItems(m_viewModel->currentMcu(), Qt::MatchExactly);
            if (!items.isEmpty())
            {
                ui->listWidgetMcu->setCurrentItem(items.first());
            }
        }
    });
#endif


    //--------------------------------------------------------------------------
    // Пользовательский ввод
    //--------------------------------------------------------------------------
#if 0
    // Выбор производителя
    connect(ui->listWidgetManufacturer, &QListWidget::currentRowChanged, [this]()
    {
        QListWidgetItem * currentItem = ui->listWidgetManufacturer->currentItem();

        if(currentItem)
        {
            m_viewModel->selectVendor(currentItem->text());
        }
    });

    connect(ui->listWidgetFamily, &QListWidget::currentRowChanged, [this]()
    {
        QListWidgetItem * currentItem = ui->listWidgetFamily->currentItem();

        if(currentItem)
        {
            m_viewModel->selectFamily(currentItem->text());
        }
    });

    connect(ui->listWidgetSeries, &QListWidget::currentRowChanged, [this]()
    {
        QListWidgetItem * currentItem = ui->listWidgetSeries->currentItem();

        if(currentItem)
        {
            m_viewModel->selectSeries(currentItem->text());
        }
    });

    connect(ui->listWidgetMcu, &QListWidget::currentRowChanged, [this]()
    {
        QListWidgetItem * currentItem = ui->listWidgetMcu->currentItem();

        if(currentItem)
        {
            m_viewModel->selectMcu(currentItem->text());
        }
    });
#endif

    // Подключаем кнопку оптимизации БД к команде ViewModel
    connect(actionDbOptimize, &QAction::triggered, m_viewModel, &MainViewModel::optimizeDatabase);

    // Подключаем кнопку установки к команде ViewModel
    connect(actionInstall, &QAction::triggered, m_viewModel, &MainViewModel::installCurrentPack);

    // Подключаем кнопку перезагрузки данных к команде ViewModel
    connect(actionReloadDfp, &QAction::triggered, [this]()
    {
        loadDFP(true);
    });

    // Изменение пути к каталогу CoIDE
    connect(ui->pushButtonSetIdePath, &QPushButton::clicked, this, &MainForm::changeCoIDEPath);
    connect(ui->actionPreferences, SIGNAL(triggered(bool)), SLOT(changeCoIDEPath()));
    connect(actionSettings, &QAction::triggered, this, &MainForm::changeCoIDEPath);

    // Выбор пакета DFP и загрузка
    connect(actionLoadDfp, &QAction::triggered, this, &MainForm::loadDFP);
    connect(ui->actionOpen_DFP, &QAction::triggered, this, &MainForm::loadDFP);

#if 0
    //--------------------------------------------------------------------------
    // Отладка
    //--------------------------------------------------------------------------
    connect(ui->listWidgetManufacturer, &QListWidget::currentRowChanged, [this]() {
       qInfo()  << "manufacturerChanged";
    });

    connect(ui->listWidgetFamily, &QListWidget::currentRowChanged, [this]() {
       qInfo()  << "familyChanged";
    });

    connect(ui->listWidgetSeries, &QListWidget::currentRowChanged, [this]() {
       qInfo()  << "seriesChanged";
    });

    connect(ui->listWidgetMcu, &QListWidget::currentRowChanged, [this]() {
       qInfo()  << "mcuChanged";
    });
    //--------------------------------------------------------------------------
#endif

#if 1
    QMetaObject::invokeMethod(this, "delayedInit", Qt::QueuedConnection);
#else
    delayedInit();
#endif
}

//------------------------------------------------------------------------------
// Деструктор
//------------------------------------------------------------------------------
MainForm::~MainForm()
{
    delete m_viewModel;
    delete ui;
}

//------------------------------------------------------------------------------
// Инициализация программы после прогрузки основного окна
//------------------------------------------------------------------------------
void MainForm::delayedInit()
{
    //Обновление данных на форме
    m_viewModel->loadDeviceFamilyPack();
}

//------------------------------------------------------------------------------
// Отобразить ошибку
//------------------------------------------------------------------------------
void MainForm::showErrorMessage(QString e)
{
    QMessageBox::critical(this, tr("Ошибка"), e, QMessageBox::Ok);
}

//------------------------------------------------------------------------------
// Отобразить информацию
//------------------------------------------------------------------------------
void MainForm::showInfoMessage(QString i)
{
    QMessageBox::information(this, tr("Информация"), i, QMessageBox::Ok);
}

//------------------------------------------------------------------------------
// Изменение пути к каталогу CoIDE
//------------------------------------------------------------------------------
void MainForm::changeCoIDEPath()
{
    QFileDialog dialog(this, tr("CoIDE install path"), Paths::instance()->coIdeDir());

    dialog.setFileMode(QFileDialog::Directory);
    dialog.setOption(QFileDialog::ShowDirsOnly, true);
    dialog.setOption(QFileDialog::DontUseNativeDialog);

    if (dialog.exec())
    {
        QString selectedDir = dialog.selectedFiles().first();

        // Сохраняем выбранную папку
        Paths::instance()->setCoIdeDir(selectedDir);
        ui->lineEditIdePath->setText(selectedDir);
    }
}

//------------------------------------------------------------------------------
// Выбор и загрузка DFP
//------------------------------------------------------------------------------
void MainForm::loadDFP(bool hideFileDialog)
{
    if(hideFileDialog)
    {
        m_viewModel->loadDeviceFamilyPack();
        return;
    }

    QFileInfo packFileInfo(Settings::instance()->lastLoadedPack());
    QFileDialog dialog(this, tr("Device Family Pack"), packFileInfo.path(), "*.pack");

    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setOption(QFileDialog::DontUseNativeDialog);

    if (dialog.exec())
    {
        QString path = dialog.selectedFiles().first();
        Settings::instance()->saveLastLoadedPack(path);
        m_viewModel->loadDeviceFamilyPack();
    }
}

//------------------------------------------------------------------------------
// Печать сообщений об ошибках и событиях
//------------------------------------------------------------------------------
void MainForm::printLogMessages(QString msg)
{
    ui->plainTextEdit->appendPlainText(msg);
    ui->plainTextEdit->verticalScrollBar()->setValue(ui->plainTextEdit->verticalScrollBar()->maximum());
}



//------------------------------------------------------------------------------
// Показать параметры микроконтроллера
//------------------------------------------------------------------------------
void MainForm::showFeatures(QTreeWidgetItem *item)
{
    if(item->data(0, Qt::UserRole).toInt() != MainForm::TypeMcu) return;

    QString vendor = item->parent()->parent()->parent()->text(0);
    QString armCore = item->parent()->parent()->text(0);
    QString series = item->parent()->text(0);
    QString mcu = item->text(0);

    showFeatures(vendor, armCore, series, mcu);
}

//------------------------------------------------------------------------------
// Очистка формы
//------------------------------------------------------------------------------
void MainForm::clearForm()
{
    ui->plainTextEditDescription->clear();
    ui->plainTextEditFeatures->clear();
    ui->lineEditFlashStart->clear();
    ui->lineEditFlashSize->clear();
    ui->lineEditRamStart->clear();
    ui->lineEditRamSize->clear();
    ui->comboBoxFlashAlg->clear();
    ui->comboBoxDebugAlg->clear();
    ui->lineEditRelease->clear();
    ui->lineEditSVD->clear();
    ui->lineEditDatasheetUrl->clear();
    ui->lineEditUrl->clear();
}

//------------------------------------------------------------------------------
// Обновление дерева устройств
//------------------------------------------------------------------------------
void MainForm::updateDeviceTree()
{
    ui->treeWidgetDevices->clear();

    for (QString vendor : m_viewModel->vendors())
    {
        QTreeWidgetItem* vItem = new QTreeWidgetItem(ui->treeWidgetDevices);
        vItem->setText(0, vendor);
        vItem->setData(0, Qt::UserRole, MainForm::TypeVendor);

        m_viewModel->selectVendor(vendor);

        for (QString family : m_viewModel->families())
        {
            QTreeWidgetItem* fItem = new QTreeWidgetItem(vItem);
            fItem->setText(0, family);
            fItem->setData(0, Qt::UserRole, MainForm::TypeFamily);

            m_viewModel->selectFamily(family);

            for (QString series : m_viewModel->series())
            {
                QTreeWidgetItem* sItem = new QTreeWidgetItem(fItem);
                sItem->setText(0, series);
                sItem->setData(0, Qt::UserRole, MainForm::TypeSeries);

                m_viewModel->selectSeries(series);

                for (QString mcu : m_viewModel->mcus())
                {
                    QTreeWidgetItem* mItem = new QTreeWidgetItem(sItem);
                    mItem->setText(0, mcu);
                    mItem->setData(0, Qt::UserRole, MainForm::TypeMcu);
                }
            }
        }
    }
}

//------------------------------------------------------------------------------
// Реакция на выбор элемента в дереве устройств
//------------------------------------------------------------------------------
void MainForm::onDeviceTreeItemClicked(QTreeWidgetItem *item, int column)
{
    Q_UNUSED(column);

    if (!item) return;

    int type = item->data(0, Qt::UserRole).toInt();

    switch(type)
    {
        case TypeVendor:
//            ui->lineEditUrl->setText(pack.url());
//            ui->lineEditRelease->setText(pack.release());
//            ui->plainTextEditDescription->setPlainText(pack.description());
            break;

        case TypeFamily:
            break;

        case TypeSeries:
            break;

        case TypeMcu:
            showFeatures(item);
            break;
    }
}

//------------------------------------------------------------------------------
// Обновление дерева компонентов
//------------------------------------------------------------------------------
void MainForm::updateComponentsTree()
{
#if 0
    if(pack.coComponentMap().isEmpty()) return;

    ui->treeWidgetComponents->clear();

    for(auto& component : pack.coComponentMap())
    {
        if(!component.hasParents())
        {
            QTreeWidgetItem* item = new QTreeWidgetItem(ui->treeWidgetComponents);
            item->setText(0, component.getName());
            item->setText(1, component.getDescription());

            if(component.hasChildren())
            {
                updateComponentsTree(item, &component);
            }
        }
    }
#if 1
    ui->treeWidgetComponents->resizeColumnToContents(0);
    ui->treeWidgetComponents->setColumnWidth(0, ui->treeWidgetComponents->columnWidth(0) + 20);
#else
    ui->treeWidgetComponents->setColumnWidth(0, ui->treeWidgetComponents->width()/2);
#endif
#endif
}

//------------------------------------------------------------------------------
// Показать параметры микроконтроллера
//------------------------------------------------------------------------------
void MainForm::showFeatures(const QString &vendor,
                            const QString &core,
                            const QString &series,
                            const QString &mcu)
{
    if(vendor.isEmpty() || core.isEmpty() || series.isEmpty() || mcu.isEmpty())
    {
        return;
    }

    m_viewModel->selectVendor(vendor);
    m_viewModel->selectFamily(core);
    m_viewModel->selectSeries(series);
    m_viewModel->selectMcu(mcu);
}

//------------------------------------------------------------------------------
// Развернуть дерево устройств
//------------------------------------------------------------------------------
void MainForm::selectMcu(const QString &vendor,
                         const QString &family,
                         const QString &series,
                         const QString &mcu)
{
    // Поиск без полного разворачивания
    QTreeWidgetItem* vItem = findVendorItem(vendor);
    if (!vItem) return;

    ui->treeWidgetDevices->expandItem(vItem);  // развернуть производителя

    QTreeWidgetItem* fItem = findChildItem(vItem, family);
    if (!fItem) return;

    ui->treeWidgetDevices->expandItem(fItem);  // развернуть семейство

    QTreeWidgetItem* sItem = findChildItem(fItem, series);
    if (!sItem) return;

    ui->treeWidgetDevices->expandItem(sItem);  // развернуть серию

    QTreeWidgetItem* mItem = findChildItem(sItem, mcu);
    if (!mItem) return;

    ui->treeWidgetDevices->setCurrentItem(mItem);
    ui->treeWidgetDevices->scrollToItem(mItem);
}

//------------------------------------------------------------------------------
// Обновление дерева компонентов
//------------------------------------------------------------------------------
void MainForm::updateComponentsTree(QTreeWidgetItem *parentItem, Component *component)
{
    if(!parentItem || !component) return;

    foreach (Component * child, component->getChildren())
    {
        QTreeWidgetItem * item = new  QTreeWidgetItem(parentItem);
        item->setText(0, child->getName());
        item->setText(1, child->getDescription());

        if(child->hasChildren())
        {
            updateComponentsTree(item, child);
        }
//        else
//        {
//            foreach(QString mcu, child->supportedMcuList())
//            {
//                QTreeWidgetItem * mcuItem = new QTreeWidgetItem(item);
//                mcuItem->setText(0, mcu);
//            }
//        }
    }
}

//------------------------------------------------------------------------------
// Найти элемент дерева устройств, соответствующий имени производителя
//------------------------------------------------------------------------------
QTreeWidgetItem *MainForm::findVendorItem(const QString &vendor)
{
    for (int i = 0; i < ui->treeWidgetDevices->topLevelItemCount(); ++i)
    {
        QTreeWidgetItem* item = ui->treeWidgetDevices->topLevelItem(i);

        if (item->text(0) == vendor)
        {
            return item;
        }
    }
    return nullptr;
}

//------------------------------------------------------------------------------
// Найти дочерний элемент дерева устройств, содержащий текст
//------------------------------------------------------------------------------
QTreeWidgetItem *MainForm::findChildItem(QTreeWidgetItem *parent, const QString &text)
{
    for (int i = 0; i < parent->childCount(); ++i)
    {
        QTreeWidgetItem* item = parent->child(i);

        if (item->text(0) == text)
        {
            return item;
        }
    }
    return nullptr;
}
