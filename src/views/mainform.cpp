#include <QScrollBar>
#include <QTimer>
#include <QThread>
#include "mainform.h"
#include "ui_mainform.h"
#include "services/paths.h"
#include "utils/ziparchive.h"
#include "models/pack/packmanager.h"
#include "models/database/dbgarbagecollector.h"
#include "services/settings.h"
#include "utils/versionhelper.h"

#define ASYNC_VIEW_MODEL 0

//------------------------------------------------------------------------------
// Конструктор окна
//------------------------------------------------------------------------------
MainForm::MainForm(QWidget *parent) :
    QMainWindow(parent),
    //m_viewModel(new MainViewModel(this)),
    m_deviceViewModel(new DeviceViewModel(this)),
    m_mcuBrowserViewModel(new McuBrowserViewModel()),
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
#elif 0
    connect(m_deviceViewModel, &DeviceViewModel::deviceTreeChanged, [this](){
        qInfo()  << "deviceTreeChanged";
    });

    connect(m_deviceViewModel, &DeviceViewModel::mcuSelected, [this](){
        qInfo()  << "mcuSelected";
    });

    connect(m_deviceViewModel, &DeviceViewModel::loadStarted, [this]()
    {
        qInfo()  << "--------------------------------------------------------------------------";
        qInfo()  << "loadStarted";
    });

    connect(m_deviceViewModel, &DeviceViewModel::loadFinished, [this]()
    {
        qInfo()  << "loadFinished";
    });

    connect(m_deviceViewModel, &DeviceViewModel::loadError, [this](const QString& error)
    {
        qInfo()  << "loadError";
    });
#endif

    //--------------------------------------------------------------------------
    // Сигналы загрузки
    //--------------------------------------------------------------------------
#if 0
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
#elif ASYNC_VIEW_MODEL
    connect(m_deviceViewModel, &DeviceViewModel::loadStarted, [this]()
    {
        ui->statusBar->showMessage("Загрузка...");
        actionLoadDfp->setEnabled(false);
        ui->treeWidgetDevices->setEnabled(false);
        clearForm();
    });

    connect(m_deviceViewModel, &DeviceViewModel::loadFinished, [this]()
    {
        ui->lineEditRelease->setText(m_deviceViewModel->releaseVersion());
        actionLoadDfp->setEnabled(true);
        ui->treeWidgetDevices->setEnabled(true);
        ui->statusBar->showMessage("Готово", 3000);
        expandDeviceTree();
    });

    connect(m_deviceViewModel, &DeviceViewModel::loadError, [this](const QString& error)
    {
        QMessageBox::critical(this, "Ошибка", error);
        actionLoadDfp->setEnabled(true);
        ui->treeWidgetDevices->setEnabled(true);
    });


    connect(m_deviceViewModel, &DeviceViewModel::mcuLoadDetailsStarted, [this]()
    {
        ui->treeWidgetDevices->setEnabled(false);
    });

    connect(m_deviceViewModel, &DeviceViewModel::mcuLoadDetailsFinished, [this]()
    {
        ui->treeWidgetDevices->setEnabled(true);
    });

    connect(m_deviceViewModel, &DeviceViewModel::mcuLoadDetailsFailed, [this]()
    {
        ui->treeWidgetDevices->setEnabled(true);
    });
#endif
    //--------------------------------------------------------------------------
    // Сигналы установки
    //--------------------------------------------------------------------------
#if 0
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
#endif

    //--------------------------------------------------------------------------
    // Отслеживаем процесс оптимизации
    //--------------------------------------------------------------------------
#if 0
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
#endif
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
#elif ASYNC_VIEW_MODEL
    // Обновление дерева устройств
    connect(m_deviceViewModel, &DeviceViewModel::deviceTreeChanged, this, &MainForm::updateDeviceTree);

    // Обновление информации об устройстве
    connect(m_deviceViewModel, &DeviceViewModel::mcuSelected, this, &MainForm::showMcuDetails);
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

    // Подключаем выбор элемента дерева устройств
    connect(ui->treeWidgetDevices, &QTreeWidget::itemClicked, this, &MainForm::onDeviceTreeItemClicked);

#if 0
    // Подключаем кнопку оптимизации БД к команде ViewModel
    connect(actionDbOptimize, &QAction::triggered, m_viewModel, &MainViewModel::optimizeDatabase);

    // Подключаем кнопку установки к команде ViewModel
    connect(actionInstall, &QAction::triggered, m_viewModel, &MainViewModel::installCurrentPack);
#endif

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
    //delete m_viewModel;
    delete m_mcuBrowserViewModel;
    delete ui;
}

