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
#include "common/version.h"
#include "services/logger.h"

//------------------------------------------------------------------------------
// Конструктор окна
//------------------------------------------------------------------------------
MainForm::MainForm(QWidget *parent) :
    QMainWindow(parent),
    m_mcuBrowserViewModel(new McuBrowserViewModel(this)),
    aboutProgDialog(new AboutDialog(this)),
    ui(new Ui::MainForm)
{
    ui->setupUi(this);
    setupLogging();

    // Иконка
    this->setWindowIcon(QIcon(":/img/hamlab.ico"));
    this->setWindowTitle(QString("%1 (v%2)").arg(APP_NAME).arg(APP_VERSION));

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
    ui->lineEditIdePath->setText(Paths::instance()->coIdeDir());
    ui->plainTextEditLog->setContextMenuPolicy(Qt::CustomContextMenu);

    //--------------------------------------------------------------------------
    // Добавление кнопки очистки в контекстное меню окна логирования
    //--------------------------------------------------------------------------
    connect(ui->plainTextEditLog, &QPlainTextEdit::customContextMenuRequested, [this](const QPoint &pos)
    {
        QMenu *menu = ui->plainTextEditLog->createStandardContextMenu();

        menu->addSeparator();

        QAction *clearAction = menu->addAction("Clear");
        clearAction->setIcon(style()->standardIcon(QStyle::SP_DialogResetButton));

        connect(clearAction, &QAction::triggered,
                ui->plainTextEditLog, &QPlainTextEdit::clear);

        menu->exec(ui->plainTextEditLog->mapToGlobal(pos));
        delete menu;
    });

    //--------------------------------------------------------------------------
    // Сигналы загрузки
    //--------------------------------------------------------------------------
    connect(m_mcuBrowserViewModel, &McuBrowserViewModel::loadStarted, [this]()
    {
        ui->statusBar->showMessage("Загрузка...");
        clearForm();
        lockUI(true);
    });

    connect(m_mcuBrowserViewModel, &McuBrowserViewModel::packLoaded, [this](bool success)
    {
        lockUI(false);

        if(success)
        {
            ui->lineEditRelease->setText(m_mcuBrowserViewModel->releaseVersion());
            ui->statusBar->showMessage("Готово", 3000);

            updateTreeFromModel();
            updateComponentsTree();
            expandDeviceTree();
        }
        else
        {
            ui->statusBar->showMessage("Ошибка загрузки пакета", 3000);
        }
    });

    //--------------------------------------------------------------------------
    // Сигналы установки
    //--------------------------------------------------------------------------
    connect(m_mcuBrowserViewModel, &McuBrowserViewModel::installStarted, [this]()
    {
        lockUI(true);
        ui->statusBar->showMessage("Установка...");
    });

    connect(m_mcuBrowserViewModel, &McuBrowserViewModel::packInstalled, [this](bool success, QString e)
    {
        lockUI(false);

        if(success)
        {
            ui->statusBar->showMessage("Готово", 3000);
            QMessageBox::information(this, "Успех", tr("Пакет успешно установлен"));
        }
        else
        {
            ui->statusBar->showMessage("Ошибка", 5000);
            QMessageBox::critical(this, "Ошибка", e);
        }
    });

    //--------------------------------------------------------------------------
    // Отслеживаем процесс оптимизации
    //--------------------------------------------------------------------------
    connect(m_mcuBrowserViewModel, &McuBrowserViewModel::dbOptimizeStarted, [this]()
    {
        lockUI(true);
        ui->statusBar->showMessage("Оптимизация БД...");
    });

    connect(m_mcuBrowserViewModel, &McuBrowserViewModel::dbOptimizeFinished, [this]() {
        lockUI(false);
        ui->statusBar->showMessage("Оптимизация завершена", 3000);
    });

    //--------------------------------------------------------------------------
    // Пользовательский ввод
    //--------------------------------------------------------------------------

    // Подключаем выбор элемента дерева устройств
    connect(ui->treeWidgetDevices, &QTreeWidget::itemClicked, this, &MainForm::onDeviceTreeItemClicked);

    // Подключаем выбор элемента дерева устройств
    connect(ui->treeWidgetComponents, &QTreeWidget::itemClicked, this, &MainForm::onComponentTreeItemClicked);

    // Подключаем кнопку оптимизации БД к команде ViewModel
    connect(actionDbOptimize, &QAction::triggered, m_mcuBrowserViewModel, &McuBrowserViewModel::optimizeDatabase);

    // Подключаем кнопку установки к команде ViewModel
    connect(actionInstall, &QAction::triggered, m_mcuBrowserViewModel, &McuBrowserViewModel::installCurrentPack);

    // Подключаем кнопку перезагрузки данных к команде ViewModel
    connect(actionReloadDfp, &QAction::triggered, [this]()
    {
        m_mcuBrowserViewModel->saveSelection();
        loadDFP(true);
    });

    // Выбор элемента в списке алгоритмов программирования
    connect(ui->comboBoxFlashAlg, QOverload<int>::of(&QComboBox::activated), [this]()
    {
        QString path = m_mcuBrowserViewModel->flashAlgorithmPath(ui->comboBoxFlashAlg->currentText());
        QString uid = m_mcuBrowserViewModel->flashAlgorithmId(ui->comboBoxFlashAlg->currentText());
        ui->lineEditUniquePath->setText(path);
        ui->lineEditFlashAlgoId->setText(uid);
    });

    // Изменение пути к каталогу CoIDE
    connect(ui->pushButtonSetIdePath, &QPushButton::clicked, this, &MainForm::changeCoIDEPath);
    connect(ui->actionPreferences, SIGNAL(triggered(bool)), SLOT(changeCoIDEPath()));
    connect(actionSettings, &QAction::triggered, this, &MainForm::changeCoIDEPath);

    // Выбор пакета DFP и загрузка
    connect(actionLoadDfp, &QAction::triggered, this, &MainForm::loadDFP);
    connect(ui->actionOpen_DFP, &QAction::triggered, this, &MainForm::loadDFP);

    // Вызов окна "О программе"
    connect(ui->actionAbout, &QAction::triggered, aboutProgDialog, &AboutDialog::show);

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
    delete m_mcuBrowserViewModel;
    delete ui;
}

