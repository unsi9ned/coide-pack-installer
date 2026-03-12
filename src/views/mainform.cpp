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
        ui->pushButtonReload->setEnabled(false);
        ui->plainTextEditFeatures->clear();
        ui->plainTextEditDescription->clear();
    });

    connect(m_viewModel, &MainViewModel::loadFinished, [this]() {
        ui->lineEditRelease->setText(m_viewModel->releaseVersion());
        ui->pushButtonReload->setEnabled(true);
        ui->statusBar->showMessage("Готово", 3000);
    });

    connect(m_viewModel, &MainViewModel::loadFailed, [this](const QString& error) {
        QMessageBox::critical(this, "Ошибка", error);
        ui->pushButtonReload->setEnabled(true);
    });

    connect(m_viewModel, &MainViewModel::statusMessage,
            ui->statusBar, &QStatusBar::showMessage);

    //--------------------------------------------------------------------------
    // Сигналы установки
    //--------------------------------------------------------------------------
    connect(m_viewModel, &MainViewModel::installStarted, [this]() {
        ui->statusBar->showMessage("Установка...");
        ui->pushButtonInstall->setEnabled(false);  // блокируем кнопку
        // можно показать прогресс-бар
    });

    connect(m_viewModel, &MainViewModel::installFinished, [this](bool success, const QString& message) {
        ui->statusBar->showMessage(message, 3000);
        ui->pushButtonInstall->setEnabled(true);
        if (success) {
            QMessageBox::information(this, "Успех", message);
        }
    });

    connect(m_viewModel, &MainViewModel::installError, [this](const QString& error) {
        ui->statusBar->showMessage("Ошибка", 5000);
        ui->pushButtonInstall->setEnabled(true);
        QMessageBox::critical(this, "Ошибка", error);
    });

    connect(m_viewModel, &MainViewModel::installLogMessage, this, &MainForm::printLogMessages);

    //--------------------------------------------------------------------------
    // Отслеживаем процесс оптимизации
    //--------------------------------------------------------------------------
    connect(m_viewModel, &MainViewModel::dbOptimizeStarted, [this]() {
        ui->pushButtonDbOptimize->setEnabled(false);
        ui->statusBar->showMessage("Оптимизация БД...");
    });

    connect(m_viewModel, &MainViewModel::dbOptimizeFinished, [this]() {
        ui->pushButtonDbOptimize->setEnabled(true);
        ui->statusBar->showMessage("Оптимизация завершена", 3000);
    });

    connect(m_viewModel, &MainViewModel::dbOptimizeError, [this](const QString& error) {
        ui->pushButtonDbOptimize->setEnabled(true);
        ui->statusBar->showMessage("Ошибка: " + error, 5000);
        QMessageBox::warning(this, "Ошибка", error);
    });

    connect(m_viewModel, &MainViewModel::dbLogMessage, this, &MainForm::printLogMessages);

    //--------------------------------------------------------------------------
    // Обновление списков
    //--------------------------------------------------------------------------
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


    //--------------------------------------------------------------------------
    // Пользовательский ввод
    //--------------------------------------------------------------------------

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

    // Подключаем кнопку выбора пути к IDE к команде ViewModel
    connect(ui->pushButtonSetIdePath, &QPushButton::clicked, this, &MainForm::changeCoIDEPath);

    // Подключаем кнопку оптимизации БД к команде ViewModel
    connect(ui->pushButtonDbOptimize, &QPushButton::clicked, m_viewModel, &MainViewModel::optimizeDatabase);

    // Подключаем кнопку установки к команде ViewModel
    connect(ui->pushButtonInstall, &QPushButton::clicked, m_viewModel, &MainViewModel::installCurrentPack);

    // Подключаем кнопку перезагрузки данных к команде ViewModel
    connect(ui->pushButtonReload, SIGNAL(clicked(bool)), m_viewModel, SLOT(loadDeviceFamilyPack()));

    // Изменение пути к каталогу CoIDE
    connect(ui->actionPreferences, SIGNAL(triggered(bool)), SLOT(changeCoIDEPath()));

    // Выбор пакета DFP и загрузка
    connect(ui->actionOpen_DFP, SIGNAL(triggered(bool)), SLOT(loadDFP()));

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
void MainForm::loadDFP()
{
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