//------------------------------------------------------------------------------
// Инициализация программы после прогрузки основного окна
//------------------------------------------------------------------------------
void MainForm::delayedInit()
{
    loadDFP(true);
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
#if ASYNC_VIEW_MODEL
        //m_viewModel->loadDeviceFamilyPack();
        m_deviceViewModel->loadDeviceFamilyPack();
#else
        if(m_mcuBrowserViewModel->loadPack())
        {
            updateTreeFromModel();
            expandDeviceTree();
        }
#endif
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

#if ASYNC_VIEW_MODEL
        //m_viewModel->loadDeviceFamilyPack();
        m_deviceViewModel->loadDeviceFamilyPack();
#else
        if(m_mcuBrowserViewModel->loadPack())
        {
            updateTreeFromModel();
            expandDeviceTree();
        }
#endif
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
// Запросить данные о микроконтроллере
//------------------------------------------------------------------------------
void MainForm::requestMcuDetails(QTreeWidgetItem *item)
{
    if(item->data(0, Qt::UserRole).toInt() != MainForm::TypeMcu) return;

    QString vendor = item->parent()->parent()->parent()->text(0);
    QString armCore = item->parent()->parent()->text(0);
    QString series = item->parent()->text(0);
    QString mcu = item->text(0);

    m_deviceViewModel->selectNodeByPath(vendor, armCore, series, mcu);
}

//------------------------------------------------------------------------------
// Отображение подробной информации о выбранном микроконтроллере
//------------------------------------------------------------------------------
void MainForm::showMcuDetails()
{
#if ASYNC_VIEW_MODEL
    ui->lineEditFlashStart->setText(m_deviceViewModel->mcuDetails()->flashStart());
    ui->lineEditFlashSize->setText(m_deviceViewModel->mcuDetails()->flashSize());
    ui->lineEditRamStart->setText(m_deviceViewModel->mcuDetails()->ramStart());
    ui->lineEditRamSize->setText(m_deviceViewModel->mcuDetails()->ramSize());
    ui->plainTextEditFeatures->setPlainText(m_deviceViewModel->mcuDetails()->features());
    ui->plainTextEditDescription->setPlainText(m_deviceViewModel->mcuDetails()->description());
    ui->lineEditUrl->setText(m_deviceViewModel->mcuDetails()->webPageUrl());
    ui->lineEditDatasheetUrl->setText(m_deviceViewModel->mcuDetails()->datasheetUrl());
    ui->lineEditSVD->setText(m_deviceViewModel->mcuDetails()->svdLocalPath());

    // Заполнение списка алгоритмов отладки
    ui->comboBoxDebugAlg->clear();

    if(!m_deviceViewModel->mcuDetails()->debugAlgorithm().isEmpty())
    {
        ui->comboBoxDebugAlg->addItem(m_deviceViewModel->mcuDetails()->debugAlgorithm());
        ui->comboBoxDebugAlg->setCurrentIndex(0);
    }

    // Заполнение списка алгоритмов программирования
    ui->comboBoxFlashAlg->clear();

    for(int i = 0; i < m_deviceViewModel->mcuDetails()->flashAlgorithms().count(); i++)
    {
        QString algo = m_deviceViewModel->mcuDetails()->flashAlgorithms().at(i);

        ui->comboBoxFlashAlg->addItem(algo);

        if(algo == m_deviceViewModel->mcuDetails()->currentFlashAlgorithm())
        {
            ui->comboBoxFlashAlg->setCurrentIndex(i);
        }
    }
#else
    ui->lineEditRelease->setText(m_mcuBrowserViewModel->releaseVersion());
    ui->lineEditFlashStart->setText(m_mcuBrowserViewModel->flashStart());
    ui->lineEditFlashSize->setText(m_mcuBrowserViewModel->flashSize());
    ui->lineEditRamStart->setText(m_mcuBrowserViewModel->ramStart());
    ui->lineEditRamSize->setText(m_mcuBrowserViewModel->ramSize());
    ui->plainTextEditFeatures->setPlainText(m_mcuBrowserViewModel->features());
    ui->plainTextEditDescription->setPlainText(m_mcuBrowserViewModel->description());
    ui->lineEditUrl->setText(m_mcuBrowserViewModel->webPageUrl());
    ui->lineEditDatasheetUrl->setText(m_mcuBrowserViewModel->datasheetUrl());
    ui->lineEditSVD->setText(m_mcuBrowserViewModel->svdLocalPath());

    // Заполнение списка алгоритмов отладки
    ui->comboBoxDebugAlg->clear();

    if(!m_mcuBrowserViewModel->debugAlgorithm().isEmpty())
    {
        ui->comboBoxDebugAlg->addItem(m_mcuBrowserViewModel->debugAlgorithm());
        ui->comboBoxDebugAlg->setCurrentIndex(0);
    }

    // Заполнение списка алгоритмов программирования
    ui->comboBoxFlashAlg->clear();

    for(int i = 0; i < m_mcuBrowserViewModel->flashAlgorithms().count(); i++)
    {
        QString algo = m_mcuBrowserViewModel->flashAlgorithms().at(i);

        ui->comboBoxFlashAlg->addItem(algo);

        if(algo == m_mcuBrowserViewModel->defaultFlashAlgorithm())
        {
            ui->comboBoxFlashAlg->setCurrentIndex(i);
        }
    }
#endif
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

    for (auto& vNode : m_deviceViewModel->deviceTree())
    {
        QTreeWidgetItem* vItem = new QTreeWidgetItem(ui->treeWidgetDevices);
        vItem->setText(0, vNode.name);
        vItem->setData(0, Qt::UserRole, vNode.type);

        for (auto& fNode : vNode.children)
        {
            QTreeWidgetItem* fItem = new QTreeWidgetItem(vItem);
            fItem->setText(0, fNode.name);
            fItem->setData(0, Qt::UserRole, fNode.type);

            for (auto& sNode : fNode.children)
            {
                QTreeWidgetItem* sItem = new QTreeWidgetItem(fItem);
                sItem->setText(0, sNode.name);
                sItem->setData(0, Qt::UserRole, sNode.type);

                for (auto& mNode : sNode.children)
                {
                    QTreeWidgetItem* mItem = new QTreeWidgetItem(sItem);
                    mItem->setText(0, mNode.name);
                    mItem->setData(0, Qt::UserRole, mNode.type);
                }
            }
        }
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
// Реакция на выбор элемента в дереве устройств
//------------------------------------------------------------------------------
void MainForm::onDeviceTreeItemClicked(QTreeWidgetItem *item, int column)
{
    Q_UNUSED(column);

    if (!item) return;

#if ASYNC_VIEW_MODEL
    int type = item->data(0, Qt::UserRole).toInt();

    switch(type)
    {
        case TypeVendor:
#if 0
            ui->lineEditUrl->setText(pack.url());
            ui->lineEditRelease->setText(pack.release());
            ui->plainTextEditDescription->setPlainText(pack.description());
#endif
            break;

        case TypeFamily:
            break;

        case TypeSeries:
            break;

        case TypeMcu:
            requestMcuDetails(item);
            break;
    }
#else
    DeviceNode node = item->data(0, Qt::UserRole).value<DeviceNode>();

    // 1. Обновляем модель
    m_mcuBrowserViewModel->selectNode(node);

    // 2. Обновляем информацию об MCU
    showMcuDetails();

#endif
}

//------------------------------------------------------------------------------
// Построение дерева устройств
//------------------------------------------------------------------------------
void MainForm::updateTreeFromModel()
{
    ui->treeWidgetDevices->clear();

    const auto& tree = m_mcuBrowserViewModel->deviceTree();

    for (const auto& node : tree)
    {
        createTreeItem(node, nullptr);
    }

    // Разворачиваем до первого уровня
    for (int i = 0; i < ui->treeWidgetDevices->topLevelItemCount(); ++i)
    {
        ui->treeWidgetDevices->topLevelItem(i)->setExpanded(true);
    }
}

//------------------------------------------------------------------------------
// Создание элемента дерева устройств
//------------------------------------------------------------------------------
QTreeWidgetItem*MainForm::createTreeItem(const DeviceNode& node, QTreeWidgetItem* parent)
{
    QTreeWidgetItem* item = parent ?
                            new QTreeWidgetItem(parent) :
                            new QTreeWidgetItem(ui->treeWidgetDevices);

    item->setText(0, node.displayName);
    item->setData(0, Qt::UserRole, QVariant::fromValue(node));

    // Иконки для разных типов
    QIcon icon;

    switch (node.type)
    {
        case DeviceNode::VendorType: icon = style()->standardIcon(QStyle::SP_ComputerIcon); break;
        case DeviceNode::FamilyType: icon = style()->standardIcon(QStyle::SP_DirIcon); break;
        case DeviceNode::SeriesType: icon = style()->standardIcon(QStyle::SP_FileIcon); break;
        case DeviceNode::McuType: icon = style()->standardIcon(QStyle::SP_FileLinkIcon); break;
        default: break;
    }

    if (!icon.isNull())
    {
        item->setIcon(0, icon);
    }

    for (const auto& child : node.children)
    {
        createTreeItem(child, item);
    }

    return item;
}

//------------------------------------------------------------------------------
// Развернуть дерево устройств
//------------------------------------------------------------------------------
void MainForm::expandDeviceTree()
{
#if ASYNC_VIEW_MODEL
    QString vendor = m_deviceViewModel->currentVendor();
    QString family = m_deviceViewModel->currentFamily();
    QString series = m_deviceViewModel->currentSeries();
    QString mcu = m_deviceViewModel->currentMcu();
#else
    if(!m_mcuBrowserViewModel->selectedNode().isValid()) return;

    QString vendor = m_mcuBrowserViewModel->selectedVendor();
    QString family = m_mcuBrowserViewModel->selectedFamily();
    QString series = m_mcuBrowserViewModel->selectedSeries();
    QString mcu = m_mcuBrowserViewModel->selectedMcu();
#endif

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

    if(m_mcuBrowserViewModel->selectedNode().isMcu())
    {
        showMcuDetails();
    }
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