//------------------------------------------------------------------------------
// Настройка логирования
//------------------------------------------------------------------------------
void MainForm::setupLogging()
{
    connect(Logger::instance(), &Logger::messageLogged, this, &MainForm::onLogMessage);
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
        m_mcuBrowserViewModel->loadPackAsync();
        return;
    }

    QFileInfo packFileInfo(Settings::instance()->lastLoadedPack());
    QFileDialog dialog(this, tr("Device Family Pack"), packFileInfo.path(), "*.jpack *.pack");

    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setOption(QFileDialog::DontUseNativeDialog);

    if (dialog.exec())
    {
        QString path = dialog.selectedFiles().first();
        Settings::instance()->saveLastLoadedPack(path);

        m_mcuBrowserViewModel->loadPackAsync();
    }
}

//------------------------------------------------------------------------------
// Печать сообщений об ошибках и событиях
//------------------------------------------------------------------------------
void MainForm::printLogMessages(QString msg)
{
    ui->plainTextEditLog->appendPlainText(msg);
    ui->plainTextEditLog->verticalScrollBar()->setValue(ui->plainTextEditLog->verticalScrollBar()->maximum());
}

void MainForm::onLogMessage(const LogMessage& msg)
{
    // Вывод в текстовое поле
    ui->plainTextEditLog->appendHtml(msg.toHtml());

    // Автопрокрутка вниз
    QTextCursor cursor = ui->plainTextEditLog->textCursor();
    cursor.movePosition(QTextCursor::End);
    ui->plainTextEditLog->setTextCursor(cursor);

    // Критические ошибки в статусбар
    if (msg.level == LogLevel::Error || msg.level == LogLevel::Critical) {
        statusBar()->showMessage(msg.message, 5000);
    }
}

//------------------------------------------------------------------------------
// Отображение подробной информации о выбранном микроконтроллере
//------------------------------------------------------------------------------
void MainForm::showMcuDetails()
{
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

    // Уникальные идентификаторы
    ui->lineEditVendorId->setText(m_mcuBrowserViewModel->vendorId());
    ui->lineEditFamilyId->setText(m_mcuBrowserViewModel->familyId());
    ui->lineEditSeriesId->setText(m_mcuBrowserViewModel->seriesId());
    ui->lineEditMcuId->setText(m_mcuBrowserViewModel->mcuId());

    // Уникальный путь
    ui->lineEditUniquePath->setText(m_mcuBrowserViewModel->devNodePath());
    ui->lineEditUniquePath->setCursorPosition(0);
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
    ui->lineEditVendorId->clear();
    ui->lineEditFamilyId->clear();
    ui->lineEditSeriesId->clear();
    ui->lineEditMcuId->clear();
    ui->lineEditComponentId->clear();
    ui->lineEditUniquePath->clear();
    ui->lineEditFlashAlgoId->clear();
}

//------------------------------------------------------------------------------
// Блокировка интерфейса для безопасного выполнения критических операций
//------------------------------------------------------------------------------
void MainForm::lockUI(bool enabled)
{
    enabled = !enabled;
    ui->treeWidgetDevices->setEnabled(enabled);
    ui->treeWidgetComponents->setEnabled(enabled);
    actionLoadDfp->setEnabled(enabled);
    ui->actionOpen_DFP->setEnabled(enabled);
    actionReloadDfp->setEnabled(enabled);
    actionDbOptimize->setEnabled(enabled);
    actionInstall->setEnabled(enabled);
    ui->pushButtonSetIdePath->setEnabled(enabled);
    ui->actionPreferences->setEnabled(enabled);
    actionSettings->setEnabled(enabled);
}

//------------------------------------------------------------------------------
// Реакция на выбор элемента в дереве устройств
//------------------------------------------------------------------------------
void MainForm::onDeviceTreeItemClicked(QTreeWidgetItem *item, int column)
{
    Q_UNUSED(column);

    if (!item) return;

    DeviceNode node = item->data(0, Qt::UserRole).value<DeviceNode>();

    // 1. Обновляем модель
    m_mcuBrowserViewModel->selectNode(node);

    // 2. Обновляем информацию об MCU
    showMcuDetails();
}

//------------------------------------------------------------------------------
// Реакция на выбор элемента в дереве компонентов
//------------------------------------------------------------------------------
void MainForm::onComponentTreeItemClicked(QTreeWidgetItem* item, int column)
{
    Q_UNUSED(column);

    if (!item) return;

    ComponentNode node = item->data(0, Qt::UserRole).value<ComponentNode>();

    m_mcuBrowserViewModel->selectComponentNode(node);
    ui->lineEditComponentId->setText(m_mcuBrowserViewModel->componentId());
    ui->lineEditUniquePath->setText(m_mcuBrowserViewModel->componentNodePath());
    ui->lineEditUniquePath->setCursorPosition(0);
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
    if(!m_mcuBrowserViewModel->selectedNode().isValid()) return;

    QString vendor = m_mcuBrowserViewModel->selectedVendor();
    QString family = m_mcuBrowserViewModel->selectedFamily();
    QString series = m_mcuBrowserViewModel->selectedSeries();
    QString mcu = m_mcuBrowserViewModel->selectedMcu();

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

//------------------------------------------------------------------------------
// Обновление дерева компонентов
//------------------------------------------------------------------------------
void MainForm::updateComponentsTree()
{
    ui->treeWidgetComponents->clear();

    const auto& tree = m_mcuBrowserViewModel->componentTree();

    for (const auto& node : tree)
    {
        createComponentTreeItem(node, nullptr);
    }

    // Разворачиваем до первого уровня
    for (int i = 0; i < ui->treeWidgetComponents->topLevelItemCount(); ++i)
    {
        ui->treeWidgetComponents->topLevelItem(i)->setExpanded(true);
    }

    ui->treeWidgetComponents->resizeColumnToContents(0);
    ui->treeWidgetComponents->setColumnWidth(0, ui->treeWidgetComponents->columnWidth(0) + 20);
}

//------------------------------------------------------------------------------
// Создание элемента дерева компонентов
//------------------------------------------------------------------------------
QTreeWidgetItem *MainForm::createComponentTreeItem(const ComponentNode &node,
                                                   QTreeWidgetItem *parent)
{
    QTreeWidgetItem* item = parent ?
                            new QTreeWidgetItem(parent) :
                            new QTreeWidgetItem(ui->treeWidgetComponents);

    item->setText(0, node.name);
    item->setData(0, Qt::UserRole, QVariant::fromValue(node));
    item->setText(1, node.description);

    // Иконки
    if(node.hasChildren())
        item->setIcon(0, style()->standardIcon(QStyle::SP_DirIcon));
    else
        item->setIcon(0, style()->standardIcon(QStyle::SP_FileIcon));

    for (const auto& child : node.children)
    {
        createComponentTreeItem(child, item);
    }

    return item;
}

